# 🎓 Week 2: 메모리 관리 & RAII (온라인 혼자 학습용)

**목표**: 메모리 누수 방지, RAII 패턴 완전 이해, 스마트 포인터 마스터

**난이도**: ⭐⭐ 중급
**학습 시간**: 약 25시간
**선수과목**: Week 1 완료

> **이 주차가 중요한 이유**: C++의 가장 큰 실수인 **메모리 누수를 영구히 해결하는 방법**을 배웁니다!

---

## 📖 학습 안내

- 💻 **모든 코드는 실행 가능** (복사해서 바로 테스트)
- 🤔 **메모리 구조를 그려가며 이해**
- ⚠️ **혼내는 실수 9가지**를 통해 배운다
- 🧪 **메모리 누수를 직접 감지**하는 도구 (valgrind)
- ✅ **3개의 마일스톤 프로젝트**

---

## 📚 **Day 1: 메모리 문제의 3가지 종류**

### 1.1 문제 1: 메모리 누수 (Memory Leak)

**메모리 누수란?**
할당한 메모리를 해제하지 않아서 남은 메모리가 쓰레기처럼 쌓이는 것

**시뮬레이션:**

```
프로그램 시작:
메모리: [  ][  ][  ][  ][  ]  남은 메모리: 100MB

new int[10] 실행:
메모리: [X ][  ][  ][  ][  ]  남은 메모리: 80MB

delete 해제:
메모리: [  ][  ][  ][  ][  ]  남은 메모리: 100MB ✓

delete 안 함 (누수!):
메모리: [X ][  ][  ][  ][  ]  남은 메모리: 80MB ← 영구 손실!
```

### 1.2 실제 메모리 누수 코드

```cpp
❌ 메모리 누수 발생!
#include <iostream>
using namespace std;

void allocateMemory() {
    int* arr = new int[1000];
    arr[0] = 42;
    cout << "배열 할당 완료" << endl;
    // return: arr을 delete하지 않았음!
    // → 1000개 int (4KB 정도)가 누수!
}

int main() {
    for (int i = 0; i < 10000; i++) {
        allocateMemory();  // 10,000번 호출 = 40MB 누수!
    }

    cout << "프로그램 종료" << endl;
    // 40MB의 메모리가 운영체제에 반환되지 않음!

    return 0;
}
```

**문제점:**
- 프로그램이 끝날 때까지 40MB가 낭비됨
- 오래 실행되는 서버 프로그램이면 시스템이 느려짐
- 심하면 메모리 부족으로 프로그램 크래시

### 1.3 메모리 누수 고치기 (수동)

```cpp
✅ 올바른 코드 (하지만 번거로움)
void allocateMemory() {
    int* arr = new int[1000];
    arr[0] = 42;
    cout << "배열 할당 완료" << endl;
    delete[] arr;  // ← 반드시 이 줄을!
    arr = nullptr; // ← 안전 장치
}
```

> **문제**: 매번 `delete`를 기억해야 함. 실수하기 쉬움!

---

### ⚠️ **흔한 실수 #1: delete 와 delete[] 혼동**

```cpp
❌ 틀린 코드:
int* arr = new int[10];  // 배열 할당
delete arr;              // ← 개별 delete! 오류!

int* num = new int;      // 개별 할당
delete[] num;            // ← 배열 delete! 오류!
```

**규칙:**
- `new int` → `delete`
- `new int[N]` → `delete[]`

```cpp
✅ 올바른 코드:
int* arr = new int[10];
delete[] arr;

int* num = new int;
delete num;
```

---

### 1.4 문제 2: 댕글링 포인터 (Dangling Pointer)

**댕글링 포인터란?**
해제된 메모리를 가리키는 포인터. 접근하면 크래시!

```cpp
❌ 크래시 발생!
int main() {
    int* ptr = new int(42);
    cout << *ptr << endl;  // 42 출력 ✓

    delete ptr;  // 메모리 해제

    cout << *ptr << endl;  // 💥 크래시! 해제된 메모리 접근
    return 0;
}
```

### 1.5 댕글링 포인터 고치기

