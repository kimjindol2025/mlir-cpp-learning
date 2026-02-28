/**
 * Exercise 1: Weighted Sharding (이기종 GPU 로드 밸런싱)
 *
 * 학습 목표:
 * - 이기종 시스템 모델링 (다양한 성능의 GPU)
 * - 작업 분할 (Sharding) 알고리즘
 * - 가중치 기반 할당 (Weighted Assignment)
 * - 로드 밸런싱 최적화
 *
 * 난이도: ⭐⭐⭐⭐ 최고급 (Graduate Level)
 *
 * 배경: 모던 데이터센터는 여러 세대의 GPU 혼합 (RTX 4090, A100, H100 등)
 * 목표: 각 GPU의 성능에 맞게 작업을 분배하여 전체 시간 최소화
 */

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <numeric>

using namespace std;

/**
 * GPU 장치 정보
 */
struct GPUDevice {
    int id;
    string name;
    float computeCapability;  // 상대 성능 (1.0 = 기준)
    float memorySize;         // GB
    float bandwidth;          // GB/s
    long peakFlops;          // FLOPS

    GPUDevice(int id, const string& name, float compute,
              float memory, float bw, long flops)
        : id(id), name(name), computeCapability(compute),
          memorySize(memory), bandwidth(bw), peakFlops(flops) {}

    void print() const {
        cout << "GPU " << id << ": " << setw(12) << name
             << " Compute=" << fixed << setprecision(2) << computeCapability
             << "x Memory=" << memorySize << "GB"
             << " Peak=" << peakFlops / 1e12 << "TFlops" << endl;
    }
};

/**
 * 데이터 샤드 (작업의 일부)
 */
struct DataShard {
    int id;
    long dataSize;      // bytes
    long computeOps;    // 필요한 연산 수
    float priority;     // 우선도 (1.0 = 기본)

    DataShard(int id, long size, long ops, float prio = 1.0f)
        : id(id), dataSize(size), computeOps(ops), priority(prio) {}

    // 이 샤드를 특정 GPU에서 실행할 때의 시간 (ms)
    long getExecutionTime(const GPUDevice& gpu) const {
        if (gpu.computeCapability == 0) return LLONG_MAX;

        // 연산 시간 = 연산 수 / (성능 * 피크 플롭스)
        // FLOPS = 연산/초이므로
        long baseTime = computeOps / (gpu.peakFlops / 1000);  // ms
        long scaledTime = (long)(baseTime / gpu.computeCapability);

        return scaledTime;
    }
};

/**
 * Weighted Sharding 알고리즘
 *
 * 목표: 주어진 GPU들에 샤드를 최적으로 배치
 * 제약: 메모리 한계
 * 목적: 전체 실행 시간 최소화
 */
class WeightedShardingAllocator {
private:
    vector<GPUDevice> gpus;
    vector<DataShard> shards;

public:
    WeightedShardingAllocator() {}

    void addGPU(const GPUDevice& gpu) {
        gpus.push_back(gpu);
    }

    void addShard(const DataShard& shard) {
        shards.push_back(shard);
    }

    void printGPUs() const {
        cout << "\n=== GPU 구성 ===" << endl;
        for (const auto& gpu : gpus) {
            gpu.print();
        }
    }

    /**
     * 단순 Greedy 할당: 가장 빨리 끝날 GPU에 할당
     */
    map<int, vector<int>> allocateGreedy() {
        cout << "\n=== Greedy 할당 ===" << endl;

        map<int, vector<int>> allocation;  // gpu_id → [shard_ids]
        map<int, long> gpuTime;            // gpu_id → 총 실행 시간

        // 초기화
        for (const auto& gpu : gpus) {
            gpuTime[gpu.id] = 0;
        }

        // 각 샤드를 가장 빨리 끝날 GPU에 할당
        for (const auto& shard : shards) {
            int bestGPU = -1;
            long minEndTime = LLONG_MAX;

            for (const auto& gpu : gpus) {
                long endTime = gpuTime[gpu.id] +
                              shard.getExecutionTime(gpu);

                if (endTime < minEndTime) {
                    minEndTime = endTime;
                    bestGPU = gpu.id;
                }
            }

            allocation[bestGPU].push_back(shard.id);
            gpuTime[bestGPU] += shards[shard.id - 1].getExecutionTime(
                gpus[bestGPU]);

            cout << "Shard " << shard.id << " → GPU " << bestGPU << endl;
        }

        cout << "\n로드 분포:" << endl;
        for (const auto& gpu : gpus) {
            cout << "GPU " << gpu.id << ": " << gpuTime[gpu.id] << "ms" << endl;
        }

        return allocation;
    }

