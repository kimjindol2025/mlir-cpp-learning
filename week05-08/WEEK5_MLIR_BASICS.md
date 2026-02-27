# 🚀 Week 5: MLIR 기초 & C++ API

**목표**: MLIR 아키텍처 이해, Context와 Builder 마스터, 첫 IR 생성

**기간**: 2026-03-29 ~ 2026-04-04 (7일)

**선수 과목**: Week 1-4 완료 (C++ 기초)

---

## 📚 **Day 1-2: MLIR 아키텍처**

### MLIR이란?

```
Multi-Level Intermediate Representation (MLIR)
├─ 다중 추상화 수준을 단일 컴파일러 프레임워크로 통합
├─ 프로그래밍 언어 → 하드웨어까지 여러 중간 표현
└─ LLVM의 IR과 다르게, 도메인 특화 Dialect 지원
```

### MLIR의 층계 구조

```
┌─────────────────────────────────┐
│  High-Level (Python/TensorFlow) │ ← 프로그래밍 언어
├─────────────────────────────────┤
│  Tensor Dialect                 │ ← 수학 연산
├─────────────────────────────────┤
│  Vector Dialect                 │ ← 벡터 연산
├─────────────────────────────────┤
│  Affine Dialect                 │ ← 루프 변환
├─────────────────────────────────┤
│  Memref Dialect                 │ ← 메모리 관리
├─────────────────────────────────┤
│  GPU Dialect                    │ ← GPU 커널
├─────────────────────────────────┤
│  LLVM Dialect                   │ ← LLVM IR
├─────────────────────────────────┤
│  Hardware (x86, ARM, GPU)       │ ← 기계 코드
└─────────────────────────────────┘
```

### MLIR의 핵심 철학

```cpp
// 1. Polyhedral Model (루프 변환)
affine.for %i = 0 to 10 {
  affine.for %j = 0 to 10 {
    memref.store %value, %A[%i, %j]
  }
}

// 2. SSA (Static Single Assignment)
%0 = arith.addi %arg0, %arg1 : i32
%1 = arith.muli %0, %arg2 : i32
return %1

// 3. Extensibility (도메인 특화 Dialect)
mycompiler.fused_op %A, %B -> %C
```

---

## 📚 **Day 3-4: Operation, Region, Block**

### 핵심 개념: Nested Structure

```
Module (최상위)
└── FuncOp (함수)
    └── Region (영역)
        └── Block (블록)
            ├── Operation (연산)
            ├── Operation
            └── Operation
```

### Operation (연산의 단위)

```cpp
// MLIR Operation의 구조:
// op_name operand1, operand2 : type -> result_type

// 예시 1: 정수 덧셈
%0 = arith.addi %arg0, %arg1 : i32

// 예시 2: 배열 접근
%1 = memref.load %A[%i, %j] : memref<10x10xf32>

// 예시 3: 함수 호출
%2 = func.call @helper(%0) : (i32) -> f32
```

### Region & Block

```cpp
// Region: 여러 연산을 포함하는 영역
scf.if %cond {
  Region 1 (then)
  ├── Block 1.1
  │   ├── op1
  │   └── op2
} else {
  Region 2 (else)
  ├── Block 2.1
  │   ├── op3
  │   └── op4
}

// C++ API 예상:
// Region& region = ifOp.getThenRegion();
// Block& block = region.front();
// auto ops = block.getOperations();
```

### MLIR 텍스트 형식 (IR)

```mlir
module {
  func.func @main(%arg0: i32, %arg1: i32) -> i32 {
    %0 = arith.addi %arg0, %arg1 : i32
    %1 = arith.muli %0, %arg0 : i32
    func.return %1 : i32
  }
}
```

---

## 📚 **Day 5-6: Type 과 Attribute**

### Type 시스템

```cpp
// Integer Types
i1, i8, i16, i32, i64      // 고정 너비
ui1, ui8, si16             // 부호 지정

// Float Types
f16, bf16, f32, f64        // IEEE 754

// Tensor Types
tensor<10x20xf32>          // 정적 크기
tensor<?x20xf32>           // 동적 크기
tensor<*xf32>              // 임의 차원

// Memref Types
memref<10x20xf32>          // 메모리 참조
memref<10x20xf32, #layout> // 메모리 레이아웃 지정

// Function Types
(i32, f32) -> i64          // 함수 시그니처
```

### Attribute (메타데이터)

```cpp
// Integer Attribute
i32:42          // 정수값

// Float Attribute
f32:3.14        // 실수값

// String Attribute
"hello"         // 문자열

// Array Attribute
[1, 2, 3]       // 배열

// Dict Attribute
{foo = 42, bar = "hello"}  // 딕셔너리

// 사용 예:
myop.foo = 42 : i32        // 정수 속성
myop.name = "kernel"       // 문자열 속성
```

### 타입과 속성의 차이

```cpp
// Type: 값의 구조를 정의
%0 = arith.constant 42 : i32  // i32는 Type

// Attribute: 메타데이터/설정
%1 = myop %0 {memory_layout = "row_major"}  // memory_layout는 Attribute
```

---

## 📚 **Day 7: Context, Builder & Hello World**

### MLIRContext (전역 컨텍스트)

```cpp
#include "mlir/IR/MLIRContext.h"

using namespace mlir;

int main() {
    // ✅ MLIRContext: 모든 타입/속성 관리
    MLIRContext ctx;

    // Dialect 등록
    ctx.getOrLoadDialect<func::FuncDialect>();
    ctx.getOrLoadDialect<arith::ArithDialect>();

    // 이제 이 ctx를 사용해서 IR 구성
    return 0;
}
```

