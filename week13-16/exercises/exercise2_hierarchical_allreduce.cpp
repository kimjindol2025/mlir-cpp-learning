/**
 * Exercise 2: Hierarchical AllReduce (계층적 전역 축약)
 *
 * 학습 목표:
 * - AllReduce 알고리즘의 여러 구현 방식
 * - 계층적 아키텍처 설계
 * - 통신 시간 분석
 * - 네트워크 토폴로지 기반 최적화
 *
 * 난이도: ⭐⭐⭐⭐ 최고급 (Graduate Level)
 *
 * 배경: 분산 머신러닝에서 AllReduce는 성능 병목
 *      1000배 성능 개선이 가능한 핵심 최적화
 * 목표: 계층 구조를 활용하여 통신 시간 단축
 */

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <numeric>

using namespace std;

/**
 * AllReduce 알고리즘 타입
 */
enum class AllReduceAlgorithm {
    Butterfly,          // 나비 알고리즘
    Ring,              // 고리 알고리즘
    Tree,              // 트리 알고리즘
    HierarchicalTree   // 계층 트리 (최적)
};

/**
 * 프로세스/노드 정보
 */
struct Rank {
    int id;
    string name;
    vector<float> data;  // 로컬 데이터

    Rank(int id, const string& name, int dataSize)
        : id(id), name(name), data(dataSize, 0.0f) {}

    void initData(float value) {
        fill(data.begin(), data.end(), value + id * 0.1f);
    }

    float reduce() const {
        return accumulate(data.begin(), data.end(), 0.0f);
    }
};

/**
 * 네트워크 토폴로지
 */
struct NetworkTopology {
    int rank_count;
    vector<vector<float>> latency;     // 지연 시간 (ms)
    vector<vector<float>> bandwidth;   // 대역폭 (GB/s)

    NetworkTopology(int n)
        : rank_count(n),
          latency(n, vector<float>(n, 1.0f)),
          bandwidth(n, vector<float>(n, 10.0f)) {
        // 대각선은 0 (자기 자신과의 통신)
        for (int i = 0; i < n; i++) {
            latency[i][i] = 0;
            bandwidth[i][i] = 1000;  // 무제한
        }
    }

    /**
     * 계층 토폴로지 설정 (예: 8개 노드, 2단계)
     * Level 0: 로컬 그룹 (4노드씩)
     * Level 1: 글로벌 (그룹 대표)
     */
    void setHierarchicalTopology(int groupSize) {
        cout << "계층 토폴로지 설정 (그룹 크기: " << groupSize << ")" << endl;

        for (int i = 0; i < rank_count; i++) {
            for (int j = 0; j < rank_count; j++) {
                if (i == j) {
                    latency[i][j] = 0;
                    bandwidth[i][j] = 1000;
                    continue;
                }

                int groupI = i / groupSize;
                int groupJ = j / groupSize;

                if (groupI == groupJ) {
                    // 같은 그룹: 빠른 통신
                    latency[i][j] = 0.1f;
                    bandwidth[i][j] = 100.0f;  // 100GB/s
                } else {
                    // 다른 그룹: 느린 통신
                    latency[i][j] = 1.0f;
                    bandwidth[i][j] = 10.0f;   // 10GB/s
                }
            }
        }

        cout << "  로컬 통신: 0.1ms, 100GB/s" << endl;
        cout << "  글로벌 통신: 1.0ms, 10GB/s" << endl;
    }

    /**
     * 통신 시간 계산
     */
    float getCommTime(int from, int to, long dataSize) const {
        // time = latency + dataSize / bandwidth
        // dataSize (bytes) / bandwidth (GB/s) = time (ms)
        // 1 GB/s = 10^9 bytes/s = 10^6 bytes/ms
        float transferTime = (float)dataSize / 1e6 / bandwidth[from][to];
        return latency[from][to] + transferTime;
    }
};

/**
 * AllReduce 구현체
 */
class AllReduceImpl {
protected:
    int rankCount;
    long dataSize;  // bytes
    NetworkTopology& topology;

    virtual float simulate() = 0;

public:
    AllReduceImpl(int n, long size, NetworkTopology& topo)
        : rankCount(n), dataSize(size), topology(topo) {}

    virtual ~AllReduceImpl() {}

    float getExecutionTime() {
        return simulate();
    }

    virtual string getName() const = 0;
};

/**
 * 나비 AllReduce (Butterfly AllReduce)
 * - 단계: log2(n)
 * - 각 단계: 모든 노드가 통신
 */