```cpp
✅ 안전한 코드:
int* ptr = new int(42);
cout << *ptr << endl;

delete ptr;
ptr = nullptr;  // ← 중요! nullptr로 설정

if (ptr != nullptr) {  // 안전 확인
    cout << *ptr << endl;
} else {
    cout << "포인터가 유효하지 않음" << endl;
}
```

---

### ⚠️ **흔한 실수 #2: nullptr 확인 안 함**

```cpp
❌ 위험한 코드:
int* ptr = nullptr;
*ptr = 10;  // 💥 크래시! nullptr에 접근
```

```cpp
✅ 안전한 코드:
int* ptr = nullptr;
if (ptr != nullptr) {
    *ptr = 10;
} else {
    cout << "포인터가 null입니다" << endl;
}
```

---

### 1.6 문제 3: Double Delete

**Double Delete란?**
같은 메모리를 두 번 해제하려고 시도

```cpp
❌ 크래시 발생!
int main() {
    int* ptr = new int(42);

    delete ptr;  // 첫 번째 해제 ✓
    delete ptr;  // 💥 두 번째 해제! 크래시!

    return 0;
}
```

**왜 위험한가?**
- 이미 해제된 메모리를 다시 해제하려고 함
- 운영체제의 메모리 관리자가 혼동함
- 프로그램 크래시!

### 1.7 Double Delete 고치기

```cpp
✅ 안전한 코드:
int* ptr = new int(42);

delete ptr;
ptr = nullptr;  // ← 중요!

delete ptr;     // nullptr이므로 안전함 (아무것도 안 함)
```

> **C++ 규칙**: `delete nullptr;` 은 안전합니다 (아무것도 하지 않음)

---

### ✅ **자기점검 Quiz 2.1**

다음 코드의 문제점을 찾으세요:

```cpp
void process() {
    int* data = new int[100];
    data[0] = 42;
    cout << data[0] << endl;
    // 함수 종료
}

int main() {
    process();
    process();
    process();
    return 0;
}
```

<details>
<summary>정답 보기</summary>

**문제**: `delete[]`를 하지 않았음!

**누수 발생**:
- 매 호출마다 400바이트 누수
- 3번 호출 = 1200바이트 누수

**해결책**:
```cpp
void process() {
    int* data = new int[100];
    data[0] = 42;
    cout << data[0] << endl;
    delete[] data;  // ← 추가!
    data = nullptr;
}
```
</details>

---

## 📚 **Day 2: 메모리 누수 감지하기 (Valgrind)**

### 2.1 Valgrind 설치

```bash
# Ubuntu/Debian
sudo apt install valgrind

# macOS
brew install valgrind

# 설치 확인
valgrind --version
```

### 2.2 누수가 있는 코드로 테스트

```cpp
// leak.cpp
#include <iostream>
using namespace std;

int main() {
    int* ptr = new int(42);
    // delete 하지 않음 (의도적으로)
    return 0;
}
```

```bash
# 컴파일
g++ -g -o leak leak.cpp

# Valgrind로 검사
valgrind --leak-check=full ./leak
```

**출력 예:**
```
==12345== Memcheck, a memory error detector
==12345== HEAP SUMMARY:
==12345==     in use at exit: 4 bytes in 1 blocks
==12345==   Loss record 1 of 1:
==12345==    DEFINITELY LOST: 4 bytes in 1 block
==12345== ERROR SUMMARY: 1 errors from 1 context
```

> **해석**: 4바이트 (int 하나)가 영구 손실됨!

### 2.3 수정 후 테스트

```cpp
// leak_fixed.cpp
#include <iostream>
using namespace std;

int main() {
    int* ptr = new int(42);
    delete ptr;  // ← 추가!
    ptr = nullptr;
    return 0;
}
```

```bash
valgrind --leak-check=full ./leak_fixed
```

**출력 예:**
```
==12346== HEAP SUMMARY:
==12346==     in use at exit: 0 bytes in 0 blocks
==12346== Loss record 1 of 1:
==12346==    DEFINITELY LOST: 0 bytes in 0 blocks
==12346== ERROR SUMMARY: 0 errors from 1 context
```

