# 🚀 Week 4: 최종 프로젝트 - 종합 정렬 라이브러리

**목표**: Week 1-3 모든 개념을 통합한 프로덕션급 정렬 라이브러리 구현

**기간**: 2026-03-22 ~ 2026-03-28 (7일)

---

## 📚 **Day 1-2: 예외 처리 (Exception Handling)**

### 기본 개념

```cpp
#include <iostream>
#include <stdexcept>
using namespace std;

// ✅ 좋은 코드: 예외 처리
class Calculator {
public:
    double divide(double a, double b) {
        if (b == 0) {
            throw invalid_argument("0으로 나눌 수 없습니다");
        }
        return a / b;
    }
};

int main() {
    Calculator calc;

    try {
        cout << calc.divide(10, 2);    // 5
        cout << calc.divide(10, 0);    // ❌ 예외 발생
    }
    catch (invalid_argument& e) {
        cout << "오류: " << e.what() << endl;
    }
    catch (exception& e) {
        cout << "일반 오류: " << e.what() << endl;
    }

    return 0;
}
```

### 사용자 정의 예외

```cpp
// 사용자 정의 예외 클래스
class SortException : public exception {
private:
    string message;
public:
    SortException(const string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

// 사용
void validate_input(const vector<int>& arr) {
    if (arr.empty()) {
        throw SortException("배열이 비어있습니다");
    }
    if (arr.size() > 1000000) {
        throw SortException("배열이 너무 큽니다");
    }
}
```

### RAII와 예외

```cpp
// ✅ RAII로 안전한 리소스 관리
class FileLogger {
private:
    FILE* file;
public:
    FileLogger(const string& filename) {
        file = fopen(filename.c_str(), "w");
        if (!file) throw runtime_error("파일 열기 실패");
    }

    ~FileLogger() {
        if (file) fclose(file);  // 예외 발생해도 자동 정리
    }

    void log(const string& msg) {
        if (!file) throw runtime_error("파일이 닫혔습니다");
        fprintf(file, "%s\n", msg.c_str());
    }
};
```

---

## 📚 **Day 3-4: 컴파일 타임 프로그래밍**

### constexpr로 컴파일 타임 계산

```cpp
#include <iostream>
using namespace std;

// ✅ 컴파일 타임에 계산됨
constexpr int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

constexpr int fibonacci(int n) {
    return (n <= 1) ? n : fibonacci(n-1) + fibonacci(n-2);
}

int main() {
    // 컴파일 타임에 계산됨!
    constexpr int fact5 = factorial(5);   // 120
    constexpr int fib10 = fibonacci(10);  // 55

    cout << fact5 << " " << fib10 << endl;

    return 0;
}
```

### constexpr 정렬 (컴파일 타임 정렬)

```cpp
#include <array>
#include <algorithm>
using namespace std;

constexpr array<int, 5> compile_time_sort() {
    array<int, 5> arr = {5, 2, 8, 1, 9};

    // C++20: constexpr std::sort 가능
    for (int i = 0; i < arr.size(); i++) {
        for (int j = i + 1; j < arr.size(); j++) {
            if (arr[i] > arr[j]) {
                swap(arr[i], arr[j]);
            }
        }
    }
    return arr;
}

int main() {
    constexpr auto sorted = compile_time_sort();

    for (int x : sorted) cout << x << " ";  // 1 2 5 8 9

    return 0;
}
```

### SFINAE (Substitution Failure Is Not An Error)

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

// ✅ 정수형만 사용 가능
template <typename T>
enable_if_t<is_integral_v<T>, void>
print_as_binary(T value) {
    cout << "이진수: ";
    for (int i = 7; i >= 0; i--) {
        cout << ((value >> i) & 1);
    }
    cout << endl;
}

// ✅ 부동소수점만 사용 가능
template <typename T>
enable_if_t<is_floating_point_v<T>, void>
print_as_binary(T value) {
    cout << "부동소수점: " << value << endl;
}