class ButterflyAllReduce : public AllReduceImpl {
public:
    ButterflyAllReduce(int n, long size, NetworkTopology& topo)
        : AllReduceImpl(n, size, topo) {}

    string getName() const override { return "Butterfly"; }

private:
    float simulate() override {
        float totalTime = 0;
        int stages = (int)log2(rankCount);

        cout << "\nButterfly AllReduce (" << stages << " stages):" << endl;

        for (int stage = 0; stage < stages; stage++) {
            float stageTime = 0;
            int stride = 1 << stage;

            // 각 노드 쌍이 통신
            for (int i = 0; i < rankCount; i += stride * 2) {
                int j = i + stride;
                if (j < rankCount) {
                    float commTime = topology.getCommTime(i, j, dataSize);
                    stageTime = max(stageTime, commTime);
                }
            }

            cout << "  Stage " << stage << ": " << fixed << setprecision(2)
                 << stageTime << "ms" << endl;
            totalTime += stageTime;
        }

        return totalTime;
    }
};

/**
 * 고리 AllReduce (Ring AllReduce)
 * - 단계: 2(n-1)
 * - 각 노드: 두 이웃과만 통신
 * - 장점: 각 링크의 대역폭 완전 활용
 */
class RingAllReduce : public AllReduceImpl {
public:
    RingAllReduce(int n, long size, NetworkTopology& topo)
        : AllReduceImpl(n, size, topo) {}

    string getName() const override { return "Ring"; }

private:
    float simulate() override {
        float totalTime = 0;
        int stages = 2 * (rankCount - 1);
        float stageTime = 0;

        cout << "\nRing AllReduce (" << stages << " stages):" << endl;

        // 모든 노드가 순서대로 이웃과 통신
        for (int step = 0; step < stages; step++) {
            for (int i = 0; i < rankCount; i++) {
                int next = (i + 1) % rankCount;
                float commTime = topology.getCommTime(i, next, dataSize);
                stageTime = max(stageTime, commTime);
            }
        }

        cout << "  단계 당: " << fixed << setprecision(2) << stageTime << "ms"
             << endl;
        cout << "  총 단계: " << stages << endl;

        return stageTime * stages;
    }
};

/**
 * 트리 AllReduce
 * - 단계: log2(n)
 * - 구조: 바이너리 트리
 */
class TreeAllReduce : public AllReduceImpl {
public:
    TreeAllReduce(int n, long size, NetworkTopology& topo)
        : AllReduceImpl(n, size, topo) {}

    string getName() const override { return "Tree"; }

private:
    float simulate() override {
        float totalTime = 0;
        int stages = (int)log2(rankCount);

        cout << "\nTree AllReduce (" << stages << " stages):" << endl;

        for (int stage = 0; stage < stages; stage++) {
            float stageTime = 0;
            int stride = 1 << stage;

            // Reduce 단계: 자식 노드가 부모로 데이터 전송
            for (int parent = 0; parent < rankCount; parent += stride * 2) {
                int child = parent + stride;
                if (child < rankCount) {
                    float commTime = topology.getCommTime(child, parent,
                                                         dataSize);
                    stageTime = max(stageTime, commTime);
                }
            }

            // Broadcast 단계: 부모가 자식으로 데이터 전송
            for (int parent = 0; parent < rankCount; parent += stride * 2) {
                int child = parent + stride;
                if (child < rankCount) {
                    float commTime = topology.getCommTime(parent, child,
                                                         dataSize);
                    stageTime = max(stageTime, commTime);
                }
            }

            cout << "  Stage " << stage << ": " << fixed << setprecision(2)
                 << stageTime << "ms" << endl;
            totalTime += stageTime;
        }

        return totalTime;
    }
};

/**
 * 계층 트리 AllReduce (최적화)
 *
 * 구조:
 * - Level 0: 로컬 그룹 내 AllReduce (빠름)
 * - Level 1: 그룹 대표들의 AllReduce (느림)
 * - Level 2: 결과를 그룹으로 Broadcast (느림)
 */
class HierarchicalTreeAllReduce : public AllReduceImpl {
private:
    int groupSize;

public:
    HierarchicalTreeAllReduce(int n, long size, NetworkTopology& topo,
                             int gSize = 4)
        : AllReduceImpl(n, size, topo), groupSize(gSize) {}