    /**
     * Weighted 할당: GPU의 성능을 고려하여 더 정확하게 배치
     */
    map<int, vector<int>> allocateWeighted() {
        cout << "\n=== Weighted 할당 ===" << endl;

        map<int, vector<int>> allocation;
        map<int, long> gpuTime;
        map<int, float> gpuLoad;

        for (const auto& gpu : gpus) {
            gpuTime[gpu.id] = 0;
            gpuLoad[gpu.id] = 0.0f;
        }

        // 총 연산량
        long totalOps = 0;
        for (const auto& shard : shards) {
            totalOps += shard.computeOps;
        }

        // 이상적인 분배 비율 (성능에 비례)
        float totalCapability = 0;
        for (const auto& gpu : gpus) {
            totalCapability += gpu.computeCapability;
        }

        cout << "이상적 분배 비율:" << endl;
        for (const auto& gpu : gpus) {
            float ratio = gpu.computeCapability / totalCapability;
            cout << "GPU " << gpu.id << ": " << fixed << setprecision(1)
                 << ratio * 100 << "%" << endl;
        }

        // 각 샤드를 가장 부하가 적은 GPU에 할당 (가중 고려)
        for (const auto& shard : shards) {
            int bestGPU = -1;
            float minLoad = INFINITY;

            for (const auto& gpu : gpus) {
                long execTime = shard.getExecutionTime(gpu);
                float futureLoad = gpuLoad[gpu.id] +
                                  (float)execTime / gpu.computeCapability;

                if (futureLoad < minLoad) {
                    minLoad = futureLoad;
                    bestGPU = gpu.id;
                }
            }

            allocation[bestGPU].push_back(shard.id);
            long execTime = shards[shard.id - 1].getExecutionTime(
                gpus[bestGPU]);
            gpuTime[bestGPU] += execTime;
            gpuLoad[bestGPU] = (float)gpuTime[bestGPU] /
                              gpus[bestGPU].computeCapability;

            cout << "Shard " << shard.id << " → GPU " << bestGPU << endl;
        }

        cout << "\n로드 분포:" << endl;
        for (const auto& gpu : gpus) {
            cout << "GPU " << gpu.id << ": " << setw(6) << gpuTime[gpu.id]
                 << "ms (가중=" << fixed << setprecision(2)
                 << gpuLoad[gpu.id] << ")" << endl;
        }

        return allocation;
    }

    /**
     * 로드 밸런싱 지수 계산 (0~1, 1이 최적)
     */
    double calculateLoadBalance(const map<int, vector<int>>& allocation) {
        map<int, long> gpuTime;

        for (const auto& gpu : gpus) {
            gpuTime[gpu.id] = 0;
        }

        for (const auto& [gpuId, shardIds] : allocation) {
            for (int shardId : shardIds) {
                const auto& shard = shards[shardId - 1];
                gpuTime[gpuId] += shard.getExecutionTime(gpus[gpuId]);
            }
        }

        long maxTime = 0;
        long sumTime = 0;
        for (const auto& [gpuId, time] : gpuTime) {
            maxTime = max(maxTime, time);
            sumTime += time;
        }

        if (maxTime == 0) return 1.0;
        return (double)(sumTime / gpus.size()) / maxTime;
    }

    /**
     * 전체 실행 시간 (Makespan) 계산
     */
    long calculateMakespan(const map<int, vector<int>>& allocation) {
        long maxTime = 0;

        for (const auto& [gpuId, shardIds] : allocation) {
            long gpuTime = 0;
            for (int shardId : shardIds) {
                gpuTime += shards[shardId - 1].getExecutionTime(
                    gpus[gpuId]);
            }
            maxTime = max(maxTime, gpuTime);
        }

        return maxTime;
    }