> **완벽함!** 메모리 누수 없음!

---

## 📚 **Day 3: RAII 패턴 (Resource Acquisition Is Initialization)**

### 3.1 RAII의 아이디어

**RAII = "객체를 만들면 리소스 획득, 객체가 사라지면 리소스 해제"**

```
구식 방식:
리소스 획득 → 사용 → 리소스 해제
                      ↑
                   실수하기 쉬움!

RAII 방식:
생성자에서 획득 → 소멸자에서 자동 해제
                 ↑
              실수 불가능!
```

### 3.2 RAII 패턴의 간단한 예

```cpp
#include <iostream>
using namespace std;

class FileWrapper {
private:
    FILE* file;

public:
    // 생성자: 파일 열기 (리소스 획득)
    FileWrapper(const string& filename) {
        file = fopen(filename.c_str(), "w");
        if (!file) {
            throw runtime_error("파일을 열 수 없습니다");
        }
        cout << "파일 열기: " << filename << endl;
    }

    // 소멸자: 파일 닫기 (리소스 해제) ← 자동 호출!
    ~FileWrapper() {
        if (file) {
            fclose(file);
            cout << "파일 닫음" << endl;
        }
    }

    // 복사 방지 (이 주차에서는 깊은 복사가 복잡함)
    FileWrapper(const FileWrapper&) = delete;
    FileWrapper& operator=(const FileWrapper&) = delete;

    void write(const string& text) {
        fprintf(file, "%s\n", text.c_str());
    }
};

int main() {
    {
        FileWrapper file("test.txt");
        file.write("Hello");
        file.write("World");
    }  // ← 여기서 자동으로 ~FileWrapper() 호출됨 (파일 닫힘)

    cout << "프로그램 종료" << endl;
    return 0;
}
```

**실행 결과:**
```
파일 열기: test.txt
파일 닫음
프로그램 종료
```

> **장점**: `delete`를 입력하지 않았는데도 자동으로 파일이 닫힙니다!

---

### ⚠️ **흔한 실수 #3: 복사 허용하기**

```cpp
❌ 위험한 코드:
class FileWrapper {
    // 복사 허용 (기본값)
};

int main() {
    FileWrapper file1("test1.txt");
    FileWrapper file2 = file1;  // 복사!

    // file1과 file2가 같은 파일 포인터를 가짐
    // 소멸자가 두 번 호출되어 double delete!

    return 0;
}
```

```cpp
✅ 안전한 코드:
class FileWrapper {
    FileWrapper(const FileWrapper&) = delete;  // 복사 금지!
};
```

---

## 📚 **Day 4: unique_ptr (유일 소유권)**

### 4.1 unique_ptr란?

**unique_ptr = 포인터를 자동으로 관리해주는 똑똑한 포인터**

```
일반 포인터:
int* ptr = new int(42);
// delete 기억해야 함
delete ptr;

unique_ptr:
unique_ptr<int> ptr = make_unique<int>(42);
// 범위를 벗어나면 자동으로 delete!
```

### 4.2 unique_ptr 사용법

```cpp
#include <iostream>
#include <memory>
using namespace std;

int main() {
    // ① unique_ptr 생성
    unique_ptr<int> ptr = make_unique<int>(42);

    // ② 일반 포인터처럼 사용
    cout << *ptr << endl;  // 42

    // ③ 범위를 벗어나면 자동으로 delete
}  // ← 여기서 자동으로 delete됨. 메모리 누수 없음!

// 이전 방식과 비교:
// ❌ 수동:
// int* ptr = new int(42);
// ... 코드 ...
// delete ptr;  // 실수하기 쉬움!

// ✅ unique_ptr:
// unique_ptr<int> ptr = make_unique<int>(42);
// ... 코드 ...
// // 자동으로 해제됨!
```

### 4.3 배열에 unique_ptr 사용

```cpp
#include <iostream>
#include <memory>
using namespace std;

int main() {
    // 배열 할당
    unique_ptr<int[]> arr = make_unique<int[]>(10);

    // 일반 배열처럼 사용
    for (int i = 0; i < 10; i++) {
        arr[i] = i * 2;
    }

    // 출력
    for (int i = 0; i < 10; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    // 범위를 벗어나면 자동으로 delete[]됨!
}
```

