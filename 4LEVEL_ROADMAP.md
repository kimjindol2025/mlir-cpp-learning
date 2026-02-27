# 📚 4단계 C++ 학습 로드맵
## 초등 → 중고등 → 대학 → 대학원

**프로젝트**: MLIR C++ Learning Journey
**목표**: 4단계 마스터로 박사급 수준 달성
**기간**: 16주 (2026-03-01 ~ 2026-06-30)

---

## 🎯 **4단계 구조**

```
레벨 1: 초등학교 (Elementary) ⭐
└─ Week 1-4: C++ 기초
   ├─ 변수, 포인터, 함수
   ├─ 클래스, 객체
   └─ 첫 프로젝트: 정렬 알고리즘

레벨 2: 중고등학교 (Junior) ⭐⭐
└─ Week 5-8: MLIR C++ API
   ├─ MLIR 아키텍처
   ├─ Op, Dialect, Pass
   └─ 첫 Dialect 작성

레벨 3: 대학 (University) ⭐⭐⭐
└─ Week 9-12: 알고리즘 포팅
   ├─ Task Graph 구현
   ├─ Scheduler 최적화
   └─ 성능 벤치마킹

레벨 4: 대학원 (Graduate) ⭐⭐⭐⭐
└─ Week 13-16: 프로덕션 최적화
   ├─ 병렬화 (OpenMP, TBB)
   ├─ 메모리 최적화
   └─ 논문 수준 성능
```

---

## 📖 **레벨 1: 초등학교 (Elementary)**

### 목표
- C++ 기초 개념 완전히 이해
- 객체지향의 기본 원리 습득
- 첫 프로젝트 완성

### 내용 (Week 1-4)

#### Week 1: 포인터 & 메모리
```cpp
// 레벨 1: "주소가 뭔가요?"
int x = 10;
int* ptr = &x;  // &x는 x의 주소
cout << *ptr;   // *ptr은 주소가 가리키는 값

// 동적 메모리 (조금 위험)
int* arr = new int[10];
delete[] arr;
```

**학습 목표**:
- ✅ 포인터 개념 이해
- ✅ 스택 vs 힙 구분
- ✅ new/delete 사용

#### Week 2: 함수 & 클래스
```cpp
// 레벨 1: "클래스가 뭔가요?"
class Student {
public:
    string name;
    int score;

    void print() {
        cout << name << ": " << score << endl;
    }
};

Student s;
s.name = "Kim";
s.score = 95;
s.print();
```

**학습 목표**:
- ✅ 클래스 정의 방법
- ✅ 멤버 변수와 함수
- ✅ 생성자

#### Week 3-4: 첫 프로젝트
```cpp
// 레벨 1: "정렬 알고리즘"
class Sorter {
    int* arr;
    int size;
public:
    void bubble_sort() { /* ... */ }
    void quick_sort() { /* ... */ }
    bool is_sorted() { /* ... */ }
};
```

**프로젝트**: 배열 정렬 클래스
**산출물**: 500줄 코드, 5개 테스트

### 난이도: ⭐ (쉬움)
### 시간: 4주
### 사전 지식: 없음

---

## 📖 **레벨 2: 중고등학교 (Junior)**

### 목표
- MLIR 이해
- C++ 고급 기능 습득
- 첫 Dialect 작성

### 내용 (Week 5-8)

#### Week 5: MLIR 아키텍처
```cpp
// 레벨 2: "MLIR의 계층 구조"

// Operation을 만드는 것 = 프로그램 만들기
mlir::MLIRContext context;
mlir::OpBuilder builder(&context);

auto loc = mlir::UnknownLoc::get(&context);
auto i32 = builder.getI32Type();

// BinaryOp = x + y 같은 연산
auto op = builder.create<MyOp>(loc, i32, lhs, rhs);
```

**MLIR 개념**:
- Operation: 연산 (x + y)
- Dialect: 연산 모음 (수학 연산, GPU 연산 등)
- Pass: 최적화 (x+0 → x)

#### Week 6: Dialect 정의
```cpp
// 레벨 2: "나만의 연산 정의"

#define GET_OP_CLASSES
#include "MyDialect.cpp.inc"

// MyOp = MyDialect의 연산
// 예: %0 = my.add %a, %b : f32
```

#### Week 7: Pass 작성
```cpp
// 레벨 2: "최적화 만들기"
class MyOptimizationPass : public mlir::OperationPass<mlir::ModuleOp> {
public:
    void runOnOperation() override {
        // x + 0 를 x로 변환 (최적화)
    }
};
```

#### Week 8: 첫 Dialect 완성
**프로젝트**: Hello Dialect
```cpp
// 출력 예:
// %0 = hello.add %a, %b : f32
// %1 = hello.mul %0, %c : f32
```

### 난이도: ⭐⭐ (중간)
### 시간: 4주
### 사전 지식: 레벨 1 완료

---

## 📖 **레벨 3: 대학 (University)**

### 목표
- Python 코드를 C++로 포팅
- 성능 최적화
- 실제 문제 해결

### 내용 (Week 9-12)

