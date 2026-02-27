# 🚀 Week 2: 메모리 관리 & RAII 패턴

**목표**: 포인터 심화, 메모리 누수 방지, 스마트 포인터 마스터

**기간**: 2026-03-08 ~ 2026-03-14 (7일)

---

## 📚 **Day 1-2: 메모리 문제와 위험성**

### 문제 1: 메모리 누수 (Memory Leak)

```cpp
#include <iostream>
using namespace std;

// ❌ 나쁜 코드: 메모리 누수 발생
void bad_memory_leak() {
    int* arr = new int[1000];

    // 함수가 여기서 끝나면...
    // arr을 delete 하지 않음 → 메모리 누수!
}

// ✅ 좋은 코드: 메모리 해제
void good_memory_management() {
    int* arr = new int[1000];

    // 사용...

    delete[] arr;  // 반드시 해제!
    arr = nullptr; // 댕글링 포인터 방지
}
```

**문제점**:
- 메모리 할당 후 해제 잊음
- 프로그램 실행 중 메모리 부족
- 시스템 느려짐

### 문제 2: 댕글링 포인터 (Dangling Pointer)

```cpp
// ❌ 위험: 해제된 메모리 접근
int* ptr = new int(42);
delete ptr;

cout << *ptr;  // 💥 크래시! (정의되지 않은 동작)
ptr = nullptr; // 이미 늦음
```

### 문제 3: Double Delete

```cpp
// ❌ 위험: 같은 메모리 두 번 해제
int* p = new int(10);
delete p;
delete p;  // 💥 크래시!
```

**실습 1**: 메모리 누수 찾기
```cpp
// 이 코드에서 메모리 누수를 찾으세요
void find_leak() {
    int* a = new int(5);
    int* b = new int(10);

    if (a == nullptr) {
        // ...
        return;  // 메모리 누수!
    }

    delete a;
    delete b;
}
```

---

## 📚 **Day 3-4: RAII 패턴 (Resource Acquisition Is Initialization)**

### RAII의 원리

```cpp
// RAII = "객체 생성 = 리소스 획득, 객체 소멸 = 리소스 해제"

class File {
private:
    FILE* handle;

public:
    // 생성자: 리소스 획득
    File(const string& filename) {
        handle = fopen(filename.c_str(), "r");
        if (!handle) throw runtime_error("파일 열기 실패");
    }

    // 소멸자: 리소스 해제 (자동 호출!)
    ~File() {
        if (handle) {
            fclose(handle);
        }
    }

    // 복사 방지
    File(const File&) = delete;
    File& operator=(const File&) = delete;
};

// ✅ 사용
void safe_file_handling() {
    {
        File f("data.txt");
        // 파일 사용
    }  // 여기서 자동으로 파일이 닫힘 (소멸자 호출)
}
```

### unique_ptr: 유일 소유권

```cpp
#include <memory>
using namespace std;

void unique_ptr_demo() {
    // ✅ 안전: 자동 메모리 관리
    unique_ptr<int> ptr(new int(42));

    cout << *ptr;  // 사용 가능

    // 함수 끝나면 자동으로 delete (소멸자)
}

// unique_ptr 사용 예
class Node {
private:
    int value;
    unique_ptr<Node> left;
    unique_ptr<Node> right;

public:
    Node(int v) : value(v) {}

    void add_left(int v) {
        left = make_unique<Node>(v);
    }

    void add_right(int v) {
        right = make_unique<Node>(v);
    }

    // 소멸자는 자동 생성
    // left와 right가 자동으로 삭제됨!
};

// 이진 트리가 안전하게 관리됨
void safe_tree() {
    auto root = make_unique<Node>(1);
    root->add_left(2);
    root->add_right(3);

    // root 소멸 시 전체 트리가 자동 정리
}
```

### shared_ptr: 공유 소유권

```cpp
#include <memory>
using namespace std;

void shared_ptr_demo() {
    // 여러 포인터가 같은 메모리 공유
    shared_ptr<int> p1 = make_shared<int>(100);
    shared_ptr<int> p2 = p1;  // 공유
    shared_ptr<int> p3 = p1;  // 공유

    cout << *p1;  // 100
    cout << p1.use_count();  // 3 (3개의 포인터 소유)

    // p1, p2 소멸
    p1 = nullptr;
    p2 = nullptr;

    cout << p3.use_count();  // 1 (p3만 남음)

    // p3 소멸 시 메모리 해제
}

// 그래프 구조 (순환 참조 주의!)
class GraphNode {
    int id;
    vector<shared_ptr<GraphNode>> neighbors;

public:
    GraphNode(int id) : id(id) {}

    void add_neighbor(shared_ptr<GraphNode> n) {
        neighbors.push_back(n);
    }
};
```

