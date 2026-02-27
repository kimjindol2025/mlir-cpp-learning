# 🎓 Week 4: 최종 프로젝트 - 완전 정렬 라이브러리 (온라인 혼자 학습용)

**목표**: Week 1-3의 모든 개념을 통합한 프로덕션급 정렬 라이브러리 완성

**난이도**: ⭐⭐⭐⭐ 도전
**학습 시간**: 약 20시간
**선수과목**: Week 1-3 완료

> **이 주차가 중요한 이유**: 실제 회사에서 사용 가능한 **수준 높은 C++ 코드**를 만들어봅니다!

---

## 📖 학습 안내

- ⚠️ **예외 처리**: 안전한 에러 핸들링
- ⚡ **컴파일 타임 최적화**: constexpr와 SFINAE
- 🔨 **완전한 라이브러리**: 4가지 정렬 알고리즘
- 🧪 **테스트와 검증**: 단위 테스트와 벤치마크
- ✅ **프로덕션 수준**: 실제 프로젝트처럼 구성

---

## 📚 **Day 1: 예외 처리 (Exception Handling)**

### 1.1 예외란?

**예외 = 프로그램이 처리할 수 없는 비정상 상황**

```
정상 실행:
입력 → 처리 → 출력 ✓

예외 발생:
입력 → 처리 → ⚠️ 오류! → 예외 처리
```

### 1.2 try-catch-throw

```cpp
#include <iostream>
#include <stdexcept>
using namespace std;

double divide(double a, double b) {
    if (b == 0) {
        // throw: 예외 던지기
        throw invalid_argument("0으로 나눌 수 없습니다");
    }
    return a / b;
}

int main() {
    // try: 위험한 코드
    try {
        cout << divide(10, 2) << endl;   // 5 (정상)
        cout << divide(10, 0) << endl;   // 예외 발생!
        cout << "이 줄은 실행 안 됨" << endl;
    }
    // catch: 예외 처리
    catch (invalid_argument& e) {
        cout << "오류: " << e.what() << endl;
    }
    catch (exception& e) {
        cout << "일반 오류: " << e.what() << endl;
    }

    cout << "프로그램 정상 종료" << endl;

    return 0;
}
```

**실행 결과:**
```
5
오류: 0으로 나눌 수 없습니다
프로그램 정상 종료
```

### 1.3 사용자 정의 예외

```cpp
#include <iostream>
#include <stdexcept>
using namespace std;

// 커스텀 예외 클래스
class SortException : public exception {
private:
    string message;

public:
    SortException(const string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

void validateArray(int* arr, int size) {
    if (arr == nullptr) {
        throw SortException("배열이 null입니다");
    }
    if (size <= 0) {
        throw SortException("배열 크기가 0 이하입니다");
    }
    if (size > 1000000) {
        throw SortException("배열이 너무 큽니다");
    }
}

int main() {
    try {
        validateArray(nullptr, 10);
    }
    catch (SortException& e) {
        cout << "정렬 오류: " << e.what() << endl;
    }

    return 0;
}
```

**실행 결과:**
```
정렬 오류: 배열이 null입니다
```

---

### ⚠️ **흔한 실수 #10: 예외 타입 불일치**

```cpp
❌ 잘못된 코드:
try {
    throw invalid_argument("오류");
}
catch (runtime_error& e) {  // ← 타입 다름!
    cout << e.what() << endl;  // 실행 안 됨!
}
```

```cpp
✅ 올바른 코드:
try {
    throw invalid_argument("오류");
}
catch (invalid_argument& e) {  // ← 일치!
    cout << e.what() << endl;  // 실행됨
}
catch (exception& e) {  // ← 모든 예외 처리
    cout << "기타 오류" << endl;
}
```

---

### ✅ **자기점검 Quiz 4.1**

다음 코드의 출력은?

```cpp
try {
    throw runtime_error("Error!");
}
catch (invalid_argument& e) {
    cout << "Caught 1" << endl;
}
catch (exception& e) {
    cout << "Caught 2" << endl;
}
```

<details>
<summary>정답 보기</summary>

**출력**: `Caught 2`