#### Week 9-10: Task Graph 포팅
```cpp
// 레벨 3: "복잡한 구조 구현"

class TaskGraph {
private:
    std::vector<Task> tasks;
    std::vector<std::vector<int>> dependencies;

public:
    std::vector<int> topological_sort() {
        // DAG 위상 정렬 (Python 버전을 C++로)
    }

    std::vector<std::vector<int>> find_parallelizable_groups() {
        // 병렬화 가능한 그룹 감지
    }
};
```

**포팅 내용**:
- Python의 networkx → C++ 수동 구현
- 리스트/딕셔너리 → STL 컨테이너
- 성능 × 100배

#### Week 11-12: 스케줄러 최적화
```cpp
// 레벨 3: "성능 코드"

class TopoAwareScheduler {
public:
    struct ScheduleResult {
        double makespan;
        double communication_time;
        std::vector<Task> schedule;
    };

    ScheduleResult schedule(const TaskGraph& graph) {
        // 토폴로지 감지
        // 최적 스케줄 결정
        // 비용 계산
    }
};
```

**학습 내용**:
- STL 고급 사용 (priority_queue, set)
- 알고리즘 복잡성 (O(N²))
- 벤치마킹 & 프로파일링

### 프로젝트: Task Graph 완전 포팅
**산출물**: 3,500줄 코드, 20개 테스트, 성능 10배 향상

### 난이도: ⭐⭐⭐ (높음)
### 시간: 4주
### 사전 지식: 레벨 2 완료

---

## 📖 **레벨 4: 대학원 (Graduate)**

### 목표
- 프로덕션 수준 코드
- 병렬화 & 최적화
- 논문 수준 성능

### 내용 (Week 13-16)

#### Week 13-14: 병렬화
```cpp
// 레벨 4: "멀티코어 활용"

// OpenMP로 병렬화
#pragma omp parallel for
for (int i = 0; i < N; i++) {
    result[i] = compute(data[i]);
}

// TBB로 Task 기반 병렬화
tbb::parallel_for(0, N, [](int i) {
    process(i);
});
```

**고급 기법**:
- SIMD (SSE, AVX)
- Cache 친화성
- Lock-free 알고리즘

#### Week 15-16: 최종 최적화
```cpp
// 레벨 4: "극한 성능"

// Hierarchical AllReduce (논문 구현)
class HierarchicalAllReduce {
private:
    struct HierarchyLevel {
        std::vector<int> nodes;
        std::vector<int> parents;
    };

    std::vector<HierarchyLevel> hierarchy;

public:
    void optimize_for_topology(const NetworkTopology& topo) {
        // NVLink vs Ethernet 자동 감지
        // 계층 구조 최적화
        // 1000배 속도 달성
    }
};
```

**프로덕션 체크리스트**:
- [ ] 메모리 누수 제로 (valgrind)
- [ ] 100% 스레드 안전
- [ ] 에러 처리 완벽
- [ ] 성능 목표 달성 (10ms/iter)
- [ ] 논문 수준 벤치마킹

### 프로젝트: 완전 포팅 + 최적화
**산출물**: 8,000줄 코드, 60개 테스트, 100배 성능

### 성과:
```
Python:  22,850줄 → C++: 8,000줄 (3배 간결)
시간:    1075ms/iter → C++: 10ms/iter (100배 빠름)
메모리:  많음 → C++: 적음 (10배 효율)
배포:    불가 → C++: 프로덕션 가능
```

### 난이도: ⭐⭐⭐⭐ (매우 높음)
### 시간: 4주
### 사전 지식: 레벨 3 완료 + 성능 최적화 이해

---

## 🎯 **각 레벨별 체크리스트**

### 레벨 1 완료 조건
- [ ] 포인터 개념 완벽 이해
- [ ] 클래스 정의 및 사용 가능
- [ ] 정렬 알고리즘 구현
- [ ] 5개 단위 테스트 통과
- [ ] 500줄 코드 작성

### 레벨 2 완료 조건
- [ ] MLIR Operation 이해
- [ ] Dialect 정의 가능
- [ ] Pass 작성 가능
- [ ] 15개 테스트 통과
- [ ] 1,500줄 코드 작성

### 레벨 3 완료 조건
- [ ] Python → C++ 포팅 완료
- [ ] Task Graph 구현
- [ ] 스케줄러 동작
- [ ] 20개 테스트 통과
- [ ] 3,500줄 코드 작성
- [ ] 10배 성능 향상 증명

### 레벨 4 완료 조건
- [ ] 병렬화 완료
- [ ] AllReduce 1000배 성능
- [ ] 메모리 누수 0
- [ ] 60개 테스트 통과
- [ ] 8,000줄 코드 작성
- [ ] 100배 성능 달성
- [ ] 프로덕션 배포 준비

---

## 📊 **단계별 성장**