    /**
     * 할당 결과 시각화
     */
    void visualizeAllocation(
        const map<int, vector<int>>& allocation) {
        cout << "\n=== 할당 시각화 ===" << endl;

        for (const auto& gpu : gpus) {
            cout << "\nGPU " << gpu.id << " (" << gpu.name << "):" << endl;

            auto it = allocation.find(gpu.id);
            if (it == allocation.end() || it->second.empty()) {
                cout << "  [할당 없음]" << endl;
                continue;
            }

            long totalTime = 0;
            for (int shardId : it->second) {
                const auto& shard = shards[shardId - 1];
                long execTime = shard.getExecutionTime(gpu);
                totalTime += execTime;

                cout << "  Shard " << shardId << ": "
                     << execTime << "ms (" << shard.dataSize / 1e6 << "MB)"
                     << endl;
            }

            cout << "  ━━━━━━━━━━━━━━━━━" << endl;
            cout << "  합계: " << totalTime << "ms" << endl;
        }
    }

    /**
     * 효율성 지표
     */
    void printEfficiencyMetrics(
        const map<int, vector<int>>& allocation) {
        cout << "\n=== 효율성 지표 ===" << endl;

        long makespan = calculateMakespan(allocation);
        double lb = calculateLoadBalance(allocation);

        // 순차 실행 시간
        long sequentialTime = 0;
        for (const auto& shard : shards) {
            sequentialTime += shard.getExecutionTime(gpus[0]);
        }

        // 총 실행 시간
        long totalParallelTime = 0;
        for (const auto& gpu : gpus) {
            auto it = allocation.find(gpu.id);
            if (it != allocation.end()) {
                for (int shardId : it->second) {
                    totalParallelTime += shards[shardId - 1]
                        .getExecutionTime(gpu);
                }
            }
        }

        double speedup = (double)sequentialTime / makespan;
        double efficiency = speedup / gpus.size();

        cout << "Makespan: " << makespan << "ms" << endl;
        cout << "로드 밸런싱: " << fixed << setprecision(3) << lb << endl;
        cout << "Speedup: " << speedup << "x" << endl;
        cout << "효율성: " << efficiency * 100 << "%" << endl;
    }
};

// ============================================================================
// 테스트
// ============================================================================

/**
 * 테스트 1: 기본 이기종 할당
 */
void testBasicHeterogeneousAllocation() {
    cout << "\n=== 테스트 1: 기본 이기종 GPU 할당 ===" << endl;

    WeightedShardingAllocator alloc;

    // 서로 다른 성능의 GPU들
    alloc.addGPU(GPUDevice(0, "RTX 4090", 1.0f, 24, 900, 1460000000000LL));
    alloc.addGPU(GPUDevice(1, "A100", 0.9f, 40, 2000, 312000000000LL));
    alloc.addGPU(GPUDevice(2, "V100", 0.4f, 32, 900, 130000000000LL));

    alloc.printGPUs();

    // 데이터 샤드들
    for (int i = 1; i <= 6; i++) {
        alloc.addShard(DataShard(i, 1000000 * i, 100000000000LL * i));
    }

    auto allocation = alloc.allocateWeighted();
    alloc.visualizeAllocation(allocation);
    alloc.printEfficiencyMetrics(allocation);
}

/**
 * 테스트 2: Greedy vs Weighted 비교
 */
void testAllocationComparison() {
    cout << "\n=== 테스트 2: Greedy vs Weighted 비교 ===" << endl;

    // Greedy
    {
        cout << "\n📊 Greedy 알고리즘:" << endl;
        WeightedShardingAllocator alloc;

        alloc.addGPU(GPUDevice(0, "GPU0", 1.0f, 24, 900, 1000000000000LL));
        alloc.addGPU(GPUDevice(1, "GPU1", 0.5f, 24, 500, 500000000000LL));

        for (int i = 1; i <= 4; i++) {
            alloc.addShard(DataShard(i, 1000000, 100000000000LL));
        }

        auto greedyAlloc = alloc.allocateGreedy();
        alloc.printEfficiencyMetrics(greedyAlloc);
    }

    // Weighted
    {
        cout << "\n📊 Weighted 알고리즘:" << endl;
        WeightedShardingAllocator alloc;

        alloc.addGPU(GPUDevice(0, "GPU0", 1.0f, 24, 900, 1000000000000LL));
        alloc.addGPU(GPUDevice(1, "GPU1", 0.5f, 24, 500, 500000000000LL));

        for (int i = 1; i <= 4; i++) {
            alloc.addShard(DataShard(i, 1000000, 100000000000LL));
        }

        auto weightedAlloc = alloc.allocateWeighted();
        alloc.printEfficiencyMetrics(weightedAlloc);
    }
}