**실행 결과:**
```
0 2 4 6 8 10 12 14 16 18
```

### 4.4 unique_ptr의 이동 (Move Semantics)

**unique_ptr은 이동만 가능합니다 (복사는 불가):**

```cpp
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

int main() {
    unique_ptr<int> ptr1 = make_unique<int>(42);

    // ❌ 복사 불가능 (컴파일 오류)
    // unique_ptr<int> ptr2 = ptr1;

    // ✅ 이동만 가능
    unique_ptr<int> ptr2 = move(ptr1);

    // 이제 ptr2가 소유권을 가짐
    cout << *ptr2 << endl;  // 42

    // ptr1은 더 이상 유효하지 않음
    if (ptr1) {
        cout << *ptr1 << endl;  // 실행 안 됨
    }

    return 0;
}
```

### 4.5 unique_ptr을 벡터에 저장

```cpp
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Student {
public:
    string name;
    int score;

    Student(string n, int s) : name(n), score(s) {
        cout << name << " 객체 생성" << endl;
    }

    ~Student() {
        cout << name << " 객체 소멸" << endl;
    }
};

int main() {
    // unique_ptr을 벡터에 저장
    vector<unique_ptr<Student>> students;

    // 학생 추가
    students.push_back(make_unique<Student>("Kim", 95));
    students.push_back(make_unique<Student>("Lee", 87));
    students.push_back(make_unique<Student>("Park", 92));

    // 학생 정보 출력
    for (auto& student : students) {
        cout << student->name << ": " << student->score << "점" << endl;
    }

    return 0;
}  // ← 벡터가 소멸하면서 모든 학생 객체도 자동 소멸
```

**실행 결과:**
```
Kim 객체 생성
Lee 객체 생성
Park 객체 생성
Kim: 95점
Lee: 87점
Park: 92점
Kim 객체 소멸
Lee 객체 소멸
Park 객체 소멸
```

---

### ⚠️ **흔한 실수 #4: get() 으로 꺼낸 포인터 delete하기**

```cpp
❌ 크래시 발생!
unique_ptr<int> ptr = make_unique<int>(42);
int* raw = ptr.get();  // 원래 포인터 꺼냄

delete raw;  // ← 위험! 소유권이 unique_ptr에 있음

return 0;
// ← 범위를 벗어나면서 unique_ptr이 다시 delete 시도
// → double delete 크래시!
```

```cpp
✅ 안전한 코드:
unique_ptr<int> ptr = make_unique<int>(42);
int* raw = ptr.get();  // 조회만 함

// raw를 delete하지 말 것!
// unique_ptr이 관리하도록 놔두기

return 0;
// ← 여기서 안전하게 delete됨
```

---

### ✅ **자기점검 Quiz 2.2**

다음 코드에서 메모리 누수가 발생하는가?

```cpp
int main() {
    unique_ptr<int> ptr = make_unique<int>(42);
    cout << *ptr << endl;
    return 0;
}
```

<details>
<summary>정답 보기</summary>

**아니요, 누수가 없습니다!** ✅

설명:
- `make_unique`로 할당한 메모리는 unique_ptr이 관리
- 범위를 벗어나면 자동으로 delete됨
- Valgrind로 확인하면: `DEFINITELY LOST: 0 bytes`
</details>

---

## 📚 **Day 5: shared_ptr (공유 소유권)**

### 5.1 shared_ptr란?

**shared_ptr = 여러 포인터가 같은 메모리를 공유**

```
unique_ptr: 한 명만 소유 (배타적)
┌─────────────────┐
│ ptr1 ──→ 메모리 │  ptr1이 유일한 소유자
└─────────────────┘

shared_ptr: 여럿이 공유
┌─────────────────┐
│ ptr1 ──→        │
│ ptr2 ──→ 메모리 │  모두 공동 소유자
│ ptr3 ──→        │
└─────────────────┘
```

### 5.2 shared_ptr 사용법