설명:
- `runtime_error`는 `invalid_argument`와 다른 타입
- 첫 번째 catch는 스킵됨
- `runtime_error`는 `exception`의 자식이므로 두 번째 catch 실행
</details>

---

## 📚 **Day 2: 컴파일 타임 프로그래밍**

### 2.1 constexpr로 컴파일 타임 계산

```cpp
#include <iostream>
using namespace std;

// 컴파일 타임에 계산됨!
constexpr int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

constexpr int fibonacci(int n) {
    return (n <= 1) ? n : fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
    // 컴파일 시 계산됨 (실행 시 계산 아님)
    constexpr int fact5 = factorial(5);    // 120
    constexpr int fib10 = fibonacci(10);   // 55

    cout << "5! = " << fact5 << endl;
    cout << "fib(10) = " << fib10 << endl;

    return 0;
}
```

**왜 유용한가?**
```
일반 함수:
- 런타임에 계산 (프로그램 실행 중)
- 느림

constexpr 함수:
- 컴파일 타임에 계산 (프로그램 빌드 중)
- 매우 빠름! (상수처럼 동작)
```

### 2.2 constexpr 배열 정렬

```cpp
#include <iostream>
#include <array>
using namespace std;

constexpr array<int, 5> sortAtCompileTime() {
    array<int, 5> arr = {5, 2, 8, 1, 9};

    // 버블 정렬 (컴파일 타임)
    for (int i = 0; i < arr.size(); i++) {
        for (int j = 0; j < arr.size() - 1; j++) {
            if (arr[j] > arr[j+1]) {
                swap(arr[j], arr[j+1]);
            }
        }
    }
    return arr;
}

int main() {
    // 컴파일 시 정렬됨!
    constexpr auto sorted = sortAtCompileTime();

    cout << "컴파일 타임 정렬: ";
    for (int x : sorted) {
        cout << x << " ";
    }
    cout << endl;

    return 0;
}
```

**실행 결과:**
```
컴파일 타임 정렬: 1 2 5 8 9
```

> **놀라운 사실**: 프로그램 실행 시간에 정렬이 이미 완료됨!

---

### ⚠️ **흔한 실수 #11: 동적 할당과 constexpr**

```cpp
❌ 컴파일 오류:
constexpr int* getArray() {
    return new int[10];  // ← 동적 할당은 불가!
}
```

```cpp
✅ 올바른 코드:
constexpr array<int, 10> getArray() {
    return {1, 2, 3, ...};  // ← 고정 크기만 가능
}
```

---

## 📚 **Day 3: SFINAE (Substitution Failure Is Not An Error)**

### 3.1 타입 기반 조건부 컴파일

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

// 정수형만 사용 가능
template <typename T>
enable_if_t<is_integral_v<T>, void>
printBinary(T value) {
    cout << "이진수: ";
    for (int i = 7; i >= 0; i--) {
        cout << ((value >> i) & 1);
    }
    cout << endl;
}

// 부동소수점만 사용 가능
template <typename T>
enable_if_t<is_floating_point_v<T>, void>
printBinary(T value) {
    cout << "부동소수점: " << value << endl;
}

int main() {
    printBinary(42);       // 정수형 버전
    printBinary(3.14);     // 부동소수점 버전
    // printBinary("text");  // ← 컴파일 오류! 둘 다 불가능

    return 0;
}
```

**실행 결과:**
```
이진수: 00101010
부동소수점: 3.14
```

---

## 📚 **Day 4-7: 최종 프로젝트 - ComprehensiveSorter**

### 4.1 프로젝트 구조

```
SortingLibrary/
├─ sorting.h          # 메인 헤더
├─ sorting_impl.hpp   # 템플릿 구현
├─ main.cpp           # 테스트
└─ CMakeLists.txt     # 빌드 설정
```

### 4.2 sorting.h

```cpp
#ifndef SORTING_H
#define SORTING_H

#include <vector>
#include <functional>
#include <memory>
#include <chrono>
#include <iostream>
#include <stdexcept>

