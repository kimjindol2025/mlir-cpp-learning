# 🎓 Week 3: 고급 기능 - 템플릿, 람다, STL (온라인 혼자 학습용)

**목표**: 일반적인 코드 작성, 함수형 프로그래밍, 표준 라이브러리 완전 활용

**난이도**: ⭐⭐⭐ 상급
**학습 시간**: 약 25시간
**선수과목**: Week 1-2 완료

> **이 주차가 중요한 이유**: C++ 프로 개발자들이 매일 사용하는 **템플릿과 STL**을 배웁니다!

---

## 📖 학습 안내

- 💻 **템플릿의 마법**: 같은 코드로 int, double, string 모두 처리
- 🎯 **람다 함수**: 한 줄 함수로 코드를 간결하게
- 📦 **STL**: 벡터, 맵, 셋 등 유용한 자료구조
- 🔄 **알고리즘**: sort, find, transform 등 강력한 함수들
- ✅ **3개의 심화 프로젝트**

---

## 📚 **Day 1: 함수 템플릿 (Function Template)**

### 1.1 템플릿이란?

**템플릿 = 타입을 변수처럼 다루는 기술**

```
일반 함수:
int add(int a, int b) { return a + b; }
double add(double a, double b) { return a + b; }  // 코드 반복!

템플릿:
template <typename T>
T add(T a, T b) { return a + b; }  // 모든 타입 처리!
```

### 1.2 함수 템플릿의 구조

```cpp
// 템플릿 선언
template <typename T>  // ← T는 "타입 변수"
T add(T a, T b) {      // ← 함수 정의
    return a + b;
}

// 사용
cout << add(3, 5);           // int add(3, 5) 컴파일됨
cout << add(3.14, 2.86);     // double add(3.14, 2.86) 컴파일됨
cout << add("Hello", " C++"); // 오류! string +는 없음
```

> **컴파일러가 해주는 일**: 컴파일 시점에 필요한 버전들을 자동으로 생성!

### 1.3 제네릭 함수 구현

```cpp
#include <iostream>
#include <string>
using namespace std;

// 모든 타입을 비교하는 함수
template <typename T>
T max_value(T a, T b) {
    return (a > b) ? a : b;
}

int main() {
    // 1️⃣ 정수 비교
    cout << "max(3, 5) = " << max_value(3, 5) << endl;
    // 컴파일러가 생성: int max_value(int, int)

    // 2️⃣ 실수 비교
    cout << "max(3.14, 2.71) = " << max_value(3.14, 2.71) << endl;
    // 컴파일러가 생성: double max_value(double, double)

    // 3️⃣ 문자 비교
    cout << "max('a', 'z') = " << max_value('a', 'z') << endl;
    // 컴파일러가 생성: char max_value(char, char)

    // 4️⃣ 문자열 비교 (사전순)
    string s1 = "apple";
    string s2 = "banana";
    cout << "max(\"apple\", \"banana\") = " << max_value(s1, s2) << endl;
    // 컴파일러가 생성: string max_value(string, string)

    return 0;
}
```

**실행 결과:**
```
max(3, 5) = 5
max(3.14, 2.71) = 3.14
max('a', 'z') = z
max("apple", "banana") = banana
```

### 1.4 다중 템플릿 매개변수

```cpp
#include <iostream>
using namespace std;

// 두 개의 타입을 받을 수 있음
template <typename T, typename U>
auto add(T a, U b) {  // auto = 컴파일러가 반환 타입 추론
    return a + b;
}

int main() {
    // 1️⃣ int + int = int
    cout << add(3, 4) << endl;  // 7

    // 2️⃣ int + double = double
    cout << add(3, 4.5) << endl;  // 7.5

    // 3️⃣ double + int = double
    cout << add(3.5, 2) << endl;  // 5.5

    // 4️⃣ string + char = string
    cout << add("Hello", '!') << endl;  // "Hello!"

    return 0;
}
```

**실행 결과:**
```
7
7.5
5.5
Hello!
```

> **규칙**: `auto` 반환 타입은 C++14 이상에서만 가능

---

### ⚠️ **흔한 실수 #7: 템플릿 오류 메시지**

```cpp
❌ 컴파일 오류:
template <typename T>
T divide(T a, T b) {
    return a / b;
}

int main() {
    string s1 = "apple";
    string s2 = "banana";
    divide(s1, s2);  // ← 오류! string에 / 연산자 없음
}
```

