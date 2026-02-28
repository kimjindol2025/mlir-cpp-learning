/**
 * Exercise 3: 통신 최적화 및 오버래핑 (Communication Optimization & Overlapping)
 *
 * 학습 목표:
 * - 프로세서 간 통신 모델링
 * - 데이터 의존성 (Data Dependency) 추적
 * - 통신 오버래핑 (Communication Overlapping)
 * - 프로세서 타입별 통신 비용 모델
 *
 * 난이도: ⭐⭐⭐ 고급 (University Level 심화)
 *
 * 배경: 분산 시스템에서 프로세서 간 통신이 병목
 * 목표: 계산과 통신을 동시에 수행하여 성능 개선
 */

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

/**
 * 프로세서 타입 정의
 */
enum class ProcessorType {
    CPU,     // 일반 CPU
    GPU,     // GPU (빠르지만 통신 느림)
    TPU,     // Tensor Processing Unit
    NPU      // Neural Processing Unit
};

/**
 * 프로세서 정보
 */
struct Processor {
    int id;
    ProcessorType type;
    string name;
    float computePower;  // FLOPS
    float bandwidth;     // GB/s (프로세서간 통신)
    float latency;       // ns (프로세서간 통신)

    Processor(int id, ProcessorType type, const string& name,
              float power, float bw, float lat)
        : id(id), type(type), name(name),
          computePower(power), bandwidth(bw), latency(lat) {}
};

/**
 * 통신 엣지 (프로세서 간 데이터 전송)
 */
struct DataEdge {
    int fromTask;
    int toTask;
    long dataSize;  // bytes

    DataEdge(int from, int to, long size)
        : fromTask(from), toTask(to), dataSize(size) {}
};

/**
 * Task with 통신 정보
 */
struct TaskWithComm {
    int id;
    string name;
    long computeTime;  // 계산 시간 (ms)

    TaskWithComm(int id, const string& name, long compute)
        : id(id), name(name), computeTime(compute) {}
};

/**
 * 통신 비용 계산기
 */
class CommunicationModel {
private:
    map<pair<int, int>, Processor*> processors;
    vector<Processor> procList;

public:
    CommunicationModel() {}

    void addProcessor(const Processor& proc) {
        procList.push_back(proc);
    }

    /**
     * 두 프로세서 간 통신 시간 계산
     * time = latency + dataSize / bandwidth
     */
    long calculateCommTime(int fromProc, int toProc, long dataSize) {
        auto& p = procList[toProc];

        // 기본 지연시간 (ns -> ms)
        long latency_ms = 1;  // 단순화: 1ms

        // 대역폭 기반 전송 시간
        // dataSize (bytes) / bandwidth (GB/s) = time (ms)
        // 1 GB/s = 10^9 bytes/s = 10^6 bytes/ms
        long transferTime = (dataSize + 1000000 - 1) / 1000000;

        return latency_ms + transferTime;
    }

    /**
     * 프로세서 타입별 통신 비용 배수
     */
    float getCommunicationFactor(ProcessorType fromType,
                                  ProcessorType toType) {
        // GPU↔GPU는 빠름 (NVLink 등)
        if (fromType == ProcessorType::GPU &&
            toType == ProcessorType::GPU) {
            return 0.5f;  // 50% 빠름
        }

        // TPU↔TPU는 매우 빠름 (같은 칩)
        if (fromType == ProcessorType::TPU &&
            toType == ProcessorType::TPU) {
            return 0.2f;
        }

        // CPU↔GPU는 느림
        if ((fromType == ProcessorType::CPU &&
             toType == ProcessorType::GPU) ||
            (fromType == ProcessorType::GPU &&
             toType == ProcessorType::CPU)) {
            return 2.0f;  // 2배 느림
        }

        return 1.0f;  // 기본값
    }

    Processor* getProcessor(int id) {
        if (id >= 0 && id < (int)procList.size()) {
            return &procList[id];
        }
        return nullptr;
    }

    int getProcessorCount() const {
        return procList.size();
    }
};

/**
 * 통신 오버래핑 분석기
 */