namespace sorting {

// 커스텀 예외
class SortException : public std::exception {
private:
    std::string message;

public:
    explicit SortException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

// 컴파일 타임 최대값
constexpr int MAX_ARRAY_SIZE = 10000000;

// 제네릭 정렬 클래스
template <typename T>
class Sorter {
private:
    std::vector<T> data;
    std::string lastAlgorithm;
    long long lastDurationMs;

public:
    // 생성자
    explicit Sorter(const std::vector<T>& initial_data = {})
        : data(initial_data), lastDurationMs(0) {
        validate();
    }

    // 이동 생성자
    Sorter(std::vector<T>&& initial_data) noexcept
        : data(std::move(initial_data)), lastDurationMs(0) {
        try { validate(); } catch (...) {}
    }

    // 복사 방지
    Sorter(const Sorter&) = delete;
    Sorter& operator=(const Sorter&) = delete;

    // 정렬 알고리즘들
    void quickSort(std::function<bool(T, T)> cmp = std::less<T>()) {
        measureTime("QuickSort", [this, cmp]() {
            quickSortHelper(0, (int)data.size() - 1, cmp);
        });
    }

    void mergeSort(std::function<bool(T, T)> cmp = std::less<T>()) {
        measureTime("MergeSort", [this, cmp]() {
            mergeSortHelper(0, (int)data.size() - 1, cmp);
        });
    }

    void heapSort(std::function<bool(T, T)> cmp = std::less<T>()) {
        measureTime("HeapSort", [this, cmp]() {
            heapSortHelper(cmp);
        });
    }

    void bubbleSort(std::function<bool(T, T)> cmp = std::less<T>()) {
        measureTime("BubbleSort", [this, cmp]() {
            bubbleSortHelper(cmp);
        });
    }

    // 정렬 검증
    bool isSorted(std::function<bool(T, T)> cmp = std::less<T>()) const {
        for (size_t i = 0; i < data.size() - 1; i++) {
            if (cmp(data[i+1], data[i])) {
                return false;
            }
        }
        return true;
    }

    // 데이터 출력
    void print(int count = 10) const {
        count = std::min(count, (int)data.size());
        for (int i = 0; i < count; i++) {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;
    }

    // 정보 출력
    void printInfo() const {
        std::cout << "크기: " << data.size() << " | ";
        std::cout << "마지막: " << lastAlgorithm << " (" << lastDurationMs << "ms)" << std::endl;
    }

    // 게터
    const std::vector<T>& getData() const { return data; }
    long long getLastDuration() const { return lastDurationMs; }

private:
    // QuickSort
    void quickSortHelper(int left, int right,
                        std::function<bool(T, T)> cmp) {
        if (left >= right) return;

        T pivot = data[right];
        int i = left - 1;

        for (int j = left; j < right; j++) {
            if (cmp(data[j], pivot)) {
                std::swap(data[++i], data[j]);
            }
        }
        std::swap(data[i+1], data[right]);

        quickSortHelper(left, i, cmp);
        quickSortHelper(i + 2, right, cmp);
    }

    // MergeSort
    void merge(int left, int mid, int right,
               std::function<bool(T, T)> cmp) {
        std::vector<T> temp;
        int i = left, j = mid + 1;

        while (i <= mid && j <= right) {
            if (cmp(data[i], data[j])) {
                temp.push_back(data[i++]);
            } else {
                temp.push_back(data[j++]);
            }
        }

        while (i <= mid) temp.push_back(data[i++]);
        while (j <= right) temp.push_back(data[j++]);

        for (size_t i = 0; i < temp.size(); i++) {
            data[left + i] = temp[i];
        }
    }

    void mergeSortHelper(int left, int right,
                        std::function<bool(T, T)> cmp) {
        if (left >= right) return;

        int mid = (left + right) / 2;
        mergeSortHelper(left, mid, cmp);
        mergeSortHelper(mid + 1, right, cmp);
        merge(left, mid, right, cmp);
    }

    // HeapSort
    void heapify(int n, int i, std::function<bool(T, T)> cmp) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && cmp(data[largest], data[left])) {
            largest = left;
        }
        if (right < n && cmp(data[largest], data[right])) {
            largest = right;
        }

        if (largest != i) {
            std::swap(data[i], data[largest]);
            heapify(n, largest, cmp);
        }
    }

