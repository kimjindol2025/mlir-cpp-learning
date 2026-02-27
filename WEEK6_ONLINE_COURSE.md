# 🎓 Week 6: Dialect 설계 (온라인 혼자 학습용)

**목표**: 커스텀 Operation과 Type 정의, TableGen으로 자동 코드 생성

**난이도**: ⭐⭐⭐⭐⭐ 최고 난이도
**학습 시간**: 약 25시간
**선수과목**: Week 5 완료

---

## 📖 핵심 학습

### Dialect이란?
**특정 영역을 위한 Operation과 Type의 집합**

```
Tensor Dialect: 신경망 연산
├─ tensor.reshape
├─ tensor.contract
└─ tensor.transpose

Vector Dialect: SIMD 연산
├─ vector.add
├─ vector.reduce
└─ vector.broadcast

Custom Dialect: 자신의 도메인
├─ custom.fused_op
├─ custom.tensor
└─ custom.optimize
```

### TableGen으로 Operation 정의

```
// SimpleDSL.td
include "mlir/IR/OpBase.td"

def SimpleDSL_Dialect : Dialect {
  let name = "simple";
  let cppNamespace = "simple";
};

def MatMulOp : Op<SimpleDSL_Dialect, "matmul", [Pure]> {
  let summary = "Matrix multiplication";
  let arguments = (ins
    TensorOf<[F32]>:$A,
    TensorOf<[F32]>:$B
  );
  let results = (outs TensorOf<[F32]>:$C);
  let assemblyFormat = "$A `,` $B attr-dict `:` type($A) `,` type($B) `->` type($C)";
};

def AddOp : Op<SimpleDSL_Dialect, "add", [Pure, ElementwiseMappable]> {
  let summary = "Element-wise addition";
  let arguments = (ins TensorOf<[F32]>:$lhs, TensorOf<[F32]>:$rhs);
  let results = (outs TensorOf<[F32]>:$result);
};
```

### TableGen으로 코드 생성

```bash
mlir-tblgen -gen-op-decls SimpleDSL.td -o SimpleDSLOps.h.inc
mlir-tblgen -gen-op-defs SimpleDSL.td -o SimpleDSLOps.cpp.inc
mlir-tblgen -gen-dialect-decls SimpleDSL.td -o SimpleDSLDialect.h.inc
```

### C++에서 사용

```cpp
#include "mlir/IR/MLIRContext.h"
#include "SimpleDSLOps.h"

using namespace simple;

int main() {
    MLIRContext ctx;
    ctx.getOrLoadDialect<SimpleDSLDialect>();
    
    OpBuilder builder(&ctx);
    auto loc = UnknownLoc::get(&ctx);
    
    auto f32 = builder.getF32Type();
    auto tensorType = RankedTensorType::get({10, 20}, f32);
    
    // Operation 생성 (TableGen으로 자동 생성된 클래스)
    // auto matmul = builder.create<MatMulOp>(loc, resultType, A, B);
    
    return 0;
}
```

---

## ✅ **Week 6 체크리스트**

- [ ] Dialect의 개념 이해
- [ ] TableGen 문법 학습
- [ ] 3개 이상의 Operation 정의
- [ ] 커스텀 Type 정의
- [ ] 코드 자동 생성 실행
- [ ] Operation 인스턴스 생성 및 사용

---

**다음**: Week 7 - Pass 프레임워크
