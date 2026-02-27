# 🎓 Week 1: C++ 기초 (온라인 혼자 학습용)

**목표**: 포인터, 클래스, 객체지향 기초를 완벽히 이해하고 간단한 프로그램 작성하기

**난이도**: ⭐ 초급
**학습 시간**: 약 20시간 (1주일)
**필수 도구**: g++ 또는 clang++ 컴파일러

---

## 📖 학습 안내

이 교재는 **혼자서 온라인으로 공부**할 수 있도록 만들어졌습니다:
- 💻 **코드는 복사해서 바로 실행** 가능
- 🤔 **왜?** 질문에 대한 답변 포함
- ⚠️ **흔한 실수** 섹션으로 막힐 때 도움
- ✅ **자기점검 퀴즈**로 이해도 확인
- 🔗 **추가 학습 자료** 제공

> **팁**: 각 코드 예제를 직접 입력하고 실행해보세요. 그래야 배워집니다!

---

## 📚 **Day 1: 변수와 메모리 이해하기**

### 1.1 변수란 무엇인가?

**변수 = 이름이 있는 상자**

```
┌─────────────────────┐
│    x = 10           │
│  (변수 x에 10 저장)  │
└─────────────────────┘
```

컴퓨터 메모리는 **주소(Address)**라는 번호가 매겨진 칸들로 이루어져 있습니다.

```
메모리의 모양:
┌──────┬──────┬──────┬──────┬──────┐
│ 1000 │ 1001 │ 1002 │ 1003 │ 1004 │  ← 주소 (위치)
├──────┼──────┼──────┼──────┼──────┤
│  10  │  20  │  30  │  40  │  50  │  ← 값 (데이터)
└──────┴──────┴──────┴──────┴──────┘
```

**변수는 이 칸에 이름을 붙인 것**입니다:

```cpp
int x = 10;   // x라는 이름으로 10을 저장
// x의 위치: 메모리 1000번
// x의 값: 10
```

### 1.2 기본 자료형 5가지

**프로그래밍을 시작하려면 이 5가지만 알면 됩니다:**

```cpp
#include <iostream>
using namespace std;

int main() {
    // 1️⃣ 정수 (integer) - 소수점 없는 수
    int age = 25;
    cout << "나이: " << age << endl;  // 출력: 나이: 25

    // 2️⃣ 실수 (float/double) - 소수점 있는 수
    double height = 1.75;
    cout << "키: " << height << endl;  // 출력: 키: 1.75

    // 3️⃣ 문자 (char) - 한 글자만
    char grade = 'A';
    cout << "학점: " << grade << endl;  // 출력: 학점: A

    // 4️⃣ 불린 (bool) - 참/거짓
    bool is_student = true;  // true는 참, false는 거짓
    cout << "학생인가: " << is_student << endl;  // 출력: 학생인가: 1 (1=참)

    // 5️⃣ 문자열 (string) - 여러 글자
    string name = "Kim";
    cout << "이름: " << name << endl;  // 출력: 이름: Kim

    return 0;
}
```

**실행 결과:**
```
나이: 25
키: 1.75
학점: A
학생인가: 1
이름: Kim
```

### 1.3 변수 사용하기 - 실전 예제

**예제 1: 원의 넓이 계산**

```cpp
#include <iostream>
using namespace std;

int main() {
    // 반지름 입력
    double radius = 5.0;

    // 원의 넓이 = π × r²
    double pi = 3.14159;
    double area = pi * radius * radius;

    cout << "반지름: " << radius << endl;
    cout << "원의 넓이: " << area << endl;

    return 0;
}
```

**실행 결과:**
```
반지름: 5
원의 넓이: 78.5398
```

---

### ⚠️ **흔한 실수 #1: 초기화 안 함**

```cpp
❌ 틀린 코드:
int x;  // 초기화하지 않음
cout << x << endl;  // 0인가? 999999인가? 모른다!
```

**문제**: 초기화하지 않으면 예측 불가능한 값이 나옵니다.

```cpp
✅ 올바른 코드:
int x = 0;  // 명시적으로 0으로 초기화
cout << x << endl;  // 항상 0 출력
```

### ⚠️ **흔한 실수 #2: 자료형 혼동**

```cpp
❌ 틀린 코드:
int result = 10 / 3;
cout << result << endl;  // 출력: 3 (소수점 버림!)
```

**문제**: int는 정수만 저장하므로 소수점이 없어집니다.

```cpp
✅ 올바른 코드:
double result = 10.0 / 3.0;
cout << result << endl;  // 출력: 3.33333
```

---

### ✅ **자기점검 Quiz 1.1**