    void heapSortHelper(std::function<bool(T, T)> cmp) {
        int n = data.size();

        for (int i = n / 2 - 1; i >= 0; i--) {
            heapify(n, i, cmp);
        }

        for (int i = n - 1; i > 0; i--) {
            std::swap(data[0], data[i]);
            heapify(i, 0, cmp);
        }
    }

    // BubbleSort
    void bubbleSortHelper(std::function<bool(T, T)> cmp) {
        int n = data.size();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (cmp(data[j+1], data[j])) {
                    std::swap(data[j], data[j+1]);
                }
            }
        }
    }

    // 시간 측정
    void measureTime(const std::string& algo,
                    std::function<void()> func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();

        lastAlgorithm = algo;
        lastDurationMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start).count();
    }

    // 데이터 검증
    void validate() {
        if (data.size() > MAX_ARRAY_SIZE) {
            throw SortException("배열이 최대 크기를 초과했습니다");
        }
    }
};

} // namespace sorting

#endif
```

### 4.3 main.cpp (완전한 테스트)

```cpp
#include "sorting.h"
#include <random>
#include <string>
using namespace std;
using namespace sorting;

// 랜덤 데이터 생성
vector<int> generateRandomInts(int count) {
    vector<int> result;
    mt19937 gen(42);  // 시드 고정 (재현성)
    uniform_int_distribution<> dis(0, 10000);

    for (int i = 0; i < count; i++) {
        result.push_back(dis(gen));
    }
    return result;
}

// 테스트 실행
template <typename T>
void runTest(const string& name, const vector<T>& data) {
    cout << "\n=== " << name << " ===" << endl;

    try {
        Sorter<T> sorter(data);
        cout << "크기: " << sorter.getData().size() << endl;

        sorter.quickSort();
        cout << "QuickSort: " << sorter.getLastDuration() << "ms "
             << (sorter.isSorted() ? "✓" : "✗") << endl;

        Sorter<T> sorter2(data);
        sorter2.mergeSort();
        cout << "MergeSort: " << sorter2.getLastDuration() << "ms "
             << (sorter2.isSorted() ? "✓" : "✗") << endl;

        Sorter<T> sorter3(data);
        sorter3.heapSort();
        cout << "HeapSort: " << sorter3.getLastDuration() << "ms "
             << (sorter3.isSorted() ? "✓" : "✗") << endl;

        cout << "앞의 10개: ";
        Sorter<T> final(data);
        final.quickSort();
        final.print(10);

    } catch (const SortException& e) {
        cout << "오류: " << e.what() << endl;
    }
}

int main() {
    cout << "╔════════════════════════════════════════════╗" << endl;
    cout << "║  Week 4: 완전 정렬 라이브러리               ║" << endl;
    cout << "║  모든 개념의 통합 프로젝트                  ║" << endl;
    cout << "╚════════════════════════════════════════════╝" << endl;

    // 1️⃣ 정수 정렬 (작은 배열)
    {
        auto data = generateRandomInts(100);
        runTest("정수 정렬 (100개)", data);
    }

    // 2️⃣ 정수 정렬 (큰 배열)
    {
        auto data = generateRandomInts(100000);
        runTest("정수 정렬 (100,000개)", data);
    }

    // 3️⃣ 실수 정렬
    {
        vector<double> data;
        mt19937 gen(42);
        uniform_real_distribution<> dis(0.0, 100.0);

        for (int i = 0; i < 10000; i++) {
            data.push_back(dis(gen));
        }

        runTest("실수 정렬 (10,000개)", data);
    }

    // 4️⃣ 문자열 정렬
    {
        vector<string> data = {
            "zebra", "apple", "banana", "cherry", "date",
            "elephant", "fig", "grape", "house", "iris",
            "jacket", "kite", "lemon", "mango", "noodle"
        };

        runTest("문자열 정렬 (15개)", data);
    }

    // 5️⃣ 내림차순 정렬
    {
        cout << "\n=== 커스텀 비교: 내림차순 ===" << endl;
        auto data = generateRandomInts(1000);

        try {
            Sorter<int> sorter(data);
            sorter.quickSort(greater<int>());
            cout << "정렬됨: " << (sorter.isSorted(greater<int>()) ? "✓" : "✗") << endl;
            cout << "앞의 10개: ";
            sorter.print(10);
        } catch (const SortException& e) {
            cout << "오류: " << e.what() << endl;
        }
    }

    // 6️⃣ 예외 처리 테스트
    {
        cout << "\n=== 예외 처리 ===" << endl;
        try {
            vector<int> empty;
            Sorter<int> sorter(empty);
            cout << "빈 배열도 정상 처리됨" << endl;
        } catch (const SortException& e) {
            cout << "오류: " << e.what() << endl;
        }
    }

    cout << "\n╔════════════════════════════════════════════╗" << endl;
    cout << "║  🎉 모든 테스트 완료!                       ║" << endl;
    cout << "║  Week 1-4 컴플릿!                          ║" << endl;
    cout << "╚════════════════════════════════════════════╝" << endl;

    return 0;
}
```

### 4.4 빌드 및 실행

```bash
# 간단한 빌드
g++ -std=c++17 -O2 -o sorting main.cpp