**오류 메시지:**
```
error: no match for 'operator/' in 's1 / s2'
```

**해결책**: 템플릿은 필요한 연산자를 지원하는 타입에만 사용

### ⚠️ **흔한 실수 #8: 명시적 타입 지정**

```cpp
template <typename T>
void print(T value) {
    cout << value << endl;
}

int main() {
    // 컴파일러가 자동으로 int로 추론
    print(42);

    // 명시적으로 타입 지정도 가능
    print<double>(3.14);

    // 타입을 명시해야 하는 경우
    print<int>(42.9);  // int로 변환 후 출력
}
```

---

### ✅ **자기점검 Quiz 3.1**

다음 함수를 실행하면 무엇이 출력될까요?

```cpp
template <typename T>
T multiply(T a, T b) {
    return a * b;
}

cout << multiply(3, 4) << endl;
cout << multiply(2.5, 3.0) << endl;
```

<details>
<summary>정답 보기</summary>

**출력:**
```
12
7.5
```

설명:
- `multiply(3, 4)`: T = int, 결과 = 12
- `multiply(2.5, 3.0)`: T = double, 결과 = 7.5
</details>

---

## 📚 **Day 2-3: 클래스 템플릿**

### 2.1 제네릭 클래스 만들기

```cpp
#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;

// 스택 (LIFO: Last In First Out)
template <typename T>
class Stack {
private:
    vector<T> elements;

public:
    // 요소 추가
    void push(const T& value) {
        elements.push_back(value);
        cout << "추가: " << value << endl;
    }

    // 요소 제거 및 반환
    T pop() {
        if (elements.empty()) {
            throw runtime_error("Stack이 비어있습니다");
        }
        T value = elements.back();
        elements.pop_back();
        cout << "제거: " << value << endl;
        return value;
    }

    // 비어있는지 확인
    bool empty() const {
        return elements.empty();
    }

    // 크기
    int size() const {
        return elements.size();
    }
};

int main() {
    cout << "=== 정수 스택 ===" << endl;
    Stack<int> intStack;
    intStack.push(10);
    intStack.push(20);
    intStack.push(30);

    cout << "크기: " << intStack.size() << endl;
    cout << intStack.pop() << endl;  // 30
    cout << intStack.pop() << endl;  // 20

    cout << "\n=== 문자열 스택 ===" << endl;
    Stack<string> stringStack;
    stringStack.push("apple");
    stringStack.push("banana");
    stringStack.push("cherry");

    cout << stringStack.pop() << endl;  // cherry
    cout << stringStack.pop() << endl;  // banana

    return 0;
}
```

**실행 결과:**
```
=== 정수 스택 ===
추가: 10
추가: 20
추가: 30
크기: 3
제거: 30
30
제거: 20
20

=== 문자열 스택 ===
추가: apple
추가: banana
추가: cherry
제거: cherry
cherry
제거: banana
banana
```

### 2.2 큐 (Queue) 구현

```cpp
#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;

// 큐 (FIFO: First In First Out)
template <typename T>
class Queue {
private:
    vector<T> elements;

public:
    void enqueue(const T& value) {
        elements.push_back(value);
    }

    T dequeue() {
        if (elements.empty()) {
            throw runtime_error("Queue가 비어있습니다");
        }
        T value = elements.front();
        elements.erase(elements.begin());
        return value;
    }

    bool empty() const {
        return elements.empty();
    }

    int size() const {
        return elements.size();
    }
};

int main() {
    Queue<int> queue;
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);

    cout << "나간 순서: ";
    while (!queue.empty()) {
        cout << queue.dequeue() << " ";
    }
    cout << endl;

    return 0;
}
```

**실행 결과:**
```
나간 순서: 1 2 3
```

---

### ✅ **자기점검 Quiz 3.2**

스택과 큐의 차이를 설명하세요:
- 스택: _____ 원칙
- 큐: _____ 원칙

<details>
<summary>정답 보기</summary>

- 스택: **LIFO** (Last In First Out) - 마지막에 들어온 것이 먼저 나감
- 큐: **FIFO** (First In First Out) - 먼저 들어온 것이 먼저 나감

시각:
```
스택 (책 쌓기):
[10]
[20]
[30] ← pop = 30

큐 (줄서기):
[10] → 먼저 나감
[20]
[30]
```
</details>

---

## 📚 **Day 4: 람다 함수 (Lambda Function)**

### 3.1 람다란?