다음 코드를 실행하면 무엇이 출력될까요?

```cpp
int x = 5;
int y = 2;
int z = x * y;
cout << z << endl;
```

<details>
<summary>정답 보기</summary>

**출력**: `10`

설명:
- x = 5
- y = 2
- z = 5 * 2 = 10
- 따라서 10 출력
</details>

---

## 📚 **Day 2: 포인터 (가장 중요!)**

### 2.1 포인터란?

**포인터 = 메모리 주소를 저장하는 변수**

```
일반 변수:
┌────────────┐
│ x = 10     │  x의 값은 10
└────────────┘

포인터 변수:
┌────────────┐
│ ptr = 1000 │  ptr의 값은 1000 (x의 주소)
└────────────┘
```

### 2.2 포인터 문법

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 10;

    // 1️⃣ & (주소 연산자): 변수의 주소를 얻음
    int* ptr = &x;  // "ptr은 x의 주소를 저장한다"

    cout << "x의 값: " << x << endl;
    cout << "x의 주소: " << &x << endl;
    cout << "ptr의 값: " << ptr << endl;

    // 2️⃣ * (역참조 연산자): 주소가 가리키는 값을 얻음
    cout << "*ptr의 값: " << *ptr << endl;  // 10 (x와 같음)

    return 0;
}
```

**실행 결과:**
```
x의 값: 10
x의 주소: 0x7ffc9e8c1eac
ptr의 값: 0x7ffc9e8c1eac
*ptr의 값: 10
```

### 2.3 포인터로 값 변경하기

**중요**: 포인터를 통해 원래 변수의 값을 바꿀 수 있습니다!

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 5;
    int* ptr = &x;

    cout << "변경 전: x = " << x << endl;  // 5

    // 포인터를 통해 x의 값을 변경
    *ptr = 20;

    cout << "변경 후: x = " << x << endl;  // 20

    return 0;
}
```

**실행 결과:**
```
변경 전: x = 5
변경 후: x = 20
```

**이게 왜 중요한가?**
- 함수에 변수를 "진짜로" 변경시키고 싶을 때
- 배열이나 문자열 같은 큰 데이터를 효율적으로 전달할 때

### 2.4 포인터로 함수에 값 전달하기

```cpp
#include <iostream>
using namespace std;

// 포인터를 받는 함수
void increase(int* num) {
    *num = *num + 1;  // 포인터가 가리키는 값을 1 증가
}

int main() {
    int x = 5;

    cout << "변경 전: " << x << endl;  // 5

    increase(&x);  // x의 주소를 전달

    cout << "변경 후: " << x << endl;  // 6

    return 0;
}
```

**실행 결과:**
```
변경 전: 5
변경 후: 6
```

---

### ⚠️ **흔한 실수 #3: 포인터 오류**

```cpp
❌ 틀린 코드:
int x = 5;
int* ptr;      // 초기화 안 함
*ptr = 10;     // 위험! 알 수 없는 메모리에 접근
```

**문제**: 포인터가 어디를 가리키는지 모르는데 값을 쓰려고 함 = 크래시!

```cpp
✅ 올바른 코드:
int x = 5;
int* ptr = &x;  // 반드시 유효한 주소로 초기화
*ptr = 10;      // 안전함!
```

### ⚠️ **흔한 실수 #4: * 와 & 헷갈리기**

```cpp
int x = 5;
int* ptr = &x;  // & = "주소를 얻어라"

cout << ptr << endl;   // 주소 출력 (0x...)
cout << *ptr << endl;  // 5 출력 (가리키는 값)
cout << &x << endl;    // 같은 주소 (0x...)
```

---

### ✅ **자기점검 Quiz 2.1**

```cpp
int a = 10;
int* p = &a;
*p = 20;
cout << a << endl;
```

이 코드는 무엇을 출력할까요?

<details>
<summary>정답 보기</summary>

**출력**: `20`

설명:
- a는 10으로 초기화됨
- p는 a의 주소를 저장
- `*p = 20`은 a의 값을 20으로 변경
- 따라서 a는 20
</details>

---

## 📚 **Day 3: 함수 (재사용 가능한 코드)**

### 3.1 함수란?

**함수 = 자주 쓰는 코드를 한 번에 묶어놓은 것**

```cpp
// 함수 정의
void greet() {
    cout << "안녕하세요!" << endl;
}

// 함수 사용
int main() {
    greet();  // 함수 호출
    greet();  // 또 호출
    greet();  // 또 호출
    return 0;
}
```

**출력:**
```
안녕하세요!
안녕하세요!
안녕하세요!
```

