# 🎓 Week 5: MLIR 기초 & C++ API (온라인 혼자 학습용)

**목표**: MLIR 아키텍처 완전 이해, Context & Builder 마스터, 첫 IR 생성

**난이도**: ⭐⭐⭐⭐ 도전 (심화)
**학습 시간**: 약 30시간
**선수과목**: Elementary Level (Week 1-4) 완료

> **이 주차가 중요한 이유**: **MLIR은 차세대 컴파일러 기술**입니다. 이 기초를 이해하면 컴파일러 최적화의 세계가 열립니다!

---

## 📖 학습 안내

- 🏗️ **MLIR 아키텍처**: 다중 추상화 수준의 IR
- 📦 **Operation, Region, Block**: MLIR의 기본 구조
- 🔧 **Context & Builder**: IR을 프로그래머블하게 생성
- 💻 **실제 코드**: LLVM 프레임워크를 사용한 구현
- ✅ **완전한 예제**: 복사해서 바로 컴파일 가능

---

## 📚 **Day 1: MLIR이란 무엇인가?**

### 1.1 컴파일러의 문제점

```
전통적인 LLVM 아키텍처:
┌──────────────┐
│ C/C++/Rust   │  ← 프로그래밍 언어
├──────────────┤
│  LLVM IR     │  ← 단일 추상화 수준
├──────────────┤
│  기계코드    │  ← CPU 명령어
└──────────────┘

문제점:
- 고수준 최적화 불가능 (구조 정보 손실)
- AI/HPC 도메인 최적화 어려움
- 중간 표현 확장 불가능
```

### 1.2 MLIR의 해결책

```
MLIR 아키텍처:
┌──────────────────────────┐
│  High-Level (Python IR)  │  ← 파이썬 코드
├──────────────────────────┤
│  Tensor Operations       │  ← 고수준 연산
├──────────────────────────┤
│  Vector Operations       │  ← 벡터화
├──────────────────────────┤
│  Affine (루프 변환)      │  ← 루프 최적화
├──────────────────────────┤
│  Memory (메모리)         │  ← 메모리 레이아웃
├──────────────────────────┤
│  GPU Dialect             │  ← GPU 커널
├──────────────────────────┤
│  LLVM IR                 │  ← 기계 수준
├──────────────────────────┤
│  기계코드                │  ← CPU/GPU 명령어
└──────────────────────────┘

장점:
✅ 다중 추상화 수준에서 최적화
✅ 도메인 특화 연산 정의 가능
✅ 점진적 변환 (Lowering)
✅ AI/HPC 친화적
```

### 1.3 MLIR의 핵심 개념

```
SSA (Static Single Assignment):
├─ 각 값은 정확히 한 번 할당됨
├─ 정보 흐름이 명확함
└─ 최적화가 쉬움

예:
%0 = arith.addi %arg0, %arg1 : i32
%1 = arith.muli %0, %arg2 : i32
return %1

%1은 한 번만 정의됨 (명확함!)
```

---

### ⚠️ **흔한 혼동 #1: MLIR vs LLVM**

```
LLVM:
- 저수준 기계 친화적 IR
- C/C++ 최적화에 강함
- 고급 변환 구현 어려움

MLIR:
- 다중 추상화 수준
- AI/HPC에 강함
- 점진적 변환 가능
- LLVM으로 lowering 가능 ← LLVM과 호환됨!
```

---

## 📚 **Day 2: MLIR의 구조**

### 2.1 계층 구조: Module → Function → Operation

```
Module (프로그램 전체)
│
├─ FuncOp (함수 1)
│  │
│  └─ Region (함수 본체)
│     │
│     └─ Block (기본 블록)
│        ├─ Operation (연산 1)
│        ├─ Operation (연산 2)
│        └─ Operation (연산 3)
│
└─ FuncOp (함수 2)
   └─ ...
```

### 2.2 Operation (연산의 단위)