**실습 2**: unique_ptr로 동적 배열 관리
```cpp
void array_with_unique_ptr() {
    unique_ptr<int[]> arr(new int[100]);

    arr[0] = 10;
    arr[1] = 20;

    // 함수 끝나면 자동으로 delete[]
}
```

---

## 📚 **Day 5-6: move 의미론과 이동 생성자**

### move란?

```cpp
// 일반 복사 (비효율)
vector<int> v1 = {1, 2, 3, 4, 5};
vector<int> v2 = v1;  // 전체 복사 (느림)

// move 사용 (효율적)
vector<int> v3 = move(v1);  // 메모리만 이동, 복사 아님
// v1은 이제 비어있음
```

### 이동 생성자 구현

```cpp
class Vector {
private:
    int* data;
    int size;

public:
    // 일반 생성자
    Vector(int s) : size(s) {
        data = new int[size];
    }

    // 복사 생성자 (느림)
    Vector(const Vector& other) : size(other.size) {
        data = new int[size];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    // 이동 생성자 (빠름!)
    Vector(Vector&& other) noexcept
        : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }

    // 이동 할당 연산자
    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            other.data = nullptr;
        }
        return *this;
    }

    ~Vector() {
        delete[] data;
    }
};

// 사용
Vector create_vector() {
    Vector v(1000);
    // ... 계산 ...
    return v;  // 이동 생성자 호출 (복사 아님)
}

void move_example() {
    Vector v = create_vector();  // 빠름!
}
```

---

## 📚 **Day 7: Week 1-2 프로젝트 개선**

### 과제: Week 1의 Sorter를 메모리 안전하게 개선

```cpp
#include <iostream>
#include <memory>
#include <chrono>
using namespace std;

class SafeSorter {
private:
    unique_ptr<int[]> arr;
    int size;

public:
    // 생성자
    SafeSorter(int s) : size(s) {
        arr = make_unique<int[]>(size);
    }

    // 이동 생성자 (큰 배열 이동)
    SafeSorter(SafeSorter&& other) noexcept
        : arr(move(other.arr)), size(other.size) {
        other.size = 0;
    }

    // 복사 방지
    SafeSorter(const SafeSorter&) = delete;
    SafeSorter& operator=(const SafeSorter&) = delete;

    void fill_random() {
        for (int i = 0; i < size; i++) {
            arr[i] = rand() % 1000;
        }
    }

    void quick_sort(int left = 0, int right = -1) {
        if (right == -1) right = size - 1;
        if (left >= right) return;

        int pivot = partition(left, right);
        quick_sort(left, pivot - 1);
        quick_sort(pivot + 1, right);
    }

private:
    int partition(int left, int right) {
        int pivot = arr[right];
        int i = left - 1;

        for (int j = left; j < right; j++) {
            if (arr[j] < pivot) {
                swap(arr[++i], arr[j]);
            }
        }
        swap(arr[i+1], arr[right]);
        return i + 1;
    }

public:
    bool is_sorted() {
        for (int i = 0; i < size - 1; i++) {
            if (arr[i] > arr[i+1]) return false;
        }
        return true;
    }
};

// 안전한 벡터 정렬
void sort_with_safety() {
    SafeSorter s(100000);
    s.fill_random();

    auto start = chrono::high_resolution_clock::now();
    s.quick_sort();
    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "정렬 시간: " << duration.count() << "ms" << endl;
    cout << "안전 확인: " << (s.is_sorted() ? "OK" : "FAIL") << endl;
}

int main() {
    sort_with_safety();

    // 프로그램 종료 시 자동으로 모든 메모리 해제
    // 메모리 누수 불가능!

    return 0;
}
```

---

## 🎯 **Week 2 체크리스트**

- [ ] 메모리 문제 이해 (누수, 댕글링, double delete)
- [ ] RAII 패턴 완전히 이해
- [ ] unique_ptr 사용 가능
- [ ] shared_ptr 사용 가능
- [ ] move 의미론 이해
- [ ] 이동 생성자 구현 가능
- [ ] SafeSorter 프로젝트 완성
- [ ] 메모리 누수 검증 (valgrind)

---

## 💾 **valgrind로 메모리 검증**

```bash
# 컴파일 (디버그 정보 포함)
g++ -g -std=c++17 -o sort sort.cpp

# valgrind로 메모리 검사
valgrind --leak-check=full ./sort

# 출력:
# ==12345== HEAP SUMMARY:
# ==12345== in use at exit: 0 bytes in 0 blocks
# ==12345== All heap blocks were freed -- no leaks possible
```

---

## 📖 **추가 학습**

- "C++ Primer" Chapter 12 (동적 메모리)
- "Effective C++" Item 17-21 (스마트 포인터)
- cppreference: unique_ptr, shared_ptr

---

**다음**: Week 3 - 고급 기능 (템플릿, 람다, STL)
