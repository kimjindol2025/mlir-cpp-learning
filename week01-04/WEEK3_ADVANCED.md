# 🚀 Week 3: 고급 기능 & STL

**목표**: 템플릿, 람다 함수, STL 컨테이너 마스터

**기간**: 2026-03-15 ~ 2026-03-21 (7일)

---

## 📚 **Day 1-2: 함수 템플릿 (Function Template)**

### 기본 개념

```cpp
#include <iostream>
using namespace std;

// ✅ 템플릿 함수 (어떤 타입도 가능)
template <typename T>
T max_value(T a, T b) {
    return (a > b) ? a : b;
}

// 사용
int main() {
    cout << max_value(3, 5);           // int: 5
    cout << max_value(3.5, 2.1);       // double: 3.5
    cout << max_value('a', 'z');       // char: z
    cout << max_value("hello", "world");  // string: world

    return 0;
}
```

### 다중 템플릿 매개변수

```cpp
template <typename T, typename U>
auto add(T a, U b) {
    return a + b;  // auto로 반환 타입 추론
}

// 사용
int main() {
    cout << add(3, 4);           // 7 (int)
    cout << add(3.5, 2);         // 5.5 (double)
    cout << add(3, 2.5);         // 5.5 (double)
}
```

---

## 📚 **Day 3-4: 클래스 템플릿**

### 제네릭 스택 구현

```cpp
template <typename T>
class Stack {
private:
    vector<T> elements;

public:
    void push(const T& value) {
        elements.push_back(value);
    }

    T pop() {
        if (elements.empty()) {
            throw runtime_error("Stack empty");
        }
        T value = elements.back();
        elements.pop_back();
        return value;
    }

    bool empty() const {
        return elements.empty();
    }

    size_t size() const {
        return elements.size();
    }
};

// 사용
int main() {
    // 정수 스택
    Stack<int> intStack;
    intStack.push(1);
    intStack.push(2);
    cout << intStack.pop();  // 2

    // 문자열 스택
    Stack<string> stringStack;
    stringStack.push("hello");
    stringStack.push("world");
    cout << stringStack.pop();  // world

    return 0;
}
```

### 제네릭 큐 구현

```cpp
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
            throw runtime_error("Queue empty");
        }
        T value = elements[0];
        elements.erase(elements.begin());
        return value;
    }

    bool empty() const {
        return elements.empty();
    }
};
```

---

## 📚 **Day 5-6: 람다 함수**