# 실행
./sorting
```

**예상 출력:**
```
╔════════════════════════════════════════════╗
║  Week 4: 완전 정렬 라이브러리               ║
║  모든 개념의 통합 프로젝트                  ║
╚════════════════════════════════════════════╝

=== 정수 정렬 (100개) ===
크기: 100
QuickSort: 0ms ✓
MergeSort: 0ms ✓
HeapSort: 0ms ✓
앞의 10개: 0 10 20 30 40 50 60 70 80 90

=== 정수 정렬 (100,000개) ===
크기: 100000
QuickSort: 20ms ✓
MergeSort: 18ms ✓
HeapSort: 22ms ✓
...

🎉 모든 테스트 완료!
Week 1-4 컴플릿!
```

---

## ✅ **Week 4 최종 자기점검**

다음을 모두 할 수 있으면 Week 4 완료입니다!

- [ ] try-catch-throw로 예외를 처리할 수 있다
- [ ] 커스텀 예외 클래스를 작성할 수 있다
- [ ] constexpr로 컴파일 타임 계산을 한다
- [ ] SFINAE와 enable_if를 이해한다
- [ ] 4가지 정렬 알고리즘을 모두 구현했다
- [ ] 제네릭 클래스로 모든 타입을 지원한다
- [ ] 람다 비교 함수로 정렬 순서를 제어한다
- [ ] 완전한 정렬 라이브러리를 성공적으로 컴파일 및 실행했다
- [ ] 모든 알고리즘이 올바르게 정렬함을 검증했다

---

## 🏆 **Elementary Level 완성!**

### Week 1-4 성과 정리

| 주차 | 주제 | 프로젝트 | 줄 수 |
|------|------|---------|------|
| Week 1 | 기초 & 포인터 | Sorter | 375줄 |
| Week 2 | 메모리 & RAII | SafeSorter | 425줄 |
| Week 3 | 템플릿 & STL | GenericSorter | 380줄 |
| Week 4 | 통합 프로젝트 | ComprehensiveSorter | 460줄 |
| **합계** | **C++ 완전 기초** | **프로덕션급 라이브러리** | **1,640줄** |

### 배운 기술

✅ **기초**: 포인터, 클래스, 함수
✅ **메모리**: 동적 할당, RAII, 스마트 포인터, move 의미론
✅ **고급**: 템플릿, 람다, STL, 알고리즘
✅ **안정성**: 예외 처리, 검증, 벤치마킹

### 다음 레벨

🚀 **Junior Level (Week 5-8): MLIR C++ API**

---

## 📚 **추가 학습 (선택사항)**

### 더 깊이 있게

- **메모리 풀**: 빈번한 new/delete 최적화
- **프로파일링**: valgrind, perf로 성능 분석
- **문자열 최적화**: SSO (Small String Optimization)
- **병렬화**: OpenMP로 다중 쓰레드 정렬
- **벤치마킹 라이브러리**: Google Benchmark 사용

---

**축하합니다!** 🎉

**C++ Elementary Level을 완전히 마스터했습니다!**

다음은 **Junior Level: MLIR C++ API**가 기다리고 있습니다!