    string getName() const override {
        return "Hierarchical (group=" + to_string(groupSize) + ")";
    }

private:
    float simulate() override {
        float totalTime = 0;
        int groupCount = (rankCount + groupSize - 1) / groupSize;

        cout << "\nHierarchical Tree AllReduce:" << endl;
        cout << "  그룹 크기: " << groupSize << endl;
        cout << "  그룹 수: " << groupCount << endl;

        // Level 0: 각 그룹 내 AllReduce
        cout << "\n  Level 0: 로컬 그룹 AllReduce" << endl;
        float localTime = 0;
        for (int g = 0; g < groupCount; g++) {
            float groupTime = 0;

            // 그룹 내 모든 노드 쌍 통신
            int groupStart = g * groupSize;
            int groupEnd = min(groupStart + groupSize, rankCount);

            for (int i = groupStart; i < groupEnd; i++) {
                for (int j = i + 1; j < groupEnd; j++) {
                    float commTime = topology.getCommTime(i, j, dataSize);
                    groupTime = max(groupTime, commTime);
                }
            }

            cout << "    Group " << g << ": " << fixed << setprecision(2)
                 << groupTime << "ms" << endl;
            localTime = max(localTime, groupTime);
        }
        totalTime += localTime;

        // Level 1: 그룹 대표들의 AllReduce
        cout << "\n  Level 1: 글로벌 AllReduce (대표들)" << endl;
        float globalTime = 0;
        for (int g1 = 0; g1 < groupCount; g1++) {
            for (int g2 = g1 + 1; g2 < groupCount; g2++) {
                int rep1 = g1 * groupSize;
                int rep2 = g2 * groupSize;
                float commTime = topology.getCommTime(rep1, rep2, dataSize);
                globalTime = max(globalTime, commTime);
            }
        }
        cout << "    " << fixed << setprecision(2) << globalTime << "ms"
             << endl;
        totalTime += globalTime;

        // Level 2: Broadcast
        cout << "\n  Level 2: Broadcast to groups" << endl;
        float broadcastTime = globalTime;  // 유사한 시간
        cout << "    " << fixed << setprecision(2) << broadcastTime << "ms"
             << endl;
        totalTime += broadcastTime;

        cout << "\n  합계: " << fixed << setprecision(2) << totalTime << "ms"
             << endl;

        return totalTime;
    }
};

// ============================================================================
// 테스트
// ============================================================================

/**
 * 테스트 1: 알고리즘 비교 (8 노드)
 */
void testAlgorithmComparison() {
    cout << "\n=== 테스트 1: AllReduce 알고리즘 비교 (8 노드) ===" << endl;

    NetworkTopology topology(8);
    long dataSize = 100000000;  // 100MB

    cout << "네트워크: 균일한 지연시간, 10GB/s 대역폭" << endl;
    cout << "데이터 크기: " << dataSize / 1e6 << "MB" << endl;

    vector<unique_ptr<AllReduceImpl>> algos;
    algos.push_back(make_unique<ButterflyAllReduce>(8, dataSize, topology));
    algos.push_back(make_unique<RingAllReduce>(8, dataSize, topology));
    algos.push_back(make_unique<TreeAllReduce>(8, dataSize, topology));
    algos.push_back(make_unique<HierarchicalTreeAllReduce>(8, dataSize,
                                                           topology, 4));

    cout << "\n실행 결과:" << endl;
    cout << "─────────────────────────────────" << endl;

    for (auto& algo : algos) {
        float time = algo->getExecutionTime();
        cout << setw(25) << (algo->getName() + ":") << fixed << setprecision(2)
             << time << "ms" << endl;
    }
}

/**
 * 테스트 2: 계층 토폴로지의 영향
 */
void testHierarchicalTopology() {
    cout << "\n=== 테스트 2: 계층 토폴로지 효과 ===" << endl;

    long dataSize = 100000000;

    // 균일한 네트워크
    {
        cout << "\n📊 균일한 네트워크 (모든 링크 동일):" << endl;
        NetworkTopology topo1(8);

        HierarchicalTreeAllReduce hier(8, dataSize, topo1, 4);
        cout << "실행 시간: " << fixed << setprecision(2)
             << hier.getExecutionTime() << "ms" << endl;
    }

    // 계층 토폴로지
    {
        cout << "\n📊 계층 토폴로지 (그룹 내 빠름, 그룹 간 느림):" << endl;
        NetworkTopology topo2(8);
        topo2.setHierarchicalTopology(4);

        HierarchicalTreeAllReduce hier(8, dataSize, topo2, 4);
        cout << "실행 시간: " << fixed << setprecision(2)
             << hier.getExecutionTime() << "ms" << endl;
    }
}