**람다 = 이름 없는 일회용 함수**

```cpp
// 일반 함수
int square(int x) {
    return x * x;
}

// 람다 (더 간단!)
auto square = [](int x) { return x * x; };

// 사용
cout << square(5) << endl;  // 25
```

### 3.2 람다 문법 상세

```cpp
//  [캡처] (매개변수) { 함수체 }
//   ↑        ↑         ↑
//   |        |         └─ 함수의 내용
//   |        └─ 입력값
//   └─ 외부 변수 사용 방법

auto add = [](int a, int b) { return a + b; };
cout << add(3, 5) << endl;  // 8
```

### 3.3 람다로 콜백 함수 만들기

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 1️⃣ 짝수만 필터링
    vector<int> evens;
    for (int n : numbers) {
        if (n % 2 == 0) {
            evens.push_back(n);
        }
    }

    cout << "짝수: ";
    for (int n : evens) {
        cout << n << " ";
    }
    cout << endl;

    // 2️⃣ 모든 원소에 2를 곱하기
    vector<int> doubled;
    for (int n : numbers) {
        doubled.push_back(n * 2);
    }

    cout << "2배: ";
    for (int n : doubled) {
        cout << n << " ";
    }
    cout << endl;

    return 0;
}
```

### 3.4 람다의 캡처 (Capture)

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    int multiplier = 10;
    vector<int> numbers = {1, 2, 3, 4, 5};

    // 1️⃣ [=] : 모든 변수를 값으로 캡처 (복사)
    auto multiply_copy = [=](int x) {
        return x * multiplier;
    };

    cout << "값 캡처 (복사): ";
    for (int n : numbers) {
        cout << multiply_copy(n) << " ";
    }
    cout << endl;

    // 2️⃣ [&] : 모든 변수를 참조로 캡처 (원본 접근)
    int sum = 0;
    auto add_to_sum = [&](int x) {
        sum += x;  // sum을 수정
    };

    for (int n : numbers) {
        add_to_sum(n);
    }

    cout << "합: " << sum << endl;  // 15

    // 3️⃣ [x, &y] : 혼합
    int x = 10;
    int y = 20;
    auto modify = [x, &y]() {
        // x는 복사된 값 (수정 불가)
        // y는 원본 (수정 가능)
        y = y + x;
    };

    modify();
    cout << "y 수정 후: " << y << endl;  // 30

    return 0;
}
```

**실행 결과:**
```
값 캡처 (복사): 10 20 30 40 50
합: 15
y 수정 후: 30
```

---

### ⚠️ **흔한 실수 #9: 라이프타임 문제**

```cpp
❌ 위험한 코드:
auto create_lambda() {
    int x = 42;
    return [&x]() { return x; };  // ← x를 참조로 캡처
}

int main() {
    auto lambda = create_lambda();
    cout << lambda() << endl;  // 💥 크래시! x는 이미 소멸됨
}
```

```cpp
✅ 안전한 코드:
auto create_lambda() {
    int x = 42;
    return [x]() { return x; };  // ← 값으로 캡처 (복사)
}

int main() {
    auto lambda = create_lambda();
    cout << lambda() << endl;  // 42 (안전!)
}
```

---

### ✅ **자기점검 Quiz 3.3**

다음 람다의 출력은?

```cpp
int value = 10;
auto increment = [&value]() { value++; };

increment();
cout << value << endl;
```

<details>
<summary>정답 보기</summary>

**출력**: `11`

설명:
- `increment`는 `value`를 참조로 캡처 ([&value])
- `increment()` 호출하면 실제 `value`가 증가
- 따라서 출력은 11
</details>

---

## 📚 **Day 5: STL 컨테이너**

### 4.1 STL 개요

**STL = Standard Template Library (표준 템플릿 라이브러리)**

```
컨테이너 (자료구조):
├─ vector: 동적 배열
├─ deque: 양쪽 끝에서 삽입/삭제
├─ map: 키-값 쌍
├─ set: 유일한 원소들
└─ ...

알고리즘:
├─ sort: 정렬
├─ find: 찾기
├─ transform: 변환
├─ ...

반복자 (Iterator):
└─ 포인터처럼 동작하는 객체
```