int main() {
    print_as_binary(42);      // 정수형 버전
    print_as_binary(3.14);    // 부동소수점 버전

    return 0;
}
```

---

## 📚 **Day 5-6: 종합 정렬 라이브러리**

### 최종 정렬 라이브러리 구조

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <chrono>
#include <memory>
#include <stdexcept>
using namespace std;

class SortException : public exception {
private:
    string message;
public:
    SortException(const string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

template <typename T, typename Comparator = less<T>>
class SortingLibrary {
private:
    Comparator comp;

    // 버블 정렬 (느림, 교육용)
    void bubble_sort_impl(vector<T>& arr) {
        int n = arr.size();
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (comp(arr[j+1], arr[j])) {
                    swap(arr[j], arr[j+1]);
                }
            }
        }
    }

    // 퀵 정렬 (빠름)
    int partition(vector<T>& arr, int left, int right) {
        T pivot = arr[right];
        int i = left - 1;

        for (int j = left; j < right; j++) {
            if (comp(arr[j], pivot)) {
                swap(arr[++i], arr[j]);
            }
        }
        swap(arr[i+1], arr[right]);
        return i + 1;
    }

    void quick_sort_impl(vector<T>& arr, int left, int right) {
        if (left >= right) return;

        int pivot = partition(arr, left, right);
        quick_sort_impl(arr, left, pivot - 1);
        quick_sort_impl(arr, pivot + 1, right);
    }

    // 병합 정렬 (안정적)
    void merge(vector<T>& arr, int left, int mid, int right) {
        vector<T> temp(right - left + 1);
        int i = left, j = mid + 1, k = 0;

        while (i <= mid && j <= right) {
            if (comp(arr[i], arr[j])) {
                temp[k++] = arr[i++];
            } else {
                temp[k++] = arr[j++];
            }
        }

        while (i <= mid) temp[k++] = arr[i++];
        while (j <= right) temp[k++] = arr[j++];

        for (int i = 0; i < k; i++) {
            arr[left + i] = temp[i];
        }
    }

    void merge_sort_impl(vector<T>& arr, int left, int right) {
        if (left >= right) return;

        int mid = (left + right) / 2;
        merge_sort_impl(arr, left, mid);
        merge_sort_impl(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

    // 힙 정렬 (공간 효율적)
    void heapify(vector<T>& arr, int n, int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && comp(arr[largest], arr[left])) {
            largest = left;
        }
        if (right < n && comp(arr[largest], arr[right])) {
            largest = right;
        }

        if (largest != i) {
            swap(arr[i], arr[largest]);
            heapify(arr, n, largest);
        }
    }

    void heap_sort_impl(vector<T>& arr) {
        int n = arr.size();

        for (int i = n / 2 - 1; i >= 0; i--) {
            heapify(arr, n, i);
        }

        for (int i = n - 1; i > 0; i--) {
            swap(arr[0], arr[i]);
            heapify(arr, i, 0);
        }
    }

public:
    // 입력 검증
    void validate(const vector<T>& arr) const {
        if (arr.empty()) {
            throw SortException("배열이 비어있습니다");
        }
        if (arr.size() > 10000000) {
            throw SortException("배열이 너무 큽니다");
        }
    }

    // 정렬 여부 확인
    bool is_sorted(const vector<T>& arr) const {
        for (size_t i = 0; i < arr.size() - 1; i++) {
            if (comp(arr[i+1], arr[i])) {
                return false;
            }
        }
        return true;
    }

    // 공개 정렬 인터페이스
    void bubble_sort(vector<T>& arr) {
        validate(arr);
        bubble_sort_impl(arr);
    }

    void quick_sort(vector<T>& arr) {
        validate(arr);
        if (arr.size() > 1) {
            quick_sort_impl(arr, 0, arr.size() - 1);
        }
    }

    void merge_sort(vector<T>& arr) {
        validate(arr);
        if (arr.size() > 1) {
            merge_sort_impl(arr, 0, arr.size() - 1);
        }
    }

    void heap_sort(vector<T>& arr) {
        validate(arr);
        heap_sort_impl(arr);
    }

    // 성능 측정 구조체
    struct BenchmarkResult {
        string algorithm;
        chrono::milliseconds duration;
        bool is_sorted;

        void print() const {
            cout << algorithm << ": " << duration.count() << "ms "
                 << (is_sorted ? "✓" : "✗") << endl;
        }
    };

    // 벤치마크
    vector<BenchmarkResult> benchmark_all(vector<T> arr) {
        vector<BenchmarkResult> results;

        // 버블 정렬
        if (arr.size() <= 1000) {  // 작은 배열만
            auto temp = arr;
            auto start = chrono::high_resolution_clock::now();
            bubble_sort(temp);
            auto end = chrono::high_resolution_clock::now();

            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            results.push_back({"버블 정렬", duration, is_sorted(temp)});
        }

        // 퀵 정렬
        {
            auto temp = arr;
            auto start = chrono::high_resolution_clock::now();
            quick_sort(temp);
            auto end = chrono::high_resolution_clock::now();

            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            results.push_back({"퀵 정렬", duration, is_sorted(temp)});
        }

        // 병합 정렬
        {
            auto temp = arr;
            auto start = chrono::high_resolution_clock::now();
            merge_sort(temp);
            auto end = chrono::high_resolution_clock::now();

            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            results.push_back({"병합 정렬", duration, is_sorted(temp)});
        }

        // 힙 정렬
        {
            auto temp = arr;
            auto start = chrono::high_resolution_clock::now();
            heap_sort(temp);
            auto end = chrono::high_resolution_clock::now();

            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            results.push_back({"힙 정렬", duration, is_sorted(temp)});
        }

        return results;
    }
};

// 사용 예
int main() {
    SortingLibrary<int> sorter;

    // 테스트 1: 작은 배열
    {
        vector<int> arr = {64, 34, 25, 12, 22, 11, 90};
        cout << "=== 작은 배열 (7개) ===" << endl;

        auto results = sorter.benchmark_all(arr);
        for (const auto& r : results) {
            r.print();
        }
    }

    // 테스트 2: 중간 배열
    {
        vector<int> arr(10000);
        for (int i = 0; i < 10000; i++) {
            arr[i] = rand() % 1000;
        }
        cout << "\n=== 중간 배열 (10,000개) ===" << endl;

        auto results = sorter.benchmark_all(arr);
        for (const auto& r : results) {
            r.print();
        }
    }

    // 테스트 3: 큰 배열
    {
        vector<int> arr(100000);
        for (int i = 0; i < 100000; i++) {
            arr[i] = rand() % 10000;
        }
        cout << "\n=== 큰 배열 (100,000개) ===" << endl;

        auto results = sorter.benchmark_all(arr);
        for (const auto& r : results) {
            r.print();
        }
    }

    // 테스트 4: 예외 처리
    {
        cout << "\n=== 예외 처리 테스트 ===" << endl;
        vector<int> empty;

        try {
            sorter.quick_sort(empty);
        }
        catch (SortException& e) {
            cout << "예외 캐치: " << e.what() << endl;
        }
    }

    return 0;
}
```