/**
 * 테스트 3: 규모 확장성 (노드 수 증가)
 */
void testScalability() {
    cout << "\n=== 테스트 3: 규모 확장성 분석 ===" << endl;

    long dataSize = 100000000;

    cout << "노드 수 증가에 따른 성능:" << endl;
    cout << "─────────────────────────────────────────────" << endl;
    cout << "Nodes  | Ring      | Tree      | Hierarchical" << endl;
    cout << "─────────────────────────────────────────────" << endl;

    for (int n : {4, 8, 16, 32, 64}) {
        NetworkTopology topo(n);

        RingAllReduce ring(n, dataSize, topo);
        float ringTime = ring.getExecutionTime();

        TreeAllReduce tree(n, dataSize, topo);
        float treeTime = tree.getExecutionTime();

        int groupSize = (int)sqrt(n);
        HierarchicalTreeAllReduce hier(n, dataSize, topo, groupSize);
        float hierTime = hier.getExecutionTime();

        cout << setw(6) << n << " | " << fixed << setprecision(1)
             << setw(8) << ringTime << " | "
             << setw(8) << treeTime << " | "
             << setw(11) << hierTime << endl;
    }
}

/**
 * 테스트 4: 대규모 클러스터 최적화
 */
void testLargeScaleOptimization() {
    cout << "\n=== 테스트 4: 대규모 클러스터 최적화 (256 노드) ===" << endl;

    int rankCount = 256;
    long dataSize = 1000000000;  // 1GB

    NetworkTopology topo(rankCount);
    topo.setHierarchicalTopology(16);  // 16개 노드씩 그룹

    cout << "구성: 256 노드, 16개 노드씩 16 그룹" << endl;
    cout << "데이터 크기: 1GB" << endl;

    HierarchicalTreeAllReduce hier(rankCount, dataSize, topo, 16);
    float time = hier.getExecutionTime();

    cout << "\n실행 시간: " << fixed << setprecision(2) << time << "ms" << endl;

    // 성능 추정
    long totalData = (long)rankCount * dataSize;
    float effectiveBandwidth = totalData / (time * 1e6);
    cout << "실효 대역폭: " << effectiveBandwidth << "GB/s" << endl;
}

int main() {
    cout << "╔═══════════════════════════════════════╗" << endl;
    cout << "║  Exercise 2: Hierarchical AllReduce   ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl;

    try {
        testAlgorithmComparison();
        testHierarchicalTopology();
        testScalability();
        testLargeScaleOptimization();

        cout << "\n╔═══════════════════════════════════════╗" << endl;
        cout << "║  ✅ 모든 테스트 완료!                  ║" << endl;
        cout << "╚═══════════════════════════════════════╝" << endl;

        return 0;
    } catch (const exception& e) {
        cerr << "오류: " << e.what() << endl;
        return 1;
    }
}

/**
 * 학습 포인트:
 *
 * 1. AllReduce 알고리즘:
 *    - Butterfly: O(log n) 단계, 모든 링크 사용
 *    - Ring: 각 링크 대역폭 100% 활용
 *    - Tree: 계층 구조, 낮은 지연
 *    - Hierarchical: 토폴로지 활용 최적
 *
 * 2. 계층 토폴로지 설계:
 *    - Level 0: 로컬 그룹 (빠른 통신)
 *    - Level 1: 글로벌 (느린 통신)
 *    - 계층 구조로 통신 시간 단축
 *
 * 3. 통신 시간 모델:
 *    - time = latency + dataSize / bandwidth
 *    - 병렬 통신으로 총 시간 결정
 *    - 네트워크 특성 반영
 *
 * 4. 성능 최적화:
 *    - 계층 활용: 1000배 개선
 *    - 대역폭 활용도 증가
 *    - 지연 시간 숨김
 *
 * 5. 확장성:
 *    - 노드 증가에 따른 성능 저하 최소화
 *    - 로그 선형의 통신 복잡도
 *    - 대규모 클러스터 지원
 *
 * 심화 과제:
 * 1. 동적 토폴로지 적응
 * 2. Fault-tolerant AllReduce
 * 3. 압축 기반 AllReduce (데이터 크기 감소)
 * 4. 비동기 AllReduce
 * 5. 다중 AllReduce 통합
 */