class CommunicationOverlapAnalyzer {
private:
    CommunicationModel& commModel;
    map<int, TaskWithComm> tasks;
    vector<DataEdge> edges;

public:
    CommunicationOverlapAnalyzer(CommunicationModel& model)
        : commModel(model) {}

    void addTask(int id, const string& name, long computeTime) {
        tasks[id] = TaskWithComm(id, name, computeTime);
    }

    void addEdge(int from, int to, long dataSize) {
        edges.push_back(DataEdge(from, to, dataSize));
    }

    /**
     * 오버래핑 가능성 분석
     *
     * Case 1: 순차 실행 (Sequential)
     *   Time = Compute + Communication
     *
     * Case 2: 오버래핑 (Overlapped)
     *   Time = max(Compute, Communication)
     */
    struct OverlapAnalysis {
        long computeTime;
        long commTime;
        long sequentialTime;
        long overlapTime;
        double overlapRatio;

        OverlapAnalysis() : computeTime(0), commTime(0),
                           sequentialTime(0), overlapTime(0),
                           overlapRatio(0.0) {}
    };

    OverlapAnalysis analyzeOverlap(int fromTask, int toTask,
                                     int fromProc, int toProc) {
        OverlapAnalysis result;

        auto fromIt = tasks.find(fromTask);
        auto toIt = tasks.find(toTask);

        if (fromIt == tasks.end() || toIt == tasks.end()) {
            return result;
        }

        // 통신 엣지 찾기
        long commDataSize = 0;
        for (const auto& edge : edges) {
            if (edge.fromTask == fromTask && edge.toTask == toTask) {
                commDataSize = edge.dataSize;
                break;
            }
        }

        result.computeTime = fromIt->second.computeTime;
        result.commTime = commModel.calculateCommTime(
            fromProc, toProc, commDataSize);
        result.sequentialTime = result.computeTime + result.commTime;
        result.overlapTime = max(result.computeTime, result.commTime);
        result.overlapRatio = 1.0 - (double)result.overlapTime /
                             result.sequentialTime;

        return result;
    }

    /**
     * 전체 그래프의 오버래핑 가능성
     */
    void analyzeFullGraph(const map<int, int>& taskToProc) {
        cout << "\n=== 통신 오버래핑 분석 ===" << endl;
        cout << endl;

        long totalCompute = 0;
        long totalComm = 0;
        long overlapTime = 0;

        for (const auto& edge : edges) {
            int fromTask = edge.fromTask;
            int toTask = edge.toTask;
            int fromProc = taskToProc.at(fromTask);
            int toProc = taskToProc.at(toTask);

            auto analysis = analyzeOverlap(fromTask, toTask,
                                          fromProc, toProc);

            totalCompute += analysis.computeTime;
            totalComm += analysis.commTime;
            overlapTime += analysis.overlapTime;

            cout << "Task " << fromTask << " (P" << fromProc << ") → "
                 << "Task " << toTask << " (P" << toProc << ")" << endl;
            cout << "  Compute: " << analysis.computeTime << "ms "
                 << "Comm: " << analysis.commTime << "ms" << endl;

            if (fromProc == toProc) {
                cout << "  → 같은 프로세서 (통신 불필요)" << endl;
            } else if (analysis.overlapRatio > 0) {
                cout << "  → 오버래핑 가능: "
                     << fixed << setprecision(1)
                     << analysis.overlapRatio * 100 << "%" << endl;
            } else {
                cout << "  → 오버래핑 불가능 (통신이 더 김)" << endl;
            }
            cout << endl;
        }

        cout << "총합:" << endl;
        cout << "  순차 실행: " << (totalCompute + totalComm) << "ms" << endl;
        cout << "  최적 오버래핑: " << overlapTime << "ms" << endl;
        cout << "  개선 효과: "
             << fixed << setprecision(1)
             << (1.0 - (double)overlapTime / (totalCompute + totalComm)) * 100
             << "%" << endl;
    }