### 3.2 함수의 4가지 패턴

**패턴 1: 입력도 없고 출력도 없음**

```cpp
void printHello() {
    cout << "Hello!" << endl;
}

int main() {
    printHello();  // 입력도 반환도 없음
    return 0;
}
```

**패턴 2: 입력은 있고 출력은 없음**

```cpp
void printNumber(int num) {
    cout << "숫자: " << num << endl;
}

int main() {
    printNumber(42);  // 42를 입력으로 전달
    return 0;
}
```

**패턴 3: 입력은 없고 출력은 있음**

```cpp
int getRandom() {
    return 42;  // 함수가 값을 돌려줌
}

int main() {
    int result = getRandom();
    cout << result << endl;  // 42 출력
    return 0;
}
```

**패턴 4: 입력도 있고 출력도 있음** ⭐ **가장 자주 쓰임**

```cpp
#include <iostream>
using namespace std;

// 두 수를 더하는 함수
int add(int a, int b) {
    int result = a + b;
    return result;  // 합을 돌려줌
}

int main() {
    int sum = add(3, 5);
    cout << "3 + 5 = " << sum << endl;  // 8 출력
    return 0;
}
```

### 3.3 함수 구조 상세 설명

```cpp
int add(int a, int b) {
//  ↑  ↑    ↑  ↑
//  |  |    |  └─ 매개변수 (입력값들)
//  |  |    └───── 매개변수 타입
//  |  └────────── 함수명
//  └───────────── 반환 타입 (int = 정수를 돌려준다)

    return a + b;
    // ↑
    // 이 값을 함수를 호출한 곳으로 돌려줌
}
```

### 3.4 실전 예제: 계산기

```cpp
#include <iostream>
using namespace std;

double divide(double num1, double num2) {
    if (num2 == 0) {
        cout << "오류: 0으로 나눌 수 없습니다!" << endl;
        return 0;
    }
    return num1 / num2;
}

int main() {
    cout << "10 / 2 = " << divide(10, 2) << endl;
    cout << "10 / 0 = " << divide(10, 0) << endl;

    return 0;
}
```

**실행 결과:**
```
10 / 2 = 5
오류: 0으로 나눌 수 없습니다!
10 / 0 = 0
```

---

### ⚠️ **흔한 실수 #5: 함수 선언 순서**

```cpp
❌ 틀린 코드:
int main() {
    int sum = add(3, 5);  // add 함수를 아직 정의하지 않았음!
    return 0;
}

int add(int a, int b) {
    return a + b;
}
// 컴파일 오류!
```

**문제**: C++는 함수를 사용하기 **전에** 정의해야 합니다.

```cpp
✅ 올바른 코드 (방법 1):
int add(int a, int b) {
    return a + b;
}

int main() {
    int sum = add(3, 5);  // 이제 괜찮음
    return 0;
}
```

```cpp
✅ 올바른 코드 (방법 2):
int add(int a, int b);  // 선언 (함수 원형)

int main() {
    int sum = add(3, 5);  // 이제 괜찮음
    return 0;
}

int add(int a, int b) {  // 정의
    return a + b;
}
```

---

### ✅ **자기점검 Quiz 3.1**

다음 함수를 작성하세요:
- 함수명: `multiply`
- 입력: 두 정수 (a, b)
- 출력: a * b의 결과

<details>
<summary>정답 보기</summary>

```cpp
int multiply(int a, int b) {
    return a * b;
}
```

사용 예:
```cpp
int result = multiply(3, 4);  // 12
```
</details>

---

## 📚 **Day 4-5: 클래스 (객체지향의 시작)**

### 4.1 클래스란?

**클래스 = 변수와 함수를 묶어놓은 것**

```
전에: 변수와 함수가 따로따로
int x = 5;
int y = 3;
void printPoint() { ... }  // x, y와 무관해 보임

지금: 함수 안에 변수가 있어서 관계가 명확
class Point {
    int x = 5;
    int y = 3;
    void print() { ... }   // x, y를 표현하는 함수
};
```

### 4.2 간단한 클래스 작성

```cpp
#include <iostream>
using namespace std;

class Rectangle {
    // 멤버 변수 (데이터)
    int width = 10;
    int height = 5;

public:
    // 멤버 함수 (행동)
    void printArea() {
        int area = width * height;
        cout << "넓이: " << area << endl;
    }
};

int main() {
    Rectangle rect;  // Rectangle 객체 생성
    rect.printArea();  // 멤버 함수 호출

    return 0;
}
```

**실행 결과:**
```
넓이: 50
```

### 4.3 생성자로 초기화하기