```
Operation의 구조:
%result = op.name %operand1, %operand2 : type -> result_type

예:
%0 = arith.addi %arg0, %arg1 : i32

파싱:
- %0: 결과를 %0에 저장
- arith.addi: 정수 덧셈
- %arg0, %arg1: 입력값
- i32: 입력 타입
```

### 2.3 Type 시스템

```cpp
// Integer Types
i1, i8, i16, i32, i64      // 고정 너비 정수

// Float Types
f16, bf16, f32, f64        // IEEE 754

// Tensor Types
tensor<10x20xf32>          // 정적 크기
tensor<?x20xf32>           // 동적 크기 (1차원)
tensor<*xf32>              // 임의 차원

// Memref Types (메모리 참조)
memref<10x20xf32>          // 메모리 배열

// Function Types
(i32, f32) -> i64          // 함수 시그니처
```

---

## 📚 **Day 3: MLIRContext (전역 환경)**

### 3.1 MLIRContext란?

```cpp
#include "mlir/IR/MLIRContext.h"

using namespace mlir;

int main() {
    // MLIRContext: 모든 타입, 속성, 메타데이터를 관리
    MLIRContext ctx;

    // Dialect 등록 (필수!)
    ctx.getOrLoadDialect<func::FuncDialect>();
    ctx.getOrLoadDialect<arith::ArithDialect>();

    cout << "Context 초기화 완료" << endl;

    return 0;
}
```

**Context의 역할:**
- 타입 (Type) 관리
- 속성 (Attribute) 관리
- Dialect 등록 및 관리
- 메모리 할당 (재사용)
- 전역 설정

### 3.2 Dialect 등록

```cpp
#include "mlir/IR/MLIRContext.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include <iostream>

using namespace mlir;

int main() {
    MLIRContext ctx;

    // 1️⃣ Func Dialect (함수)
    ctx.getOrLoadDialect<func::FuncDialect>();
    cout << "func Dialect 로드됨" << endl;

    // 2️⃣ Arith Dialect (산술)
    ctx.getOrLoadDialect<arith::ArithDialect>();
    cout << "arith Dialect 로드됨" << endl;

    // 3️⃣ 다른 Dialect들...
    // ctx.getOrLoadDialect<memref::MemRefDialect>();
    // ctx.getOrLoadDialect<scf::SCFDialect>();

    cout << "모든 필요한 Dialect 로드됨" << endl;

    return 0;
}
```

---

## 📚 **Day 4: OpBuilder (IR 생성자)**

### 4.1 OpBuilder란?

**OpBuilder = 연산들을 쉽게 생성해주는 도구**

```cpp
#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"

using namespace mlir;

int main() {
    MLIRContext ctx;

    // 1️⃣ Builder 생성
    OpBuilder builder(&ctx);

    // 2️⃣ 위치 설정 (어디에 연산을 추가할지)
    // builder.setInsertionPoint(...);

    // 3️⃣ 연산 생성 (자동으로 올바른 위치에 추가됨)
    // auto op = builder.create<arith::ConstantOp>(...);

    return 0;
}
```

### 4.2 Builder의 주요 메서드

```cpp
OpBuilder builder(&ctx);

// 위치 설정
builder.setInsertionPointToStart(block);     // 블록 시작
builder.setInsertionPointToEnd(block);       // 블록 끝
builder.setInsertionPoint(op);               // 특정 연산 앞

// 타입 생성
auto i32 = builder.getI32Type();             // i32 타입
auto f32 = builder.getF32Type();             // f32 타입
auto tensorType = RankedTensorType::get({10, 20}, f32);  // tensor

// 상수 생성
auto zero = builder.create<arith::ConstantOp>(loc, 0);
```

---

## 📚 **Day 5-6: Hello World - 첫 IR 생성**

### 5.1 목표

**다음과 같은 MLIR 코드를 C++로 생성:**

```mlir
module {
  func.func @add(%arg0: i32, %arg1: i32) -> i32 {
    %0 = arith.addi %arg0, %arg1 : i32
    func.return %0 : i32
  }
}
```