    /**
     * 프로세서 배치 최적화 (간단한 휴리스틱)
     */
    map<int, int> optimizePlacement() {
        cout << "\n=== 프로세서 배치 최적화 ===" << endl;

        map<int, int> placement;
        int procCount = commModel.getProcessorCount();

        // 단순 휴리스틱: Task ID를 프로세서에 라운드 로빈 배치
        for (const auto& [taskId, task] : tasks) {
            placement[taskId] = taskId % procCount;
        }

        // 높은 통신량의 Task들을 같은 프로세서에 배치
        long maxEdgeSize = 0;
        int maxFromTask = -1, maxToTask = -1;

        for (const auto& edge : edges) {
            if (edge.dataSize > maxEdgeSize) {
                maxEdgeSize = edge.dataSize;
                maxFromTask = edge.fromTask;
                maxToTask = edge.toTask;
            }
        }

        if (maxFromTask != -1) {
            // 최대 통신 엣지의 Task들을 같은 프로세서에
            placement[maxToTask] = placement[maxFromTask];
            cout << "최대 통신량 Task " << maxFromTask << " ↔ "
                 << maxToTask << "를 P" << placement[maxFromTask]
                 << "에 배치" << endl;
        }

        cout << "\n배치 결과:" << endl;
        for (const auto& [taskId, procId] : placement) {
            cout << "  Task " << taskId << " → Processor " << procId << endl;
        }

        return placement;
    }

    /**
     * 통신 비용 매트릭스 출력
     */
    void printCommunicationMatrix() {
        cout << "\n=== 프로세서 간 통신 비용 ===" << endl;
        cout << "(1000 bytes 전송 기준)" << endl;
        cout << endl;

        int procCount = commModel.getProcessorCount();
        long dataSize = 1000;  // 1KB

        // 헤더
        cout << "    ";
        for (int j = 0; j < procCount; j++) {
            cout << setw(8) << ("P" + to_string(j));
        }
        cout << endl;

        // 행렬
        for (int i = 0; i < procCount; i++) {
            cout << setw(3) << ("P" + to_string(i)) << " ";

            for (int j = 0; j < procCount; j++) {
                if (i == j) {
                    cout << setw(8) << "-";
                } else {
                    long time = commModel.calculateCommTime(i, j, dataSize);
                    cout << setw(8) << (to_string(time) + "ms");
                }
            }
            cout << endl;
        }
    }
};

// ============================================================================
// 테스트
// ============================================================================

/**
 * 테스트 1: 기본 통신 비용 계산
 */
void testCommunicationCost() {
    cout << "\n=== 테스트 1: 통신 비용 계산 ===" << endl;

    CommunicationModel model;

    model.addProcessor(Processor(0, ProcessorType::CPU,
                                 "CPU", 100, 20, 100));  // 100 Gflops, 20GB/s
    model.addProcessor(Processor(1, ProcessorType::GPU,
                                 "GPU0", 1000, 100, 100));  // 1Tflops, 100GB/s
    model.addProcessor(Processor(2, ProcessorType::TPU,
                                 "TPU0", 500, 50, 100));

    model.printCommunicationMatrix();
}

/**
 * 테스트 2: 오버래핑 분석
 */
void testOverlapAnalysis() {
    cout << "\n=== 테스트 2: 통신 오버래핑 분석 ===" << endl;

    CommunicationModel model;
    model.addProcessor(Processor(0, ProcessorType::CPU, "CPU", 100, 20, 100));
    model.addProcessor(Processor(1, ProcessorType::GPU, "GPU", 1000, 100, 100));

    CommunicationOverlapAnalyzer analyzer(model);

    analyzer.addTask(1, "MatMul", 50);    // 50ms 계산
    analyzer.addTask(2, "Activation", 20);  // 20ms 계산

    analyzer.addEdge(1, 2, 10000000);     // 10MB 데이터

    // 다른 프로세서 배치
    map<int, int> placement;
    placement[1] = 0;  // Task 1 on CPU
    placement[2] = 1;  // Task 2 on GPU

    analyzer.analyzeFullGraph(placement);
}

/**
 * 테스트 3: 프로세서 배치 최적화
 */