```cpp
#include <iostream>
using namespace std;

class Rectangle {
    int width;
    int height;

public:
    // 생성자 (객체를 만들 때 자동으로 호출)
    Rectangle(int w, int h) {
        width = w;
        height = h;
    }

    void printArea() {
        int area = width * height;
        cout << "넓이: " << area << endl;
    }
};

int main() {
    Rectangle rect1(10, 5);   // width=10, height=5
    Rectangle rect2(20, 3);   // width=20, height=3

    rect1.printArea();  // 50 출력
    rect2.printArea();  // 60 출력

    return 0;
}
```

**실행 결과:**
```
넓이: 50
넓이: 60
```

### 4.4 getter/setter 패턴

```cpp
#include <iostream>
using namespace std;

class BankAccount {
    int balance = 0;  // 비공개 (private)

public:
    // 잔액 조회 (getter)
    int getBalance() {
        return balance;
    }

    // 입금 (setter)
    void deposit(int amount) {
        if (amount > 0) {
            balance = balance + amount;
            cout << amount << "원 입금했습니다" << endl;
        }
    }

    // 출금 (setter)
    void withdraw(int amount) {
        if (amount > 0 && amount <= balance) {
            balance = balance - amount;
            cout << amount << "원 출금했습니다" << endl;
        } else {
            cout << "출금할 수 없습니다" << endl;
        }
    }
};

int main() {
    BankAccount account;

    account.deposit(10000);    // 10000 입금
    account.deposit(5000);     // 5000 입금
    cout << "잔액: " << account.getBalance() << endl;  // 15000

    account.withdraw(3000);    // 3000 출금
    cout << "잔액: " << account.getBalance() << endl;  // 12000

    return 0;
}
```

**실행 결과:**
```
10000원 입금했습니다
5000원 입금했습니다
잔액: 15000
3000원 출금했습니다
잔액: 12000
```

---

### ⚠️ **흔한 실수 #6: private vs public 혼동**

```cpp
❌ 안 좋은 코드:
class BankAccount {
public:
    int balance = 0;  // public이면 누구나 접근 가능
};

int main() {
    BankAccount account;
    account.balance = -999999;  // 음수 입금? 위험!
    return 0;
}
```

```cpp
✅올바른 코드:
class BankAccount {
    int balance = 0;  // private (보호됨)

public:
    void deposit(int amount) {
        if (amount > 0) {  // 검증!
            balance = balance + amount;
        }
    }
};
```

---

### ✅ **자기점검 Quiz 4.1**

다음 클래스를 작성하세요:

- 클래스명: `Student`
- 멤버 변수: `name` (문자열), `score` (정수)
- 멤버 함수: `printInfo()` (이름과 점수 출력)
- 생성자로 name과 score 초기화

<details>
<summary>정답 보기</summary>

```cpp
#include <iostream>
#include <string>
using namespace std;

class Student {
    string name;
    int score;

public:
    Student(string n, int s) {
        name = n;
        score = s;
    }

    void printInfo() {
        cout << name << ": " << score << "점" << endl;
    }
};

int main() {
    Student s1("Kim", 95);
    Student s2("Lee", 87);

    s1.printInfo();  // Kim: 95점
    s2.printInfo();  // Lee: 87점

    return 0;
}
```
</details>

---

## 📚 **Day 6-7: 최종 프로젝트 (정렬 알고리즘)**

### 5.1 프로젝트: 간단한 정렬기

지금까지 배운 모든 개념을 사용해서 프로그램을 만들어봅시다!

**요구사항**:
1. 배열을 받아서 정렬하는 클래스
2. 두 가지 정렬 방식 (버블 정렬, 퀵 정렬)
3. 정렬되었는지 확인하는 함수

### 5.2 완전한 코드