/**
 * 테스트 3: 대규모 GPU 클러스터
 */
void testLargeCluster() {
    cout << "\n=== 테스트 3: 대규모 클러스터 (8 GPU, 16 샤드) ===" << endl;

    WeightedShardingAllocator alloc;

    // 8개의 이기종 GPU
    alloc.addGPU(GPUDevice(0, "H100-1", 1.0f, 80, 2400, 1980000000000LL));
    alloc.addGPU(GPUDevice(1, "H100-2", 1.0f, 80, 2400, 1980000000000LL));
    alloc.addGPU(GPUDevice(2, "A100-1", 0.7f, 40, 2000, 312000000000LL));
    alloc.addGPU(GPUDevice(3, "A100-2", 0.7f, 40, 2000, 312000000000LL));
    alloc.addGPU(GPUDevice(4, "V100-1", 0.3f, 32, 900, 130000000000LL));
    alloc.addGPU(GPUDevice(5, "V100-2", 0.3f, 32, 900, 130000000000LL));
    alloc.addGPU(GPUDevice(6, "T4-1", 0.1f, 16, 300, 65000000000LL));
    alloc.addGPU(GPUDevice(7, "T4-2", 0.1f, 16, 300, 65000000000LL));

    alloc.printGPUs();

    // 16개 샤드
    for (int i = 1; i <= 16; i++) {
        long size = 500000000 + i * 100000000;      // 0.5-2.1 GB
        long ops = 50000000000LL + i * 10000000000LL;  // 50-210B ops
        alloc.addShard(DataShard(i, size, ops));
    }

    auto allocation = alloc.allocateWeighted();
    alloc.visualizeAllocation(allocation);
    alloc.printEfficiencyMetrics(allocation);

    cout << "\n✓ 대규모 클러스터 분석 완료" << endl;
}

/**
 * 테스트 4: 우선도 기반 할당
 */
void testPriorityAllocation() {
    cout << "\n=== 테스트 4: 우선도 기반 할당 ===" << endl;

    WeightedShardingAllocator alloc;

    alloc.addGPU(GPUDevice(0, "GPU0", 1.0f, 24, 900, 1000000000000LL));
    alloc.addGPU(GPUDevice(1, "GPU1", 0.7f, 24, 700, 700000000000LL));

    // 다양한 우선도의 샤드
    alloc.addShard(DataShard(1, 1000000, 100000000000LL, 1.0f));  // 일반
    alloc.addShard(DataShard(2, 1000000, 100000000000LL, 2.0f));  // 높음
    alloc.addShard(DataShard(3, 1000000, 100000000000LL, 0.5f));  // 낮음
    alloc.addShard(DataShard(4, 1000000, 100000000000LL, 1.5f));  // 중간

    auto allocation = alloc.allocateWeighted();
    alloc.visualizeAllocation(allocation);
}

int main() {
    cout << "╔═══════════════════════════════════════╗" << endl;
    cout << "║  Exercise 1: Weighted Sharding        ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl;

    try {
        testBasicHeterogeneousAllocation();
        testAllocationComparison();
        testLargeCluster();
        testPriorityAllocation();

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
 * 1. 이기종 시스템 모델:
 *    - 다양한 성능의 처리기
 *    - 메모리 크기 차이
 *    - 통신 대역폭 다름
 *
 * 2. Sharding 알고리즘:
 *    - 작업을 여러 부분으로 분할
 *    - 각 부분을 최적 장치에 할당
 *    - 메모리/연산 균형
 *
 * 3. Weighted 할당:
 *    - 장치 성능에 비례하여 작업 분배
 *    - 이상적 부하 계산
 *    - 동적 로드 밸런싱
 *
 * 4. 성능 지표:
 *    - Makespan (최단 완료 시간)
 *    - 로드 밸런싱 지수
 *    - Speedup & 효율성
 *
 * 5. 최적화 전략:
 *    - Greedy: 간단하지만 최적 아님
 *    - Weighted: 더 정확한 성능 모델
 *    - 휴리스틱 + 미세 조정
 *
 * 심화 과제:
 * 1. 동적 재스케줄링 (작업 마이그레이션)
 * 2. 메모리 제약 고려
 * 3. 통신 시간 포함
 * 4. 에너지 효율성 최적화
 * 5. 온라인 스케줄링 (작업 도착 시간 불확실)
 */