### 4.2 Vector (동적 배열)

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    // 1️⃣ 생성
    vector<int> v;
    cout << "초기 크기: " << v.size() << endl;  // 0

    // 2️⃣ 요소 추가
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    cout << "추가 후 크기: " << v.size() << endl;  // 3

    // 3️⃣ 접근
    cout << "첫 번째: " << v[0] << endl;  // 10
    cout << "마지막: " << v.back() << endl;  // 30

    // 4️⃣ 순회
    cout << "모든 요소: ";
    for (int x : v) {
        cout << x << " ";
    }
    cout << endl;

    // 5️⃣ 제거
    v.pop_back();  // 마지막 요소 제거
    cout << "제거 후 크기: " << v.size() << endl;  // 2

    return 0;
}
```

**실행 결과:**
```
초기 크기: 0
추가 후 크기: 3
첫 번째: 10
마지막: 30
모든 요소: 10 20 30
제거 후 크기: 2
```

### 4.3 Map (키-값 쌍)

```cpp
#include <iostream>
#include <map>
#include <string>
using namespace std;

int main() {
    // 1️⃣ 생성
    map<string, int> ages;

    // 2️⃣ 요소 추가
    ages["Kim"] = 25;
    ages["Lee"] = 30;
    ages["Park"] = 28;

    // 3️⃣ 조회
    cout << "Kim의 나이: " << ages["Kim"] << endl;

    // 4️⃣ 순회
    cout << "모든 사람: " << endl;
    for (auto& pair : ages) {
        cout << pair.first << " - " << pair.second << "세" << endl;
    }

    // 5️⃣ 존재 확인
    if (ages.find("Park") != ages.end()) {
        cout << "Park이 있습니다" << endl;
    }

    return 0;
}
```

**실행 결과:**
```
Kim의 나이: 25
모든 사람:
Kim - 25세
Lee - 30세
Park - 28세
Park이 있습니다
```

### 4.4 Set (유일한 원소)

```cpp
#include <iostream>
#include <set>
using namespace std;

int main() {
    // 1️⃣ 생성
    set<int> numbers;

    // 2️⃣ 요소 추가
    numbers.insert(5);
    numbers.insert(3);
    numbers.insert(5);  // 중복! 무시됨
    numbers.insert(1);
    numbers.insert(4);

    // 3️⃣ 크기
    cout << "크기: " << numbers.size() << endl;  // 4 (5는 1번만)

    // 4️⃣ 순회 (자동 정렬됨)
    cout << "원소: ";
    for (int x : numbers) {
        cout << x << " ";
    }
    cout << endl;

    return 0;
}
```

**실행 결과:**
```
크기: 4
원소: 1 3 4 5
```

---

## 📚 **Day 6: STL 알고리즘**

### 5.1 sort (정렬)

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    vector<int> v = {5, 2, 8, 1, 9, 3};

    // 1️⃣ 오름차순 정렬
    sort(v.begin(), v.end());
    cout << "오름차순: ";
    for (int x : v) cout << x << " ";
    cout << endl;

    // 2️⃣ 내림차순 정렬
    sort(v.begin(), v.end(), greater<int>());
    cout << "내림차순: ";
    for (int x : v) cout << x << " ";
    cout << endl;

    // 3️⃣ 람다로 커스텀 정렬
    sort(v.begin(), v.end(), [](int a, int b) {
        return abs(a) < abs(b);  // 절댓값 기준
    });
    cout << "절댓값 순: ";
    for (int x : v) cout << x << " ";
    cout << endl;

    return 0;
}
```

**실행 결과:**
```
오름차순: 1 2 3 5 8 9
내림차순: 9 8 5 3 2 1
절댓값 순: 1 2 3 5 8 9
```

### 5.2 find (찾기)

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    vector<int> v = {10, 20, 30, 40, 50};

    // 1️⃣ 정확한 값 찾기
    auto it = find(v.begin(), v.end(), 30);
    if (it != v.end()) {
        cout << "30을 찾음! 위치: " << (it - v.begin()) << endl;
    }

    // 2️⃣ 조건으로 찾기 (30보다 큼)
    auto it2 = find_if(v.begin(), v.end(), [](int x) {
        return x > 30;
    });
    if (it2 != v.end()) {
        cout << "30보다 큰 첫 원소: " << *it2 << endl;
    }

    return 0;
}
```

**실행 결과:**
```
30을 찾음! 위치: 2
30보다 큰 첫 원소: 40
```

### 5.3 transform (변환)

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    vector<int> original = {1, 2, 3, 4, 5};
    vector<int> result(original.size());

    // 각 원소를 2배로 변환
    transform(original.begin(), original.end(),
              result.begin(),
              [](int x) { return x * 2; });

    cout << "원래: ";
    for (int x : original) cout << x << " ";
    cout << endl;

    cout << "2배: ";
    for (int x : result) cout << x << " ";
    cout << endl;

    return 0;
}
```