### 기본 문법

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// ✅ 람다 함수 기본
int main() {
    // 문법: [캡처] (매개변수) -> 반환타입 { 본문 }

    // 1. 단순 람다
    auto add = [](int a, int b) { return a + b; };
    cout << add(3, 5);  // 8

    // 2. 변수 캡처
    int x = 10;
    auto add_x = [x](int a) { return a + x; };  // x 캡처
    cout << add_x(5);  // 15

    // 3. 참조 캡처
    int y = 10;
    auto inc_y = [&y]() { y++; };  // y를 참조로 캡처
    inc_y();
    cout << y;  // 11

    // 4. 모든 변수 캡처
    int a = 10, b = 20;
    auto use_all = [=]() { return a + b; };  // 값 캡처
    auto modify_all = [&]() { a++; b++; };  // 참조 캡처

    return 0;
}
```

### STL 알고리즘과 람다

```cpp
#include <algorithm>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {3, 1, 4, 1, 5, 9, 2};

    // 정렬
    sort(v.begin(), v.end(),
         [](int a, int b) { return a > b; });  // 내림차순

    // 필터링
    auto it = find_if(v.begin(), v.end(),
                      [](int x) { return x > 5; });

    // 변환
    vector<int> squared(v.size());
    transform(v.begin(), v.end(), squared.begin(),
              [](int x) { return x * x; });

    // 순회
    for_each(v.begin(), v.end(),
             [](int x) { cout << x << " "; });

    return 0;
}
```

---

## 📚 **Day 7: Week 3 프로젝트 - 제네릭 정렬기**

### 제네릭 정렬 클래스

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
using namespace std;

template <typename T>
class GenericSorter {
private:
    unique_ptr<T[]> arr;
    int size;

    int partition(int left, int right,
                  function<bool(T, T)> compare) {
        T pivot = arr[right];
        int i = left - 1;

        for (int j = left; j < right; j++) {
            if (compare(arr[j], pivot)) {
                swap(arr[++i], arr[j]);
            }
        }
        swap(arr[i+1], arr[right]);
        return i + 1;
    }

public:
    GenericSorter(int s) : size(s) {
        arr = make_unique<T[]>(size);
    }

    void fill_random() {
        if constexpr (is_same_v<T, int>) {
            for (int i = 0; i < size; i++) {
                arr[i] = rand() % 1000;
            }
        }
    }

    void quick_sort(function<bool(T, T)> compare,
                    int left = 0, int right = -1) {
        if (right == -1) right = size - 1;
        if (left >= right) return;

        int pivot = partition(left, right, compare);
        quick_sort(compare, left, pivot - 1);
        quick_sort(compare, pivot + 1, right);
    }

    bool is_sorted(function<bool(T, T)> compare) {
        for (int i = 0; i < size - 1; i++) {
            if (compare(arr[i+1], arr[i])) {
                return false;
            }
        }
        return true;
    }

    void print(int count = 10) {
        for (int i = 0; i < min(count, size); i++) {
            cout << arr[i] << " ";
        }
        cout << endl;
    }
};

// 사용 예
int main() {
    // 정수 정렬 (오름차순)
    {
        GenericSorter<int> sorter(10000);
        sorter.fill_random();

        auto start = chrono::high_resolution_clock::now();
        sorter.quick_sort([](int a, int b) { return a < b; });
        auto end = chrono::high_resolution_clock::now();

        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "정수 정렬: " << duration.count() << "ms" << endl;
        cout << "검증: " << (sorter.is_sorted([](int a, int b) { return a > b; }) ? "OK" : "FAIL") << endl;
    }

    // 정수 정렬 (내림차순)
    {
        GenericSorter<int> sorter(10000);
        sorter.fill_random();

        sorter.quick_sort([](int a, int b) { return a > b; });
        cout << "검증: " << (sorter.is_sorted([](int a, int b) { return a < b; }) ? "OK" : "FAIL") << endl;
    }

    return 0;
}
```

---

## 📚 **STL 컨테이너 소개**

### vector (동적 배열)

```cpp
vector<int> v;
v.push_back(1);
v.push_back(2);
v[0] = 10;
for (int x : v) cout << x;
```

### deque (양쪽 큐)

```cpp
deque<int> d;
d.push_back(1);
d.push_front(0);
cout << d[0];  // 0
cout << d[1];  // 1
```

### set (유일한 원소)

```cpp
set<int> s;
s.insert(5);
s.insert(3);
s.insert(5);  // 중복이므로 무시
cout << s.size();  // 2
```

### map (키-값 쌍)

```cpp
map<string, int> m;
m["apple"] = 5;
m["banana"] = 3;
cout << m["apple"];  // 5
```

### unordered_map (해시 맵)

```cpp
unordered_map<string, int> m;
m["python"] = 1;
m["cpp"] = 2;
cout << m["python"];  // 1
```

---

## 🎯 **Week 3 체크리스트**

- [ ] 함수 템플릿 이해 및 사용
- [ ] 클래스 템플릿 이해 및 구현
- [ ] 람다 함수 기본 문법
- [ ] 변수 캡처 (값, 참조)
- [ ] STL 알고리즘 (sort, find, transform)
- [ ] STL 컨테이너 기본 사용
- [ ] GenericSorter 프로젝트 완성
- [ ] 템플릿 에러 메시지 이해 시작

---

## 📖 **추가 학습**

- "C++ Primer" Chapter 16 (템플릿)
- "C++ Primer" Chapter 10 (STL 알고리즘)
- "Effective C++" Item 41-45 (템플릿)

---

**다음**: Week 4 - 최종 프로젝트 (정렬 라이브러리)