```cpp
#include <iostream>
#include <memory>
using namespace std;

int main() {
    // 공유 포인터 생성
    shared_ptr<int> ptr1 = make_shared<int>(42);

    cout << "값: " << *ptr1 << endl;
    cout << "참조 개수: " << ptr1.use_count() << endl;  // 1

    // 다른 포인터에 할당 (공유!)
    shared_ptr<int> ptr2 = ptr1;
    cout << "값: " << *ptr2 << endl;
    cout << "참조 개수: " << ptr1.use_count() << endl;  // 2

    // 또 다른 포인터에 할당
    shared_ptr<int> ptr3 = ptr1;
    cout << "참조 개수: " << ptr1.use_count() << endl;  // 3

    // ptr1이 범위를 벗어남
    {
        shared_ptr<int> ptr4 = ptr1;
        cout << "내부 범위: " << ptr1.use_count() << endl;  // 4
    }  // ← ptr4 소멸
    cout << "외부 범위: " << ptr1.use_count() << endl;  // 3

    return 0;
}  // ← ptr1, ptr2, ptr3 모두 소멸
    // 마지막 포인터가 사라질 때 메모리가 delete됨
```

**실행 결과:**
```
값: 42
참조 개수: 1
값: 42
참조 개수: 2
참조 개수: 3
내부 범위: 4
외부 범위: 3
```

### 5.3 shared_ptr 실전 예제: 그래프 구조

```cpp
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Node {
public:
    int id;
    vector<shared_ptr<Node>> neighbors;  // 다른 노드들을 가리킴

    Node(int id) : id(id) {
        cout << "Node " << id << " 생성" << endl;
    }

    ~Node() {
        cout << "Node " << id << " 소멸" << endl;
    }

    void connect(shared_ptr<Node> other) {
        neighbors.push_back(other);
    }
};

int main() {
    // 노드 생성
    shared_ptr<Node> node1 = make_shared<Node>(1);
    shared_ptr<Node> node2 = make_shared<Node>(2);
    shared_ptr<Node> node3 = make_shared<Node>(3);

    // 연결
    node1->connect(node2);
    node1->connect(node3);
    node2->connect(node3);

    cout << "node1 참조: " << node1.use_count() << endl;  // 1
    cout << "node2 참조: " << node2.use_count() << endl;  // 2 (node1, neighbors)
    cout << "node3 참조: " << node3.use_count() << endl;  // 3

    return 0;
}  // ← 모든 노드가 안전하게 소멸
```

**실행 결과:**
```
Node 1 생성
Node 2 생성
Node 3 생성
node1 참조: 1
node2 참조: 2
node3 참조: 3
Node 3 소멸
Node 2 소멸
Node 1 소멸
```

---

### ⚠️ **흔한 실수 #5: weak_ptr 없이 순환 참조**

```cpp
❌ 메모리 누수 (순환 참조):
class Node {
public:
    shared_ptr<Node> next;  // 다음 노드
    shared_ptr<Node> prev;  // 이전 노드
};

int main() {
    auto node1 = make_shared<Node>();
    auto node2 = make_shared<Node>();

    node1->next = node2;
    node2->prev = node1;  // 순환 참조!

    return 0;
    // node1 → node2 → node1로 순환
    // 참조 개수가 절대 0이 되지 않음!
    // 메모리 누수 발생!
}
```

```cpp
✅ 해결책: weak_ptr 사용 (Week 2 고급)
class Node {
public:
    shared_ptr<Node> next;
    weak_ptr<Node> prev;  // weak_ptr 사용!
};
```

---

### ✅ **자기점검 Quiz 2.3**

다음 코드에서 메모리는 언제 해제될까요?

```cpp
shared_ptr<int> ptr1 = make_shared<int>(42);
{
    shared_ptr<int> ptr2 = ptr1;
    cout << ptr1.use_count() << endl;
}  // ← 여기서 해제되는가?
cout << ptr1 << endl;
return 0;
```

<details>
<summary>정답 보기</summary>

**아니요, 여기서 해제되지 않습니다!**

설명:
- `{}`내부: ptr1, ptr2 모두 존재 → use_count = 2
- `}`벗어남: ptr2 소멸 → use_count = 1
- ptr1은 여전히 존재하므로 메모리 유지
- `return 0`에서 ptr1 소멸 → 메모리 해제

