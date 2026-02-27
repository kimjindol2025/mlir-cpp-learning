# 🎓 University & Graduate Levels (Week 9-16)

**목표**: Python 22,850줄 → C++ 8,000줄, 1075ms → 10ms (100배 성능 향상)

---

## 📚 University Level (Week 9-12): Algorithm Porting

### Week 9-10: Task Graph & DAG

**목표**: 계산 그래프를 C++로 표현, 병렬화 감지

```cpp
// Task Graph 구조
class Task {
    int id;
    vector<int> dependencies;  // 선행 작업
    long duration;             // 실행 시간
};

class TaskGraph {
    vector<Task> tasks;

    // 위상 정렬 (topological sort)
    vector<int> topologicalSort();

    // 병렬화 감지 (parallelization detection)
    vector<vector<int>> findParallelLevels();

    // Critical Path
    long getCriticalPath();
};
```

**학습 내용**:
- DAG (방향성 비순환 그래프) 구현
- 위상 정렬 알고리즘
- 병렬화 가능한 부분 감지
- Critical Path 분석

**예상 코드**: 2,500-3,000줄

---

### Week 11-12: Topology-Aware Scheduler

**목표**: 네트워크 토폴로지를 고려한 작업 스케줄링

```cpp
// 네트워크 토폴로지
class NetworkTopology {
    // 통신 비용 행렬
    vector<vector<float>> latency;     // 지연 시간
    vector<vector<float>> bandwidth;   // 대역폭

    // 최적 경로 찾기
    vector<int> findOptimalPath(int src, int dst);

    // 통신 오버래핑
    void overlapCommunication(TaskGraph& graph);
};

class TopoAwareScheduler {
    void schedule(TaskGraph& graph, NetworkTopology& topo);
    float calculateScheduleLength();
};
```

**학습 내용**:
- 네트워크 토폴로지 모델링
- 통신 오버래핑
- 로드 밸런싱
- 성능 측정

**예상 코드**: 2,500-3,000줄

---

## 📚 Graduate Level (Week 13-16): High-Performance Optimization

### Week 13-14: Weighted Sharding

**목표**: 이기종 GPU에서 작업 균등 분배

```cpp
// 이기종 GPU 모델링
class GPUDevice {
    int compute_power;      // 연산 능력 (FLOPS)
    int memory_size;        // 메모리 크기
    float communication_bw;  // 통신 대역폭
};

class WeightedSharding {
    // 작업 분배
    void assignTasks(vector<Task>& tasks,
                    vector<GPUDevice>& devices);

    // 로드 밸런싱
    float getLoadBalance();

    // 최적화
    void optimizeAssignment();
};
```

**학습 내용**:
- 작업 가중치 모델링
- 그리디 알고리즘 vs 최적화
- ILP (Integer Linear Programming)
- 동적 부하 조정

**예상 코드**: 2,000-2,500줄

---

### Week 15-16: Hierarchical AllReduce

**목표**: 계층적 통신으로 1000배 성능 향상

```cpp
// AllReduce 알고리즘
class AllReduceAlgorithm {
    enum Type {
        Butterfly,          // 나비 구조
        Ring,               // 고리 구조
        Tree,               // 트리 구조
        HierarchicalTree    // 계층적 트리 ← 목표!
    };

    Type type;
    vector<vector<int>> tree;  // 계층 구조

    // 통신 시간 계산
    float calculateTime(long dataSize);

    // 최적화
    void optimizeTree(NetworkTopology& topo);
};
```

**학습 내용**:
- AllReduce의 여러 구현 방식
- 통신 시간 분석
- 계층적 알고리즘 설계
- 최종 벤치마킹

**예상 코드**: 2,000-2,500줄

---

## 📊 전체 통계

| 레벨 | 주차 | 주제 | 코드 | 학습 자료 |
|------|------|------|------|---------|
| Elementary | 1-4 | C++ 기초 | 1,640줄 | 16,000줄 |
| Junior | 5-8 | MLIR | 3,500줄 | 12,000줄 |
| University | 9-12 | 알고리즘 포팅 | 5,000줄 | 10,000줄 |
| **Graduate** | **13-16** | **최적화** | **4,500줄** | **8,000줄** |
| **합계** | **1-16** | **완전한 C++ 여정** | **14,640줄** | **46,000줄** |

---

## 🎯 최종 성과

```
입력: Python 코드 (22,850줄)
      ├─ Task Graph
      ├─ Scheduler
      ├─ Sharding
      └─ AllReduce

출력: C++ 코드 (8,000줄)
      ├─ 3배 더 간결
      ├─ 100배 더 빠름
      ├─ 메모리 10배 절약
      └─ 프로덕션 준비 완료
```

---

## 📚 추가 자료

### 필수 논문
1. "Ansor: Generating High-Performance Tensor Programs for Deep Learning"
2. "A Study of BFGS Updating with Limited Storage"
3. "Efficient Algorithms for Distributed Machine Learning"

### 추천 도서
1. "Collective Communication Algorithms" (NVIDIA)
2. "Distributed Systems Design" (van Steen)
3. "Performance Optimization of C++ Code"

---

## 🏆 최종 마일스톤

- [ ] **Week 12 끝**: Task Graph + Scheduler 완료 (6,000줄)
- [ ] **Week 14 끝**: Weighted Sharding 완료 (8,000줄)
- [ ] **Week 16 끝**: AllReduce 최적화 완료 (8,500줄)
- [ ] **완료**: 프로덕션 배포 준비

---

**축하합니다!** 이 경로를 완료하면 당신은 **C++ 고급 개발자**이면서 동시에 **컴파일러/분산 시스템 전문가**가 됩니다! 🚀