**실행 결과:**
```
원래: 1 2 3 4 5
2배: 2 4 6 8 10
```

---

## 📚 **Day 7: 최종 프로젝트 - GenericSorter**

### 6.1 프로젝트 요구사항

**모든 타입을 정렬할 수 있는 라이브러리:**

1. ✅ 제네릭 클래스 (템플릿)
2. ✅ 여러 정렬 알고리즘 (QuickSort, MergeSort, HeapSort)
3. ✅ 람다로 커스텀 비교 함수
4. ✅ STL 알고리즘과 통합
5. ✅ 성능 벤치마크

### 6.2 완전한 GenericSorter 구현

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <chrono>
#include <memory>
using namespace std;

template <typename T>
class GenericSorter {
private:
    vector<T> data;

    // QuickSort 보조 함수
    void quickSortHelper(int left, int right, function<bool(T, T)> compare) {
        if (left >= right) return;

        T pivot = data[right];
        int i = left - 1;

        for (int j = left; j < right; j++) {
            if (compare(data[j], pivot)) {
                swap(data[++i], data[j]);
            }
        }
        swap(data[i+1], data[right]);

        quickSortHelper(left, i, compare);
        quickSortHelper(i + 2, right, compare);
    }

    // MergeSort 보조 함수
    void merge(int left, int mid, int right, function<bool(T, T)> compare) {
        vector<T> temp;
        int i = left, j = mid + 1;

        while (i <= mid && j <= right) {
            if (compare(data[i], data[j])) {
                temp.push_back(data[i++]);
            } else {
                temp.push_back(data[j++]);
            }
        }

        while (i <= mid) temp.push_back(data[i++]);
        while (j <= right) temp.push_back(data[j++]);

        for (int i = 0; i < temp.size(); i++) {
            data[left + i] = temp[i];
        }
    }

    void mergeSortHelper(int left, int right, function<bool(T, T)> compare) {
        if (left >= right) return;

        int mid = (left + right) / 2;
        mergeSortHelper(left, mid, compare);
        mergeSortHelper(mid + 1, right, compare);
        merge(left, mid, right, compare);
    }

public:
    GenericSorter(const vector<T>& initial_data) : data(initial_data) {}

    // QuickSort
    void quickSort(function<bool(T, T)> compare = less<T>()) {
        if (data.size() > 1) {
            quickSortHelper(0, data.size() - 1, compare);
        }
    }

    // MergeSort
    void mergeSort(function<bool(T, T)> compare = less<T>()) {
        if (data.size() > 1) {
            mergeSortHelper(0, data.size() - 1, compare);
        }
    }

    // 정렬 여부 확인
    bool isSorted(function<bool(T, T)> compare = less<T>()) const {
        for (int i = 0; i < data.size() - 1; i++) {
            if (compare(data[i+1], data[i])) {
                return false;
            }
        }
        return true;
    }

    // 데이터 출력
    void print(int count = 10) const {
        for (int i = 0; i < min((int)count, (int)data.size()); i++) {
            cout << data[i] << " ";
        }
        cout << endl;
    }

    const vector<T>& getData() const { return data; }
};

// 벤치마크 함수
template <typename T>
void benchmark(GenericSorter<T> sorter,
               const string& algorithm,
               function<void(GenericSorter<T>&)> sort_func) {
    auto start = chrono::high_resolution_clock::now();
    sort_func(sorter);
    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << algorithm << ": " << duration.count() << "ms";
    cout << " (정렬됨: " << (sorter.isSorted() ? "✓" : "✗") << ")" << endl;
}