```
레벨 1: 초등학교
  코드:    500줄
  개념:    포인터, 클래스
  난이도:  ⭐

레벨 2: 중고등학교
  누적:    2,000줄
  개념:    MLIR, Dialect, Pass
  난이도:  ⭐⭐

레벨 3: 대학
  누적:    5,500줄
  개념:    포팅, 최적화, 벤치마킹
  난이도:  ⭐⭐⭐

레벨 4: 대학원
  누적:    8,000줄
  개념:    병렬화, 프로덕션, 논문
  난이도:  ⭐⭐⭐⭐

최종: 박사급
  성과:    22,850줄 (Python) → 8,000줄 (C++)
  성능:    1075ms → 10ms (100배)
  수준:    프로덕션 준비 완료
```

---

## 🏆 **각 레벨의 결과물**

### 레벨 1: "코드 기초를 배웠어요"
```cpp
// 정렬 알고리즘
void bubble_sort() { /* ... */ }
void quick_sort() { /* ... */ }
```
→ 초등학교 과학 프로젝트 수준

### 레벨 2: "MLIR 맛을 봤어요"
```mlir
%0 = my.add %a, %b : f32
%1 = my.mul %0, %c : f32
```
→ 고등학교 프로그래밍 경진대회 수준

### 레벨 3: "실제 문제 풀 수 있어요"
```cpp
// Task Graph → MLIR → 최적화
auto result = scheduler.optimize(graph);
assert(result.makespan < 10ms);
```
→ 대학 졸업 프로젝트 수준

### 레벨 4: "논문 쓸 수 있어요"
```cpp
// Hierarchical AllReduce
// 성능: 100배 향상
// 논문 제목: "Topology-Aware AllReduce..."
```
→ 대학원 박사 논문 수준

---

## 📋 **주간 체크인**

| 주차 | 레벨 | 목표 | 코드 | 테스트 | 상태 |
|------|------|------|------|--------|------|
| 1 | 1초 | 포인터 기초 | 100 | 2 | ⭕ |
| 2 | 1초 | 클래스 | 200 | 3 | ⭕ |
| 3 | 1초 | 첫 프로젝트 | 150 | 3 | ⭕ |
| 4 | 1초 | 정렬 완성 | 50 | 2 | ⭕ |
| 5 | 2주 | MLIR 기초 | 300 | 3 | 🔄 |
| 6 | 2주 | Dialect | 400 | 4 | 🔄 |
| 7 | 2주 | Pass | 350 | 4 | 🔄 |
| 8 | 2주 | 첫 Dialect | 150 | 4 | 🔄 |
| 9 | 3대 | Task Graph | 900 | 5 | 예정 |
| 10 | 3대 | Scheduler | 1200 | 5 | 예정 |
| 11 | 3대 | 포팅 | 800 | 5 | 예정 |
| 12 | 3대 | 벤치마킹 | 600 | 5 | 예정 |
| 13 | 4대 | 병렬화 | 400 | 10 | 예정 |
| 14 | 4대 | 최적화 | 300 | 10 | 예정 |
| 15 | 4대 | AllReduce | 300 | 10 | 예정 |
| 16 | 4대 | 논문 | 200 | 10 | 예정 |

---

## 🎓 **최종 성과 증명**

### Python vs C++

```
┌─────────────────────────────────────────┐
│        Python (박사 프로젝트)            │
├─────────────────────────────────────────┤
│ 코드:     22,850줄                       │
│ 실행:     1075ms/iter                    │
│ 메모리:   많음 (메모리 누수 위험)         │
│ 배포:     어려움 (라이브러리 의존성)      │
│ 학습도:   ⭐⭐⭐ (이미 완성)             │
└─────────────────────────────────────────┘
              ↓ (포팅)
┌─────────────────────────────────────────┐
│          C++ (4단계 학습 후)             │
├─────────────────────────────────────────┤
│ 코드:     8,000줄 (3배 간결!)           │
│ 실행:     10ms/iter (100배 빠름!)       │
│ 메모리:   적음 (완벽한 관리)             │
│ 배포:     쉬움 (단일 바이너리)           │
│ 학습도:   ⭐⭐⭐⭐ (박사급!)            │
└─────────────────────────────────────────┘
```

### 증명 가능한 지표

- **코드 라인**: 22,850 → 8,000 (3배 감소)
- **성능**: 1075ms → 10ms (100배)
- **메모리**: 프로파일링으로 입증
- **안정성**: 60개 테스트 통과
- **논문**: 성능 결과로 발표 가능

---

## 🚀 **시작하기**

**지금 바로**:
1. `/home/mlir-cpp-learning` 폴더로 이동
2. `week01-04/WEEK1_BASICS.md` 읽기
3. Day 1 코드 작성
4. Gogs에 커밋

**레벨별 진행**:
- 레벨 1 완료 후 레벨 2 진행
- 각 레벨에서 충분히 학습
- 서두르지 말기 (실력 구축)

**최종 목표**:
- 16주 후 프로덕션 수준 C++ 코드
- 100배 성능 향상 입증
- 박사급 코드 포트폴리오

---

**시작일**: 2026-03-01
**목표일**: 2026-06-30
**난이도**: 초등 → 중고등 → 대학 → 대학원
**최종 목표**: 박사급 수준 달성! 🎓