### 5.2 CMakeLists.txt (빌드 설정)

```cmake
cmake_minimum_required(VERSION 3.13.4)
project(mlir-hello)

find_package(MLIR REQUIRED CONFIG)

add_executable(hello_world main.cpp)

target_include_directories(hello_world PRIVATE ${MLIR_INCLUDE_DIRS})
target_link_libraries(hello_world PRIVATE MLIRCore MLIRIR MLIRPass)
```

### 5.3 main.cpp (완전한 코드)

```cpp
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Module.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/Location.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

using namespace mlir;
using namespace mlir::func;

int main() {
    std::cout << "=== MLIR Hello World ===" << std::endl << std::endl;

    // ✅ Step 1: Context 생성
    MLIRContext ctx;
    ctx.getOrLoadDialect<FuncDialect>();
    ctx.getOrLoadDialect<arith::ArithDialect>();
    std::cout << "✓ Context 생성" << std::endl;

    // ✅ Step 2: 위치 정보 생성 (디버그용)
    auto loc = UnknownLoc::get(&ctx);
    std::cout << "✓ Location 생성" << std::endl;

    // ✅ Step 3: Module 생성 (최상위 컨테이너)
    auto module = ModuleOp::create(loc);
    std::cout << "✓ Module 생성" << std::endl;

    // ✅ Step 4: Builder 설정
    OpBuilder builder(&ctx);
    builder.setInsertionPointToEnd(module.getBody());
    std::cout << "✓ Builder 설정" << std::endl;

    // ✅ Step 5: 함수 타입 정의
    SmallVector<Type> funcArgs = {
        builder.getI32Type(),
        builder.getI32Type()
    };
    auto funcType = builder.getFunctionType(funcArgs, builder.getI32Type());
    std::cout << "✓ 함수 타입 정의" << std::endl;

    // ✅ Step 6: 함수 생성
    auto func = builder.create<FuncOp>(loc, "add", funcType);
    std::cout << "✓ 함수 생성: @add" << std::endl;

    // ✅ Step 7: 함수 본체 생성
    auto& funcBody = func.getBody();
    auto block = funcBody.addBlock();
    builder.setInsertionPointToEnd(block);
    std::cout << "✓ 함수 본체 생성" << std::endl;

    // ✅ Step 8: 함수 인자 가져오기
    auto arg0 = block->getArgument(0);
    auto arg1 = block->getArgument(1);
    std::cout << "✓ 인자 0, 1 추출" << std::endl;

    // ✅ Step 9: 덧셈 연산 생성
    auto addOp = builder.create<arith::AddIOp>(loc, arg0, arg1);
    std::cout << "✓ AddI 연산 생성" << std::endl;

    // ✅ Step 10: 반환 연산 생성
    builder.create<ReturnOp>(loc, addOp->getResults());
    std::cout << "✓ Return 연산 생성" << std::endl;

    // ✅ Step 11: IR 출력
    std::cout << "\n=== 생성된 MLIR IR ===" << std::endl;
    module.print(llvm::outs());
    std::cout << std::endl;

    std::cout << "=== 성공! ===" << std::endl;

    return 0;
}
```

### 5.4 빌드 및 실행

```bash
# 1. 빌드 디렉토리 생성
mkdir build && cd build

# 2. CMake 구성
cmake .. -DMLIR_DIR=$MLIR_DIR

# 3. 빌드
cmake --build .

# 4. 실행
./hello_world
```

**예상 출력:**
```
=== MLIR Hello World ===

✓ Context 생성
✓ Location 생성
✓ Module 생성
✓ Builder 설정
✓ 함수 타입 정의
✓ 함수 생성: @add
✓ 함수 본체 생성
✓ 인자 0, 1 추출
✓ AddI 연산 생성
✓ Return 연산 생성

=== 생성된 MLIR IR ===
module {
  func.func @add(%arg0: i32, %arg1: i32) -> i32 {
    %0 = arith.addi %arg0, %arg1 : i32
    func.return %0 : i32
  }
}

=== 성공! ===
```