```cpp
#include <iostream>
#include <cstdlib>  // rand() 함수
#include <chrono>   // 시간 측정
using namespace std;

class SimpleSorter {
private:
    int* arr;
    int size;

public:
    // 생성자: 배열 할당
    SimpleSorter(int s) : size(s) {
        arr = new int[size];
    }

    // 소멸자: 메모리 해제
    ~SimpleSorter() {
        delete[] arr;
    }

    // 랜덤 데이터 채우기
    void fillRandom() {
        for (int i = 0; i < size; i++) {
            arr[i] = rand() % 1000;
        }
    }

    // 버블 정렬 (느리지만 이해하기 쉬움)
    void bubbleSort() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                // 인접한 두 원소 비교
                if (arr[j] > arr[j+1]) {
                    // 작은 것이 앞으로 오도록 교환
                    int temp = arr[j];
                    arr[j] = arr[j+1];
                    arr[j+1] = temp;
                }
            }
        }
    }

    // 빠른 정렬 (시간이 덜 걸림)
    void quickSort() {
        quickSortHelper(0, size - 1);
    }

    // 빠른 정렬 보조 함수
    private:
    void quickSortHelper(int left, int right) {
        if (left >= right) return;

        // 피벗 선택
        int pivot = arr[right];
        int i = left - 1;

        // 분할
        for (int j = left; j < right; j++) {
            if (arr[j] < pivot) {
                i++;
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }

        // 피벗 위치 정하기
        int temp = arr[i+1];
        arr[i+1] = arr[right];
        arr[right] = temp;

        // 재귀
        quickSortHelper(left, i);
        quickSortHelper(i + 2, right);
    }

    public:
    // 정렬되었는지 확인
    bool isSorted() {
        for (int i = 0; i < size - 1; i++) {
            if (arr[i] > arr[i+1]) {
                return false;
            }
        }
        return true;
    }

    // 앞의 10개만 출력
    void print() {
        cout << "앞의 10개: ";
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
        SimpleSorter sorter(size);
        sorter.fillRandom();

        auto start = chrono::high_resolution_clock::now();
        sorter.bubbleSort();
        auto end = chrono::high_resolution_clock::now();

        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

        cout << "== 버블 정렬 ==" << endl;
        cout << "시간: " << duration.count() << "ms" << endl;
        cout << "정렬 확인: " << (sorter.isSorted() ? "✓" : "✗") << endl;
        sorter.print();
    }

    // 빠른 정렬 테스트
    {
        SimpleSorter sorter(size);
        sorter.fillRandom();

        auto start = chrono::high_resolution_clock::now();
        sorter.quickSort();
        auto end = chrono::high_resolution_clock::now();

        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

        cout << "\n== 빠른 정렬 ==" << endl;
        cout << "시간: " << duration.count() << "ms" << endl;
        cout << "정렬 확인: " << (sorter.isSorted() ? "✓" : "✗") << endl;
        sorter.print();
    }

    return 0;
}
```

### 5.3 이 코드를 실행하려면

```bash
# 1. 파일 저장 (sorter.cpp)
# 2. 컴파일
g++ -std=c++17 -O2 -o sorter sorter.cpp

# 3. 실행
./sorter
```

**예상 결과:**
```
== 버블 정렬 ==
시간: 5234ms
정렬 확인: ✓
앞의 10개: 0 1 2 3 4 5 6 7 8 9

== 빠른 정렬 ==
시간: 12ms
정렬 확인: ✓
앞의 10개: 0 1 2 3 4 5 6 7 8 9
```

**왜 빠른 정렬이 400배 빠를까?**
- 버블 정렬: O(n²) = 10,000² = 약 100,000,000번 비교
- 빠른 정렬: O(n log n) ≈ 10,000 × 14 = 약 140,000번 비교

---

## ✅ **Week 1 최종 자기점검**

다음을 모두 할 수 있으면 Week 1 완료입니다!

- [ ] 변수 5가지 자료형을 설명할 수 있다
- [ ] & (주소) 와 * (역참조)를 구분할 수 있다
- [ ] 포인터로 함수에 값을 전달할 수 있다
- [ ] 함수의 4가지 패턴을 작성할 수 있다
- [ ] 클래스를 작성하고 생성자를 만들 수 있다
- [ ] 정렬 프로그램을 직접 컴파일하고 실행할 수 있다
- [ ] 버블 정렬과 빠른 정렬의 속도 차이를 이해한다

---

## 📚 **추가 학습 (심화)**

### 더 배우고 싶다면?

- **배열과 포인터의 관계**: `int arr[10]` = `int* arr`
- **동적 배열**: `new` 와 `delete` 사용법
- **헤더 파일 분리**: `.h` 와 `.cpp` 파일 나누기
- **메모리 누수**: 왜 중요한가?

### 추천 연습 문제

1. **입력받기**: `cin >> x;` 로 사용자 입력받기
2. **조건문 확장**: 다양한 if-else 패턴
3. **반복문 확장**: while, do-while 루프
4. **배열 다루기**: 배열을 매개변수로 전달
5. **여러 클래스 사용**: 클래스끼리 상호작용

---

## 🔗 **다음: Week 2 메모리 관리 & RAII**

Week 1에서 배운 `new` 와 `delete` 를 더 안전하게 사용하는 방법을 배웁니다!

---

**마지막 팁**: 각 코드를 직접 입력하고 실행해보세요. 복사-붙여넣기는 배움이 적습니다! ✏️

