# 🚀 Week 6: Dialect 설계 & 커스텀 Operation

**목표**: 도메인 특화 Dialect 설계, 커스텀 Operation 및 Type 정의

**기간**: 2026-04-05 ~ 2026-04-11 (7일)

**선수 과목**: Week 5 완료

---

## 📚 **Day 1-2: Dialect이란?**

### Dialect의 개념

```cpp
// Dialect: 특정 영역을 위한 연산의 집합

// 예: Tensor Dialect (딥러닝)
tensor.reshape %input, %shape : tensor<?x?xf32>
tensor.contract %A, %B -> %C : ...

// 예: Vector Dialect (SIMD)
vector.reduction "add" %vector -> %result : vector<16xf32>

// 예: Affine Dialect (루프 최적화)
affine.for %i = 0 to 10 {
  affine.store %value, %A[%i]
}
```

### 왜 Dialect를 만드는가?

```
Python 코드:
├─ 자신만의 연산 정의 (mycompiler.fused_op)
├─ 자신만의 타입 정의 (mycompiler.tensor)
├─ 자신만의 변환 정의 (mycompiler.optimize_pass)
└─ MLIR 프레임워크 활용

장점:
✓ LLVM 인프라 활용 가능
✓ 기존 Pass 재사용 가능
✓ 여러 레벨의 Dialect 연결 가능
✓ 커뮤니티 기여 가능
```

---

## 📚 **Day 3-4: 커스텀 Operation 정의**

### Operation의 구조

```cpp
// TableGen (MLIR의 설정 언어): SimpleDSL.td
include "mlir/IR/OpBase.td"
include "mlir/IR/EnumAttr.td"

// Dialect 정의
def SimpleDSL_Dialect : Dialect {
  let name = "simpledsl";
  let description = [{
    Simple DSL for educational purposes
  }];
  let cppNamespace = "simpledsl";
}

// Operation 1: Add
def AddOp : Op<SimpleDSL_Dialect, "add", [Pure]> {
  let summary = "Add two tensors";
  let description = [{
    Adds two tensors element-wise:
    %result = simpledsl.add %lhs, %rhs : tensor<10xf32>
  }];

  let arguments = (ins
    TensorOf<[F32]>:$lhs,
    TensorOf<[F32]>:$rhs
  );

  let results = (outs
    TensorOf<[F32]>:$result
  );

  let assemblyFormat = "$lhs `,` $rhs attr-dict `:` type(results)";
}

// Operation 2: MatMul
def MatMulOp : Op<SimpleDSL_Dialect, "matmul", [Pure]> {
  let summary = "Matrix multiplication";
  let description = [{
    %result = simpledsl.matmul %A, %B : tensor<10x20xf32>, tensor<20x30xf32> -> tensor<10x30xf32>
  }];

  let arguments = (ins
    TensorOf<[F32]>:$A,
    TensorOf<[F32]>:$B
  );

  let results = (outs
    TensorOf<[F32]>:$result
  );

  let assemblyFormat = "$A `,` $B attr-dict `:` type($A) `,` type($B) `->` type(results)";
}

// Operation 3: Relu
def ReluOp : Op<SimpleDSL_Dialect, "relu", [Pure]> {
  let summary = "ReLU activation";

  let arguments = (ins
    TensorOf<[F32]>:$input
  );

  let results = (outs
    TensorOf<[F32]>:$output
  );

  let assemblyFormat = "$input attr-dict `:` type(results)";
}
```

### C++ 코드 생성

```cpp
// TableGen 실행
mlir-tblgen -gen-op-defs SimpleDSL.td -o SimpleDSLOps.cpp.inc
mlir-tblgen -gen-op-decls SimpleDSL.td -o SimpleDSLOps.h.inc

// 생성된 C++ 헤더: SimpleDSLOps.h
#ifndef SIMPLEDSL_OPS_H
#define SIMPLEDSL_OPS_H

#include "mlir/IR/OpDefinition.h"
#include "SimpleDSLOpsDialect.h.inc"

#define GET_OP_CLASSES
#include "SimpleDSLOps.h.inc"

#endif

// 생성된 C++ 구현: SimpleDSLOps.cpp
#define GET_OP_CLASSES
#include "SimpleDSLOps.cpp.inc"
```

---

## 📚 **Day 5-6: 커스텀 Type 정의**

### Type 정의

```cpp
// SimpleDSL.td에 추가

// Custom Type: Tensor Shape
def TensorShapeType : DialectType<SimpleDSL_Dialect, CPred<[{
  llvm::isa<TensorShapeType>(($_self))
}]>> {
  let mnemonic = "tensor_shape";
  let description = [{Tensor shape type}];
}

// Custom Type: Device
def DeviceType : DialectType<SimpleDSL_Dialect, CPred<[{
  llvm::isa<DeviceType>(($_self))
}]>> {
  let mnemonic = "device";
  let description = [{GPU device type}];
}
```

### C++ Type 구현

```cpp
// SimpleDSLTypes.h
#include "mlir/IR/Types.h"

namespace simpledsl {

class TensorShapeType : public mlir::Type {
public:
  using mlir::Type::Type;

  static bool classof(mlir::Type type);
  static TensorShapeType get(mlir::MLIRContext *ctx);

  SmallVector<int64_t> getShape() const;
};

class DeviceType : public mlir::Type {
public:
  using mlir::Type::Type;

  static bool classof(mlir::Type type);
  static DeviceType get(mlir::MLIRContext *ctx, StringRef device);

  StringRef getDevice() const;
};

} // namespace simpledsl
```

---

## 📚 **Day 7: 통합 - 첫 Dialect 구현**

### 완전한 Simple Dialect 예제