**출력:**
```
2       // 블록 내부에서
42      // ptr1은 여전히 유효
```
</details>

---

## 📚 **Day 6: Move Semantics (이동 의미론)**

### 6.1 복사 vs 이동

```
복사 (Copy):
원본: [1, 2, 3, 4, 5]
복사본: [1, 2, 3, 4, 5]
메모리 낭비! (5배)

이동 (Move):
원본: [1, 2, 3, 4, 5] → [비어있음]
새로운 위치: [1, 2, 3, 4, 5]
효율적! (메모리 재사용)
```

### 6.2 이동 생성자 구현

```cpp
#include <iostream>
#include <cstring>
using namespace std;

class Vector {
private:
    int* data;
    int size;

public:
    // 일반 생성자
    Vector(int s) : size(s) {
        data = new int[size];
        cout << "일반 생성자 호출 (크기: " << size << ")" << endl;
    }

    // 복사 생성자 (느림)
    Vector(const Vector& other) : size(other.size) {
        data = new int[size];
        memcpy(data, other.data, sizeof(int) * size);
        cout << "복사 생성자 호출" << endl;
    }

    // 이동 생성자 (빠름!) ← 새로운 개념
    Vector(Vector&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
        cout << "이동 생성자 호출" << endl;
    }

    // 이동 할당 연산자
    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        cout << "이동 할당 연산자 호출" << endl;
        return *this;
    }

    ~Vector() {
        delete[] data;
    }

    int getSize() { return size; }
};

int main() {
    // 1️⃣ 일반 생성
    Vector v1(5);

    // 2️⃣ 이동으로 할당
    v1 = Vector(10);  // ← 임시 객체를 이동!

    cout << "v1 크기: " << v1.getSize() << endl;

    return 0;
}
```

**실행 결과:**
```
일반 생성자 호출 (크기: 5)
일반 생성자 호출 (크기: 10)
이동 할당 연산자 호출
v1 크기: 10
```

### 6.3 move() 함수 사용

```cpp
#include <iostream>
#include <memory>
#include <utility>  // move 포함
using namespace std;

class Image {
public:
    int* pixels;
    int size;

    Image(int s) : size(s) {
        pixels = new int[size];
        cout << "Image 생성" << endl;
    }

    // 이동 생성자
    Image(Image&& other) noexcept {
        pixels = other.pixels;
        size = other.size;
        other.pixels = nullptr;
        cout << "Image 이동" << endl;
    }

    ~Image() {
        delete[] pixels;
    }
};

// 큰 이미지를 반환하는 함수
Image createLargeImage() {
    Image img(1000000);  // 1M 픽셀
    return img;  // ← 여기서 이동!
}

int main() {
    // 이동이 발생!
    Image img = createLargeImage();

    return 0;
}
```

**메모리 효율:**
```
복사 방식:
1. createLargeImage()에서 생성: 4MB
2. main에 반환: 4MB 복사
3. 임시 객체 소멸: 4MB 해제
→ 최대 8MB 사용

이동 방식:
1. createLargeImage()에서 생성: 4MB
2. main에서 이동: 포인터만 변경
3. 임시 객체 소멸: nullptr 소멸
→ 최대 4MB 사용 (50% 절감!)
```

---

### ⚠️ **흔한 실수 #6: 이동 후에 사용**

```cpp
❌ 위험한 코드:
Vector v1(5);
Vector v2 = move(v1);  // v1을 이동시킴

cout << v1.getSize() << endl;  // ← 위험! v1은 이제 비어있음
```

```cpp
✅ 안전한 코드:
Vector v1(5);
Vector v2 = move(v1);

// v1을 더 이상 사용하지 않기
// 혹은 v1을 다시 초기화하기
v1 = Vector(10);
```

---

### ✅ **자기점검 Quiz 2.4**

다음 중 어떤 것이 이동할까요?

```cpp
Vector v1(5);
Vector v2(10);

v1 = v2;  // ① 복사인가 이동인가?
v1 = move(v2);  // ② 복사인가 이동인가?
v1 = Vector(15);  // ③ 복사인가 이동인가?
```

