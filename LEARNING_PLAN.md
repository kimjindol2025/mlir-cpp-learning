# 📖 C++ 학습 계획 (4개월, 16주)

## ⏱️ **시간표**

### **Week 1-4: C++ 기초** (4주)
```
목표: C++ 문법, 메모리 관리, STL 기초 마스터

Week 1: 기본 문법
├─ 변수, 자료형, 포인터
├─ 함수, 클래스, 객체
└─ 연산자 오버로딩

Week 2: 메모리 & RAII
├─ 스택 vs 힙
├─ new/delete (위험)
├─ unique_ptr/shared_ptr (안전)
└─ RAII 패턴

Week 3: STL 컨테이너
├─ vector, deque
├─ map, unordered_map
├─ set, priority_queue
└─ 반복자(iterator)

Week 4: 고급 기능
├─ 템플릿(template)
├─ 람다 함수
├─ 예외 처리
└─ 첫 프로젝트: 정렬 알고리즘 구현
```

**실습**:
- 벡터 동적 배열 구현
- 이진 탐색 트리 구현
- 간단한 해시 맵 구현

**도서**: "C++ Primer" Chapter 1-13

---

### **Week 5-8: MLIR C++ API** (4주)
```
목표: MLIR 구조 이해, C++ API 실습

Week 5: MLIR 기초
├─ MLIR 아키텍처
├─ Operation, Region, Block
├─ Type, Attribute
└─ Context, Builder

Week 6: Dialect 개념
├─ Dialect 정의
├─ Operation 정의
├─ Attribute/Type 정의
└─ 간단한 Dialect 작성

Week 7: Pass 프레임워크
├─ OperationPass, ModulePass
├─ Pattern Rewriter
├─ Conversion
└─ 첫 최적화 Pass 작성

Week 8: 통합 프로젝트
├─ 파이썬 코드 분석
├─ MLIR Dialect 설계
├─ 기초 변환 구현
└─ 단위 테스트
```

**실습**:
- Hello World Dialect
- 간단한 산술 연산 Dialect
- 첫 최적화 Pass

**자료**: MLIR 공식 튜토리얼, LLVM 예제

---

### **Week 9-16: 핵심 알고리즘 포팅** (8주)
```
목표: Python 버전을 C++로 포팅

Week 9-10: Task Graph (2주)
├─ DAG 구조 C++로 표현
├─ 위상 정렬 구현
├─ 병렬화 감지
└─ 단위 테스트

Week 11-12: Topology Aware Scheduler (2주)
├─ 네트워크 토폴로지 감지
├─ 스케줄링 알고리즘
├─ 통신 오버랩
└─ 성능 테스트

Week 13-14: Weighted Sharding (2주)
├─ 이기종 GPU 모델링
├─ 작업 할당 알고리즘
├─ 로드 밸런싱
└─ 벤치마킹

Week 15-16: Hierarchical AllReduce (2주)
├─ 계층적 통신 트리
├─ 비용 최적화
├─ 실제 데이터 테스트
└─ 1000배 속도 검증
```

**실습**:
- 각 모듈별 단위 테스트
- 성능 벤치마킹
- Python 버전과 비교

---

### **Week 17+: 성능 최적화** (4주+)
```
목표: 프로덕션 수준 최적화

성능 목표:
  Python: 1075ms/iter
  C++:    10ms/iter (100배)

최적화 항목:
├─ SIMD (SSE, AVX)
├─ 병렬화 (OpenMP, TBB)
├─ 메모리 풀
├─ 캐시 친화성
└─ 벤치마킹
```

---

## 📚 **권장 도서/자료**

### 필독서
1. **"C++ Primer"** (Lippman et al.)
   - 종합적인 기초
   - 초보자 친화적
   - 900페이지

2. **"Effective C++"** (Scott Meyers)
   - 고급 기법
   - 실전 팁
   - 50가지 경험담

3. **"MLIR Tutorials"** (LLVM 공식)
   - 공식 학습 자료
   - 예제 코드
   - 최신 정보

### 온라인 강의
- Coursera: "Modern C++ Programming"
- edX: "C++ for Programmers"
- YouTube: CppCon 강연

### 실습
- LeetCode (C++ 태그)
- Project Euler (C++)
- HackerRank (C++ 알고리즘)

---

## 🎯 **마일스톤**

- [ ] **Week 4 끝**: C++ 기초 완성 (Hello, World!)
- [ ] **Week 8 끝**: MLIR API 기초 (첫 Dialect)
- [ ] **Week 12 끝**: Task Graph 포팅 완료
- [ ] **Week 16 끝**: 모든 알고리즘 포팅
- [ ] **Week 20 끝**: 성능 최적화 완료
- [ ] **최종**: 프로덕션 배포 준비

---

## 💻 **환경 설정**

### 필수 설치
```bash
# Ubuntu/Debian
sudo apt install clang++ cmake git

# LLVM/MLIR 빌드
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
mkdir build
cd build
cmake -G Ninja ../llvm -DLLVM_ENABLE_PROJECTS=mlir
ninja

# 테스트
clang++ --version
```

### IDE 권장
- Visual Studio Code
  - C/C++ Extension
  - CMake Tools
  - LLDB Debugger

### 빌드 시스템
- CMake (권장)
- Ninja (빌드 속도)
- Make (대체)

---

## 📊 **성과 지표**

| 주차 | 목표 | 코드 줄수 | 테스트 |
|------|------|---------|--------|
| Week 4 | C++ 기초 | 500 | 10 |
| Week 8 | MLIR API | 1,500 | 15 |
| Week 12 | Task Graph | 3,500 | 20 |
| Week 16 | 전체 포팅 | 8,000 | 50 |
| Week 20 | 최적화 | 8,500 | 60 |

---

## 🚀 **최종 목표**

```
Python Version:         C++ Version:
├─ 22,850줄            ├─ 8,000줄 (3배 간결)
├─ 실행: 1075ms/iter   ├─ 실행: 10ms/iter (100배)
├─ 메모리: 많음         ├─ 메모리: 적음 (10배)
└─ 배포: 어려움         └─ 배포: 쉬움 (프로덕션)
```

**최종 성과**: 
✅ 프로덕션 수준의 고성능 컴파일러
✅ 박사급 C++ 코드
✅ MLIR 전문성 증명
✅ 논문 발표 가능 수준

---

**시작일**: 2026-03-01
**목표일**: 2026-06-30
**진행상황**: 🚀 Starting