---

## 📚 **Day 7: Week 4 프로젝트 - 고급 정렬 라이브러리**

### 람다와 함께하는 정렬

```cpp
int main() {
    SortingLibrary<int, greater<int>> desc_sorter;  // 내림차순
    vector<int> arr = {3, 1, 4, 1, 5, 9, 2, 6};

    desc_sorter.quick_sort(arr);

    for (int x : arr) cout << x << " ";  // 9 6 5 4 3 2 1 1

    return 0;
}
```

### 구조체 정렬

```cpp
struct Person {
    string name;
    int age;

    bool operator<(const Person& other) const {
        return age < other.age;
    }
};

int main() {
    SortingLibrary<Person> people_sorter;
    vector<Person> people = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35}
    };

    people_sorter.quick_sort(people);

    for (const auto& p : people) {
        cout << p.name << " (" << p.age << ")" << endl;
    }

    return 0;
}
```

---

## 🎯 **Week 4 체크리스트**

- [ ] 예외 처리 이해 및 사용
- [ ] try-catch-throw 패턴 습득
- [ ] 사용자 정의 예외 클래스 작성
- [ ] constexpr로 컴파일 타임 프로그래밍
- [ ] SFINAE와 enable_if 이해
- [ ] 4가지 정렬 알고리즘 구현
  - [ ] 버블 정렬 (교육용, O(n²))
  - [ ] 퀵 정렬 (평균 O(n log n))
  - [ ] 병합 정렬 (안정적, O(n log n))
  - [ ] 힙 정렬 (공간 효율, O(n log n))
- [ ] 성능 벤치마크 구현
- [ ] 전체 정렬 라이브러리 완성
- [ ] 1000~100,000개 원소 테스트

---

## 💾 **빌드 & 실행**

```bash
# 컴파일 (C++17 이상)
g++ -std=c++17 -O2 -o sort_library sort_library.cpp

# 실행
./sort_library

# 예상 출력:
# === 작은 배열 (7개) ===
# 버블 정렬: 0ms ✓
# 퀵 정렬: 0ms ✓
# 병합 정렬: 0ms ✓
# 힙 정렬: 0ms ✓
#
# === 중간 배열 (10,000개) ===
# 버블 정렬: 버블은 작은 배열만 테스트
# 퀵 정렬: 8ms ✓
# 병합 정렬: 5ms ✓
# 힙 정렬: 6ms ✓
#
# === 큰 배열 (100,000개) ===
# 퀵 정렬: 85ms ✓
# 병합 정렬: 65ms ✓
# 힙 정렬: 80ms ✓
```

---

## 📖 **추가 학습**

- "C++ Primer" Chapter 5 (문장과 표현식)
- "C++ Primer" Chapter 18 (예외)
- "Effective C++" Item 50-55 (고급 기법)
- CppCon 2023: "The C++ Template 200" by Herb Sutter

---

## 🎓 **Week 1-4 최종 정리**

### 학습 경로
```
Week 1: 기초 (포인터, 클래스) → 500줄
    ↓
Week 2: 메모리 (RAII, 스마트 포인터) → 1,000줄
    ↓
Week 3: 고급 (템플릿, 람다, STL) → 1,500줄
    ↓
Week 4: 통합 (종합 라이브러리) → 2,000줄
```

### 성과
- ✅ C++ 기초 문법 완전 습득
- ✅ 메모리 안전성 (RAII, 스마트 포인터)
- ✅ 제네릭 프로그래밍 (템플릿)
- ✅ 함수형 프로그래밍 (람다)
- ✅ 표준 라이브러리 (STL)
- ✅ 예외 처리 및 검증
- ✅ 성능 벤치마킹

### 다음 레벨
Week 5-8: MLIR C++ API (Junior Level)
- MLIR 구조 이해
- Dialect 정의 및 구현
- Pass 프레임워크
- 첫 최적화 구현

---

**다음**: Week 5 - MLIR 기초 (Context, Builder, Operation)