<details>
<summary>정답 보기</summary>

① **복사**: `v2`는 일반 참조이므로 복사 생성자 호출
② **이동**: `move(v2)`는 우측값이므로 이동 생성자 호출
③ **이동**: `Vector(15)`는 임시 객체이므로 이동 생성자 호출

**규칙**: 우측값(rvalue)이면 이동, 좌측값(lvalue)이면 복사
</details>

---

## 📚 **Day 7: 최종 프로젝트 - SafeSorter**

### 7.1 프로젝트 요구사항

완전히 메모리 안전한 정렬기를 만들기:

1. ✅ `unique_ptr`로 동적 배열 관리
2. ✅ 이동 생성자 구현
3. ✅ 복사 방지 (메모리 안전)
4. ✅ 정렬 알고리즘 (QuickSort)
5. ✅ Valgrind로 검증 (메모리 누수 없음)

### 7.2 완전한 SafeSorter 구현

```cpp
#include <iostream>
#include <memory>
#include <chrono>
using namespace std;

class SafeSorter {
private:
    unique_ptr<int[]> arr;
    int size;

    // QuickSort 보조 함수
    void quickSortHelper(int left, int right) {
        if (left >= right) return;

        int pivot = arr[right];
        int i = left - 1;

        for (int j = left; j < right; j++) {
            if (arr[j] < pivot) {
                swap(arr[++i], arr[j]);
            }
        }

        swap(arr[i+1], arr[right]);
        quickSortHelper(left, i);
        quickSortHelper(i + 2, right);
    }

public:
    // 생성자: unique_ptr로 배열 할당
    SafeSorter(int s) : size(s) {
        arr = make_unique<int[]>(size);
        cout << "[생성] " << size << "개 원소 배열 생성" << endl;
    }

    // 이동 생성자 (큰 배열을 효율적으로 이동)
    SafeSorter(SafeSorter&& other) noexcept
        : arr(move(other.arr)), size(other.size) {
        other.size = 0;
        cout << "[이동] 배열 이동 완료" << endl;
    }

    // 복사 방지 (메모리 안전)
    SafeSorter(const SafeSorter&) = delete;
    SafeSorter& operator=(const SafeSorter&) = delete;

    // 소멸자 (unique_ptr이 자동으로 처리)
    ~SafeSorter() {
        cout << "[소멸] 배열 메모리 해제" << endl;
    }

    // 무작위 데이터 채우기
    void fillRandom() {
        for (int i = 0; i < size; i++) {
            arr[i] = rand() % 1000;
        }
    }

    // QuickSort 실행
    void quickSort() {
        if (size > 1) {
            quickSortHelper(0, size - 1);
        }
    }

    // 정렬 여부 확인
    bool isSorted() const {
        for (int i = 0; i < size - 1; i++) {
            if (arr[i] > arr[i+1]) {
                return false;
            }
        }
        return true;
    }

    // 앞의 n개 원소 출력
    void printFirst(int count) const {
        cout << "앞의 " << min(count, size) << "개: ";
        for (int i = 0; i < min(count, size); i++) {
            cout << arr[i] << " ";
        }
        cout << endl;
    }

    int getSize() const { return size; }
};

// SafeSorter를 생성하는 함수
SafeSorter createSorter(int size) {
    SafeSorter sorter(size);
    sorter.fillRandom();
    cout << "[함수] SafeSorter 생성 후 반환" << endl;
    return sorter;  // ← 이동이 발생!
}

int main() {
    cout << "=== Week 2 최종 프로젝트: SafeSorter ===" << endl << endl;

    // 1️⃣ 기본 사용
    {
        cout << "1️⃣ 기본 정렬:" << endl;
        SafeSorter sorter(100);
        sorter.fillRandom();

        auto start = chrono::high_resolution_clock::now();
        sorter.quickSort();
        auto end = chrono::high_resolution_clock::now();

        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

        cout << "정렬 시간: " << duration.count() << "μs" << endl;
        cout << "정렬 확인: " << (sorter.isSorted() ? "✓ 통과" : "✗ 실패") << endl;
        sorter.printFirst(10);
        cout << endl;
    }

    // 2️⃣ 이동 사용
    {
        cout << "2️⃣ 이동을 통한 생성:" << endl;
        SafeSorter sorter = createSorter(100);  // ← 이동!
        sorter.quickSort();
        cout << "정렬 확인: " << (sorter.isSorted() ? "✓ 통과" : "✗ 실패") << endl;
        cout << endl;
    }

    // 3️⃣ 큰 배열로 성능 테스트
    {
        cout << "3️⃣ 큰 배열 정렬:" << endl;
        SafeSorter sorter(100000);
        sorter.fillRandom();

        auto start = chrono::high_resolution_clock::now();
        sorter.quickSort();
        auto end = chrono::high_resolution_clock::now();

        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

        cout << "정렬 시간: " << duration.count() << "ms" << endl;
        cout << "정렬 확인: " << (sorter.isSorted() ? "✓ 통과" : "✗ 실패") << endl;
    }

    cout << endl << "=== 프로그램 종료 ===" << endl;
    cout << "Valgrind로 검증하세요!" << endl;

    return 0;
}
```