```cpp
// SimpleDSL.td (완전한 정의)
include "mlir/IR/OpBase.td"
include "mlir/IR/EnumAttr.td"

def SimpleDSL_Dialect : Dialect {
  let name = "simple";
  let cppNamespace = "simple";
  let description = [{
    A simple dialect for educational purposes.
    Demonstrates: custom ops, types, attributes.
  }];

  let emitAccessorPrefix = kEmitAccessorPrefix_Prefixed;
}

// ===== Operations =====

def AddOp : Op<SimpleDSL_Dialect, "add", [Pure, ElementwiseMappable]> {
  let summary = "Element-wise addition";
  let arguments = (ins TensorOf<[F32]>:$lhs, TensorOf<[F32]>:$rhs);
  let results = (outs TensorOf<[F32]>:$result);
  let assemblyFormat = "$lhs `,` $rhs attr-dict `:` type($result)";
}

def MulOp : Op<SimpleDSL_Dialect, "mul", [Pure, ElementwiseMappable]> {
  let summary = "Element-wise multiplication";
  let arguments = (ins TensorOf<[F32]>:$lhs, TensorOf<[F32]>:$rhs);
  let results = (outs TensorOf<[F32]>:$result);
  let assemblyFormat = "$lhs `,` $rhs attr-dict `:` type($result)";
}

def MatMulOp : Op<SimpleDSL_Dialect, "matmul", [Pure]> {
  let summary = "Matrix multiplication";
  let description = [{
    Performs matrix multiplication.
    Example: %C = simple.matmul %A, %B : tensor<M,Kxf32>, tensor<K,Nxf32> -> tensor<M,Nxf32>
  }];

  let arguments = (ins TensorOf<[F32]>:$A, TensorOf<[F32]>:$B);
  let results = (outs TensorOf<[F32]>:$C);

  let assemblyFormat = "$A `,` $B attr-dict `:` type($A) `,` type($B) `->` type($C)";

  let hasCustomAssemblyFormat = 0;
  let hasCanonicalizer = 1;
  let hasFolder = 1;
}

def PoolOp : Op<SimpleDSL_Dialect, "pool", [Pure]> {
  let summary = "Max pooling";
  let arguments = (ins
    TensorOf<[F32]>:$input,
    I64ArrayAttr:$kernel_size,
    I64ArrayAttr:$strides
  );
  let results = (outs TensorOf<[F32]>:$output);
  let assemblyFormat = "$input attr-dict `:` type($input)";
}
```

### Dialect 등록 및 사용

```cpp
// SimpleDSLDialect.h
namespace simple {

class SimpleDSLDialect : public ::mlir::Dialect {
  explicit SimpleDSLDialect(::mlir::MLIRContext *context);

  void initialize();

  static ::llvm::StringRef getDialectNamespace() { return "simple"; }
};

} // namespace simple

// main.cpp에서 사용
#include "mlir/IR/MLIRContext.h"
#include "SimpleDSLDialect.h"
#include "SimpleDSLOps.h"

int main() {
  mlir::MLIRContext ctx;

  // Dialect 등록
  ctx.loadDialect<simple::SimpleDSLDialect>();

  // IR 생성
  auto loc = mlir::UnknownLoc::get(&ctx);
  auto module = mlir::ModuleOp::create(loc);

  mlir::OpBuilder builder(&ctx);
  builder.setInsertionPointToEnd(module.getBody());

  // 타입 생성
  auto f32_type = builder.getF32Type();
  auto tensor_type = mlir::RankedTensorType::get({10, 10}, f32_type);

  // Operation 생성
  auto a = builder.create<simple::AddOp>(loc, tensor_type, ValueRange{});
  auto m = builder.create<simple::MulOp>(loc, tensor_type, ValueRange{});

  // 출력
  module.print(llvm::outs());

  return 0;
}
```

---

## 🎯 **Week 6 체크리스트**

- [ ] Dialect 개념 완전 이해
- [ ] Operation 정의 방법 학습 (TableGen)
- [ ] Type 정의 방법 학습
- [ ] Attribute 정의 방법 학습
- [ ] 간단한 Dialect 설계 (5개 이상 Operation)
- [ ] Dialect 등록 및 사용 가능
- [ ] Operation 인스턴스 생성 가능
- [ ] IR 텍스트 형식 이해
- [ ] 커스텀 Operation 검증 (verification)
- [ ] AssemblyFormat 정의 가능

---

## 💾 **빌드 & 실행**

### TableGen으로 코드 생성
```bash
# MLIR OpsGen에서 C++ 코드 자동 생성
mlir-tblgen -gen-op-decls SimpleDSL.td -o SimpleDSLOps.h.inc
mlir-tblgen -gen-op-defs SimpleDSL.td -o SimpleDSLOps.cpp.inc
mlir-tblgen -gen-dialect-decls SimpleDSL.td -o SimpleDSLDialect.h.inc
mlir-tblgen -gen-dialect-defs SimpleDSL.td -o SimpleDSLDialect.cpp.inc
```

### 컴파일
```bash
# 생성된 코드 포함
clang++ -I${MLIR_INCLUDE} -I${LLVM_INCLUDE} \
  main.cpp SimpleDSLDialect.cpp \
  -o simple_dialect \
  -L${MLIR_LIB} -L${LLVM_LIB} \
  -lMLIRCore -lMLIRIR
```

---

## 📖 **추가 학습**

- MLIR Dialect Tutorial: https://mlir.llvm.org/docs/Dialects/
- MLIR OpDef Language: https://mlir.llvm.org/docs/OpDefinitions/
- TableGen의 기초: https://llvm.org/docs/TableGen/
- "MLIR as a Compiler Infrastructure" (Chris Lattner)

---

**다음**: Week 7 - Pass 프레임워크 & 최적화