int main() {
    cout << "=== Week 3 최종 프로젝트: GenericSorter ===" << endl << endl;

    // 1️⃣ 정수 정렬
    {
        cout << "1️⃣ 정수 정렬 (1,000개):" << endl;
        vector<int> ints(1000);
        for (int i = 0; i < 1000; i++) {
            ints[i] = rand() % 10000;
        }

        GenericSorter<int> sorter(ints);

        benchmark(sorter, "QuickSort (오름차순)",
            [](GenericSorter<int>& s) { s.quickSort(); });

        benchmark(sorter, "MergeSort (내림차순)",
            [](GenericSorter<int>& s) { s.mergeSort(greater<int>()); });

        cout << "앞의 10개: ";
        sorter.print(10);
        cout << endl;
    }

    // 2️⃣ 실수 정렬
    {
        cout << "2️⃣ 실수 정렬 (1,000개):" << endl;
        vector<double> doubles(1000);
        for (int i = 0; i < 1000; i++) {
            doubles[i] = (double)rand() / RAND_MAX * 100;
        }

        GenericSorter<double> sorter(doubles);
        sorter.quickSort();

        cout << "정렬됨: " << (sorter.isSorted() ? "✓" : "✗") << endl;
        cout << "앞의 10개: ";
        sorter.print(10);
        cout << endl;
    }

    // 3️⃣ 문자열 정렬
    {
        cout << "3️⃣ 문자열 정렬:" << endl;
        vector<string> words = {
            "zebra", "apple", "banana", "cherry", "date",
            "elephant", "fig", "grape", "house", "iris"
        };

        GenericSorter<string> sorter(words);
        sorter.quickSort();

        cout << "정렬된 단어: ";
        sorter.print(10);
        cout << endl;
    }

    // 4️⃣ 커스텀 정렬 (구조체)
    {
        cout << "4️⃣ 구조체 정렬:" << endl;

        struct Person {
            string name;
            int age;

            bool operator<(const Person& other) const {
                return age < other.age;  // 나이 기준 정렬
            }

            friend ostream& operator<<(ostream& os, const Person& p) {
                os << p.name << "(" << p.age << ")";
                return os;
            }
        };

        vector<Person> people = {
            {"Kim", 25},
            {"Lee", 30},
            {"Park", 22},
            {"Choi", 28}
        };

        GenericSorter<Person> sorter(people);
        sorter.quickSort();

        cout << "나이순: ";
        sorter.print(10);
        cout << endl;
    }

    cout << "=== 프로젝트 완료 ===" << endl;

    return 0;
}
```

### 6.3 빌드 및 실행

```bash
# 컴파일
g++ -std=c++17 -O2 -o generic_sorter generic_sorter.cpp

# 실행
./generic_sorter
```

**예상 출력:**
```
=== Week 3 최종 프로젝트: GenericSorter ===

1️⃣ 정수 정렬 (1,000개):
QuickSort (오름차순): 2ms (정렬됨: ✓)
MergeSort (내림차순): 2ms (정렬됨: ✓)
앞의 10개: 0 1 2 3 4 5 6 7 8 9

2️⃣ 실수 정렬 (1,000개):
정렬됨: ✓
앞의 10개: 0.00124 0.00234 0.00456 ...

3️⃣ 문자열 정렬:
정렬된 단어: apple banana cherry date elephant fig grape house iris zebra

4️⃣ 구조체 정렬:
나이순: Park(22) Kim(25) Choi(28) Lee(30)

=== 프로젝트 완료 ===
```

---

## ✅ **Week 3 최종 자기점검**

다음을 모두 할 수 있으면 Week 3 완료입니다!

- [ ] 함수 템플릿을 이해하고 작성할 수 있다
- [ ] 클래스 템플릿으로 제네릭 클래스를 만들 수 있다
- [ ] 람다 함수의 문법을 완벽히 안다
- [ ] 람다의 캡처 ([=], [&], 혼합)를 구분해서 사용한다
- [ ] vector, map, set을 자유롭게 사용할 수 있다
- [ ] sort, find, transform 같은 STL 알고리즘을 사용할 수 있다
- [ ] GenericSorter를 완성했고 모든 타입으로 동작한다
- [ ] 람다 캡처의 라이프타임 문제를 이해한다

---

## 📚 **추가 학습 (심화)**

### 더 배우고 싶다면?

- **범위 기반 for**: `for (auto& x : container)` 심화
- **반복자 (Iterator)**: begin(), end(), next() 활용
- **알고리즘 조합**: sort + transform + find
- **람다와 함수 객체**: 성능 비교
- **가변 템플릿**: `template <typename... Args>`

---

## 🔗 **다음: Week 4 최종 프로젝트 (모든 개념 통합)**

Week 1-3을 모두 합쳐서 **프로덕션급 정렬 라이브러리**를 만듭니다!

---

**팁**: 각 알고리즘을 손으로 종이에 그려가며 이해하세요. 그래야 진정으로 배워집니다! 📝