### 7.3 빌드 및 실행

```bash
# 컴파일
g++ -std=c++17 -g -o safe_sorter safe_sorter.cpp

# 실행
./safe_sorter

# Valgrind로 메모리 누수 검증
valgrind --leak-check=full ./safe_sorter
```

**예상 출력:**
```
=== Week 2 최종 프로젝트: SafeSorter ===

1️⃣ 기본 정렬:
[생성] 100개 원소 배열 생성
정렬 시간: 45μs
정렬 확인: ✓ 통과
앞의 10개: 0 1 2 3 4 5 6 7 8 9

2️⃣ 이동을 통한 생성:
[생성] 100개 원소 배열 생성
[함수] SafeSorter 생성 후 반환
[이동] 배열 이동 완료
정렬 확인: ✓ 통과

3️⃣ 큰 배열 정렬:
[생성] 100000개 원소 배열 생성
정렬 시간: 12ms
정렬 확인: ✓ 통과

=== 프로그램 종료 ===
Valgrind로 검증하세요!
[소멸] 배열 메모리 해제
[소멸] 배열 메모리 해제
[소멸] 배열 메모리 해제
```

**Valgrind 결과:**
```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 0 bytes in 0 blocks
==12345== Loss record 1 of 1:
==12345==    DEFINITELY LOST: 0 bytes in 0 blocks
==12345== ERROR SUMMARY: 0 errors from 1 context
```

> **완벽!** 메모리 누수 없음! ✅

---

## ✅ **Week 2 최종 자기점검**

다음을 모두 할 수 있으면 Week 2 완료입니다!

- [ ] 메모리 누수의 원인을 설명할 수 있다
- [ ] 댕글링 포인터와 Double Delete를 구분할 수 있다
- [ ] RAII 패턴의 장점을 이해한다
- [ ] unique_ptr과 shared_ptr을 구분해서 사용할 수 있다
- [ ] 이동 생성자와 이동 할당을 구현할 수 있다
- [ ] Valgrind로 메모리 누수를 감지할 수 있다
- [ ] SafeSorter를 완전히 메모리 안전하게 구현했다

---

## 📚 **추가 학습 (심화)**

### 더 배우고 싶다면?

- **weak_ptr**: shared_ptr의 순환 참조 해결
- **커스텀 Deleter**: unique_ptr에 특수 해제 함수 지정
- **make_shared vs shared_ptr**: 성능 차이
- **정적 할당 vs 동적 할당**: 언제 뭘 쓸까?
- **메모리 풀**: 빈번한 new/delete 최적화

---

## 🔗 **다음: Week 3 고급 기능 (템플릿, 람다, STL)**

Week 2에서 배운 메모리 관리를 토대로, **일반적인 코드를 작성하는 방법**을 배웁니다!

---

**마지막 팁**: Valgrind로 검증하는 습관을 들이면, 메모리 문제는 절대 문제가 되지 않습니다! 🛡️