---

### ⚠️ **흔한 실수 #12: Dialect 안 로드**

```cpp
❌ 오류 발생:
MLIRContext ctx;
// ctx.getOrLoadDialect<FuncDialect>();  ← 빼먹음!

auto module = ModuleOp::create(loc);  // ← 오류!
```

**오류 메시지:**
```
error: Unknown Dialect: func
```

```cpp
✅ 올바른 코드:
MLIRContext ctx;
ctx.getOrLoadDialect<FuncDialect>();  // ← 필수!
ctx.getOrLoadDialect<arith::ArithDialect>();  // ← 필수!
```

---

## 📚 **Day 7: 고급 - 곱셈 함수와 조건문**

### 6.1 프로젝트: multiply 함수

```cpp
// 과제: @add를 복사해서 @multiply 함수 만들기

// 변경사항:
// 1. 함수명: "add" → "multiply"
// 2. 연산: arith::AddIOp → arith::MulIOp
// 3. 테스트: 2 * 3 = 6 확인

// 힌트:
auto mulOp = builder.create<arith::MulIOp>(loc, arg0, arg1);
```

### 6.2 프로젝트: 조건문 (scf.if)

```cpp
// @max 함수: max(a, b) 계산

#include "mlir/Dialect/SCF/IR/SCF.h"

using namespace scf;

// 함수 구조:
// func @max(%arg0: i32, %arg1: i32) -> i32 {
//   %cmp = arith.cmpi sgt, %arg0, %arg1 : i32
//   %res = scf.if %cmp -> i32 {
//     scf.yield %arg0 : i32
//   } else {
//     scf.yield %arg1 : i32
//   }
//   func.return %res : i32
// }
```

---

### ✅ **자기점검 Quiz 5.1**

다음 중 맞는 설명은?

```
A) MLIR은 LLVM을 완전히 대체한다
B) MLIR은 LLVM으로 낮춰질(lowering) 수 있다
C) Module은 Operation의 일종이다
D) Context는 각 함수마다 별개로 필요하다
```

<details>
<summary>정답 보기</summary>

**정답: B, C**

설명:
- B: 맞음. MLIR은 점진적으로 LLVM IR로 변환됨
- C: 맞음. ModuleOp는 Operation의 특수한 형태
- A: 틀림. MLIR과 LLVM은 상호 보완 관계
- D: 틀림. Context는 프로그램 전체에서 1개만 필요
</details>

---

## ✅ **Week 5 최종 자기점검**

다음을 모두 할 수 있으면 Week 5 완료입니다!

- [ ] MLIR 아키텍처 (다중 추상화 수준)의 장점을 설명할 수 있다
- [ ] Operation, Region, Block의 관계를 그릴 수 있다
- [ ] Type 시스템 (i32, f32, tensor, memref)을 이해한다
- [ ] MLIRContext의 역할을 설명할 수 있다
- [ ] Dialect를 등록하고 사용할 수 있다
- [ ] OpBuilder로 연산을 생성할 수 있다
- [ ] Hello World IR 생성 코드를 작성 및 컴파일했다
- [ ] 생성된 IR을 읽고 이해할 수 있다
- [ ] 함수 타입과 인자를 정의할 수 있다

---

## 📚 **추가 학습 (심화)**

### 더 깊이 있게

- **Attribute 시스템**: 메타데이터 관리
- **SSA 값**: Value와 Operation의 관계
- **Operand & Result**: 연산 입출력
- **Block 분기**: 조건문과 루프
- **Dominance**: 제어 흐름 분석

---

## 🔗 **다음: Week 6 Dialect 설계**

이번 주에는 **자신만의 Operation을 정의**하고 **TableGen으로 코드를 자동 생성**합니다!

---

**팁**: MLIR은 배우는 데 시간이 걸리지만, 일단 이해하면 정말 강력합니다! 🚀