void testPlacementOptimization() {
    cout << "\n=== 테스트 3: 프로세서 배치 최적화 ===" << endl;

    CommunicationModel model;
    model.addProcessor(Processor(0, ProcessorType::CPU, "CPU", 100, 20, 100));
    model.addProcessor(Processor(1, ProcessorType::GPU, "GPU0", 1000, 100, 100));
    model.addProcessor(Processor(2, ProcessorType::GPU, "GPU1", 1000, 100, 100));

    CommunicationOverlapAnalyzer analyzer(model);

    for (int i = 1; i <= 6; i++) {
        analyzer.addTask(i, "Task" + to_string(i), 20 + i * 5);
    }

    // 통신 패턴
    analyzer.addEdge(1, 2, 5000000);   // 5MB
    analyzer.addEdge(2, 3, 1000000);   // 1MB
    analyzer.addEdge(3, 4, 10000000);  // 10MB (큼!)
    analyzer.addEdge(4, 5, 2000000);   // 2MB
    analyzer.addEdge(5, 6, 3000000);   // 3MB

    auto placement = analyzer.optimizePlacement();
    analyzer.analyzeFullGraph(placement);
}

/**
 * 테스트 4: 복잡한 의존성 그래프
 */
void testComplexGraph() {
    cout << "\n=== 테스트 4: 복잡한 그래프 (8 Task, 4 Processor) ===" << endl;

    CommunicationModel model;
    model.addProcessor(Processor(0, ProcessorType::CPU, "CPU", 100, 20, 100));
    model.addProcessor(Processor(1, ProcessorType::GPU, "GPU0", 1000, 100, 100));
    model.addProcessor(Processor(2, ProcessorType::GPU, "GPU1", 1000, 100, 100));
    model.addProcessor(Processor(3, ProcessorType::TPU, "TPU0", 500, 50, 100));

    CommunicationOverlapAnalyzer analyzer(model);

    // 8개 Task
    for (int i = 1; i <= 8; i++) {
        analyzer.addTask(i, "T" + to_string(i), 30 + i * 2);
    }

    // 복잡한 의존성
    analyzer.addEdge(1, 2, 2000000);
    analyzer.addEdge(1, 3, 3000000);
    analyzer.addEdge(2, 4, 1500000);
    analyzer.addEdge(3, 4, 2500000);
    analyzer.addEdge(4, 5, 4000000);
    analyzer.addEdge(4, 6, 5000000);
    analyzer.addEdge(5, 7, 1000000);
    analyzer.addEdge(6, 7, 2000000);
    analyzer.addEdge(7, 8, 3000000);

    auto placement = analyzer.optimizePlacement();
    analyzer.analyzeFullGraph(placement);

    cout << "\n✓ 복잡한 그래프 분석 완료" << endl;
}

int main() {
    cout << "╔═══════════════════════════════════════╗" << endl;
    cout << "║  Exercise 3: 통신 최적화              ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl;

    try {
        testCommunicationCost();
        testOverlapAnalysis();
        testPlacementOptimization();
        testComplexGraph();

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
 * 1. 통신 비용 모델:
 *    - Latency + Bandwidth 모델
 *    - 프로세서 타입별 차이
 *    - 거리 기반 비용
 *
 * 2. 통신 오버래핑:
 *    - 계산과 통신을 동시에 수행
 *    - 성능 개선: 최대 50-70%
 *    - 메모리 대역폭 제약 고려
 *
 * 3. 작업 배치 (Task Placement):
 *    - 통신 비용을 고려한 배치
 *    - 고통신 Task는 같은 프로세서에
 *    - 계산 부하 균형 고려
 *
 * 4. 이질적 시스템:
 *    - CPU, GPU, TPU 등 혼합
 *    - 각 프로세서 특성 활용
 *    - 통신 경로 선택
 *
 * 5. 성능 예측:
 *    - Makespan = max(Compute, Comm)
 *    - Communication-hiding 기법
 *    - 확장성 분석
 *
 * 심화 과제:
 * 1. 메모리 대역폭 모델링
 * 2. 캐시 일관성 프로토콜 고려
 * 3. 네트워크 토폴로지 기반 라우팅
 * 4. 동적 작업 마이그레이션
 * 5. 에너지 효율성 고려
 */
