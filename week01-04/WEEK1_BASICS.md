# 🚀 Week 1: C++ 기초 문법 & 개념

**목표**: 포인터, 클래스, 기본 객체지향 이해

**기간**: 2026-03-01 ~ 2026-03-07 (7일)

---

## 📚 **Day 1-2: 변수 & 포인터**

### 변수 선언 & 초기화

```cpp
#include <iostream>
using namespace std;

int main() {
    // 기본 자료형
    int x = 10;           // 정수
    double y = 3.14;      // 실수
    char c = 'A';         // 문자
    bool flag = true;     // 불린
    
    // 포인터 (주소를 저장하는 변수)
    int* ptr = &x;        // x의 주소를 ptr에 저장
    
    cout << "x = " << x << endl;
    cout << "주소 = " << &x << endl;
    cout << "포인터 = " << ptr << endl;
    cout << "역참조 = " << *ptr << endl;  // ptr이 가리키는 값
    
    return 0;
}
```

**실습 1**: 포인터로 변수값 변경
```cpp
int a = 5;
int* pa = &a;
*pa = 20;  // a의 값이 20으로 변경됨
cout << a;  // 20 출력
```

---

### 동적 메모리 할당 (위험한 방법)

```cpp
#include <iostream>
using namespace std;

int main() {
    // 힙에 메모리 할당
    int* arr = new int[10];
    
    // 사용
    for (int i = 0; i < 10; i++) {
        arr[i] = i * 2;
    }
    
    // 반드시 해제!
    delete[] arr;
    arr = nullptr;  // 댕글링 포인터 방지
    
    return 0;
}
```

**주의**: `delete` 안 하면 메모리 누수!

---

## 📚 **Day 3-4: 함수 & 클래스**

### 함수 작성

```cpp
#include <iostream>
using namespace std;

// 함수 선언 (프로토타입)
int add(int a, int b);
void print(int x);

int main() {
    int result = add(3, 5);
    print(result);
    
    return 0;
}

// 함수 정의
int add(int a, int b) {
    return a + b;
}

void print(int x) {
    cout << "결과: " << x << endl;
}
```

**실습 2**: 배열의 합 구하는 함수
```cpp
int sum_array(int arr[], int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}
```

---

### 클래스 정의 (객체지향)

```cpp
#include <iostream>
using namespace std;

class Vector {
private:
    float x, y;
    
public:
    // 생성자
    Vector(float x = 0, float y = 0) : x(x), y(y) {}
    
    // 메서드
    float magnitude() {
        return sqrt(x*x + y*y);
    }
    
    void print() {
        cout << "(" << x << ", " << y << ")" << endl;
    }
};

int main() {
    Vector v(3, 4);
    v.print();
    cout << "크기: " << v.magnitude() << endl;
    
    return 0;
}
```

**실습 3**: Point 클래스 작성
```cpp
class Point {
private:
    int x, y;
public:
    Point(int x, int y) : x(x), y(y) {}
    
    int getX() { return x; }
    int getY() { return y; }
    
    double distance_to(Point other) {
        return sqrt(pow(x-other.x, 2) + pow(y-other.y, 2));
    }
};
```

---

## 📚 **Day 5-6: 연산자 오버로딩**

```cpp
#include <iostream>
using namespace std;

class Complex {
public:
    double real, imag;
    
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}
    
    // 덧셈 연산자 오버로딩
    Complex operator+(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }
    
    // 곱셈 연산자 오버로딩
    Complex operator*(const Complex& other) const {
        return Complex(
            real*other.real - imag*other.imag,
            real*other.imag + imag*other.real
        );
    }
    
    void print() {
        cout << real << " + " << imag << "i" << endl;
    }
};

int main() {
    Complex a(1, 2);
    Complex b(3, 4);
    
    Complex c = a + b;  // 연산자 오버로딩 사용
    c.print();  // 4 + 6i
    
    return 0;
}
```

---

## 📚 **Day 7: 정렬 알고리즘 (첫 프로젝트)**

### 프로젝트: 배열 정렬 클래스

**요구사항**:
1. 버블 정렬 구현
2. 퀵 정렬 구현
3. 성능 비교

**코드**:

```cpp
#include <iostream>
#include <chrono>
#include <cstring>
using namespace std;

class Sorter {
private:
    int* arr;
    int size;
    
public:
    Sorter(int size) : size(size) {
        arr = new int[size];
    }
    
    ~Sorter() {
        delete[] arr;
    }
    
    void fill_random() {
        for (int i = 0; i < size; i++) {
            arr[i] = rand() % 1000;
        }
    }
    
    // 버블 정렬
    void bubble_sort() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (arr[j] > arr[j+1]) {
                    swap(arr[j], arr[j+1]);
                }
            }
        }
    }
    
    // 퀵 정렬
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
    
    void print_first_10() {
        for (int i = 0; i < min(10, size); i++) {
            cout << arr[i] << " ";
        }
        cout << endl;
    }
};

int main() {
    int size = 10000;
    
    // 버블 정렬 테스트
    {
        Sorter s(size);
        s.fill_random();
        
        auto start = chrono::high_resolution_clock::now();
        s.bubble_sort();
        auto end = chrono::high_resolution_clock::now();
        
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "버블 정렬: " << duration.count() << "ms" << endl;
        cout << "정렬 확인: " << (s.is_sorted() ? "OK" : "FAIL") << endl;
    }
    
    // 퀵 정렬 테스트
    {
        Sorter s(size);
        s.fill_random();
        
        auto start = chrono::high_resolution_clock::now();
        s.quick_sort();
        auto end = chrono::high_resolution_clock::now();
        
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "퀵 정렬: " << duration.count() << "ms" << endl;
        cout << "정렬 확인: " << (s.is_sorted() ? "OK" : "FAIL") << endl;
    }
    
    return 0;
}
```

---

## 🎯 **Week 1 체크리스트**

- [ ] 포인터 개념 이해 & 실습
- [ ] 동적 메모리 할당/해제
- [ ] 함수 작성 & 호출
- [ ] 클래스 정의 & 인스턴스 생성
- [ ] 연산자 오버로딩
- [ ] 정렬 알고리즘 구현 & 성능 비교
- [ ] 코드 커밋 (Gogs에)

---

## 💾 **빌드 & 실행**

```bash
# 컴파일
g++ -std=c++17 -o sort sort.cpp

# 실행
./sort

# 예상 출력:
# 버블 정렬: 5234ms
# 정렬 확인: OK
# 퀵 정렬: 12ms
# 정렬 확인: OK
```

---

## 📖 **추가 학습**

- "C++ Primer" Chapter 2-3 (변수와 기본 타입)
- "C++ Primer" Chapter 7 (함수)
- "C++ Primer" Chapter 13 (클래스)

---

**다음**: Week 2 - 메모리 관리 & RAII 패턴