### OpBuilder (연산 생성 도구)

```cpp
#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"

using namespace mlir;

int main() {
    MLIRContext ctx;

    // OpBuilder: 연산을 쉽게 생성
    OpBuilder builder(&ctx);

    // 위치 설정: 어디에 연산을 추가할지
    auto insertionPoint = builder.getInsertionPoint();

    // 예: i32 상수 생성
    // auto constant = builder.create<arith::ConstantOp>(loc, ...);

    return 0;
}
```

### Hello World: 간단한 IR 생성

```cpp
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Module.h"
#include "mlir/IR/Builders.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include <iostream>

using namespace mlir;

int main() {
    // ✅ Step 1: Context 생성
    MLIRContext ctx;
    ctx.getOrLoadDialect<func::FuncDialect>();
    ctx.getOrLoadDialect<arith::ArithDialect>();

    // ✅ Step 2: Module 생성 (최상위 컨테이너)
    auto loc = UnknownLoc::get(&ctx);
    auto module = ModuleOp::create(loc);

    // ✅ Step 3: Builder 설정
    OpBuilder builder(&ctx);
    builder.setInsertionPointToEnd(module.getBody());

    // ✅ Step 4: 함수 생성
    SmallVector<Type> args;
    args.push_back(builder.getI32Type());
    args.push_back(builder.getI32Type());

    auto funcType = builder.getFunctionType(args, builder.getI32Type());
    auto func = builder.create<func::FuncOp>(loc, "add", funcType);

    // ✅ Step 5: 함수 본체 작성
    auto& body = func.getBody();
    auto block = body.addBlock();
    builder.setInsertionPointToEnd(block);

    // %arg0 + %arg1
    auto lhs = block->getArgument(0);
    auto rhs = block->getArgument(1);

    auto result = builder.create<arith::AddIOp>(loc, lhs, rhs);
    builder.create<func::ReturnOp>(loc, result);

    // ✅ Step 6: IR 출력
    module.print(llvm::outs());
    llvm::outs() << "\n";

    return 0;
}
```

**예상 출력:**
```mlir
module {
  func.func @add(%arg0: i32, %arg1: i32) -> i32 {
    %0 = arith.addi %arg0, %arg1 : i32
    return %0 : i32
  }
}
```

### 실습: 곱셈 함수 작성

```cpp
// 과제: 위 코드를 수정하여 곱셈 함수 생성
// arith::AddIOp → arith::MulIOp로 변경
// 함수명: "multiply"

// 정답:
auto result = builder.create<arith::MulIOp>(loc, lhs, rhs);
```

---

## 🎯 **Week 5 체크리스트**

- [ ] MLIR 아키텍처 이해 (다중 추상화 수준)
- [ ] Operation, Region, Block 개념 이해
- [ ] Type 시스템 숙달 (Integer, Float, Tensor, Memref)
- [ ] Attribute와 Type의 차이 이해
- [ ] MLIRContext 사용 가능
- [ ] OpBuilder로 연산 생성 가능
- [ ] func::FuncOp 생성 가능
- [ ] arith::AddIOp, arith::MulIOp 사용 가능
- [ ] Hello World 프로그램 컴파일 및 실행
- [ ] 간단한 IR을 MLIR 텍스트로 출력

---

## 💾 **빌드 & 실행**

### 사전 요구사항
```bash
# LLVM/MLIR 빌드 (사전에 완료됨)
cmake -G Ninja ../llvm \
  -DLLVM_ENABLE_PROJECTS=mlir \
  -DCMAKE_BUILD_TYPE=Release

ninja

# 환경 변수 설정
export MLIR_DIR=/path/to/llvm-build/lib/cmake/mlir
export LLVM_DIR=/path/to/llvm-build/lib/cmake/llvm
```

### 빌드
```bash
# CMakeLists.txt
cmake_minimum_required(VERSION 3.13.4)
project(mlir-hello)

find_package(MLIR REQUIRED CONFIG)
find_package(LLVM REQUIRED CONFIG)

add_executable(hello_world main.cpp)
target_link_libraries(hello_world PRIVATE MLIRCore MLIRIR MLIRPass)

target_include_directories(hello_world PRIVATE ${LLVM_INCLUDE_DIRS} ${MLIR_INCLUDE_DIRS})
target_compile_definitions(hello_world PRIVATE ${LLVM_DEFINITIONS})
```

```bash
# 컴파일
mkdir build && cd build
cmake ..
cmake --build .

# 실행
./hello_world
```

---

## 📖 **참고 자료**

- MLIR Official Tutorial: https://mlir.llvm.org/docs/
- MLIR Pass Framework: https://mlir.llvm.org/docs/PassesAndAnalyses/
- "MLIR for System Compiler" (Chris Lattner, PLDI 2021)
- LLVM Weekly: https://llvmweekly.org

---

## 🔗 **Week 5와 후속 주차의 관계**

```
Week 5: MLIR 기초 (Operation, Type, Attribute)
    ↓
Week 6: Dialect 설계 (Operation 정의, Custom Type)
    ↓
Week 7: Pass 프레임워크 (변환, 최적화)
    ↓
Week 8: 통합 프로젝트 (Dialect + Pass)
```

---

**다음**: Week 6 - Dialect 설계 & Operation 정의

