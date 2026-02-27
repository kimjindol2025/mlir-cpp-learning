# 🚀 Week 7: Pass 프레임워크 & 최적화

**목표**: 커스텀 Pass 작성, Operation 변환, 최적화 기법 습득

**기간**: 2026-04-12 ~ 2026-04-18 (7일)

**선수 과목**: Week 5-6 완료

---

## 📚 **Day 1-2: Pass 시스템이란?**

### Pass의 개념

```cpp
// Pass: IR을 분석하고 변환하는 기능 단위

Pass의 종류:
1. OperationPass (함수/모듈 단위)
2. FunctionPass (함수 단위)
3. ModulePass (전체 모듈 단위)
4. InterfacePass (특정 인터페이스 구현 Op)

동작:
IR → [Pass 1] → [Pass 2] → [Pass 3] → 최적화된 IR
```

### 간단한 Pass 예제

```cpp
// SimpleDSL Dialect에 대한 Pass 작성

#include "mlir/Pass/Pass.h"
#include "SimpleDSLOps.h"

namespace simple {

// OpPass: 모든 Operation을 순회하며 변환
class SimplifyPass : public mlir::OperationPass<> {
public:
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(SimplifyPass)

  void runOnOperation() final {
    Operation *op = getOperation();

    // 모든 작업을 순회
    op->walk([](Operation *inner_op) {
      // AddOp를 찾으면 상수 폴딩
      if (auto add_op = dyn_cast<AddOp>(inner_op)) {
        // 최적화 로직
      }

      // MulOp를 찾으면 변환
      if (auto mul_op = dyn_cast<MulOp>(inner_op)) {
        // 최적화 로직
      }
    });
  }
};

} // namespace simple
```

---

## 📚 **Day 3-4: Pattern과 Rewriter**

### Pattern 기반 변환

```cpp
#include "mlir/IR/PatternMatch.h"
#include "SimpleDSLOps.h"

namespace simple {

// Pattern 1: Add(x, 0) → x (항등원소 제거)
struct AddZeroPattern : public OpRewritePattern<AddOp> {
  using OpRewritePattern::OpRewritePattern;

  LogicalResult matchAndRewrite(AddOp op, PatternRewriter &rewriter) const final {
    // 오른쪽 피연산자가 0인지 확인
    auto rhs = op.getRhs();
    auto lhs = op.getLhs();

    // TODO: 상수 폴딩 확인
    // if (isZero(rhs)) {
    //   rewriter.replaceOp(op, lhs);
    //   return success();
    // }

    return failure();
  }
};

// Pattern 2: Mul(x, 1) → x
struct MulOnePattern : public OpRewritePattern<MulOp> {
  using OpRewritePattern::OpRewritePattern;

  LogicalResult matchAndRewrite(MulOp op, PatternRewriter &rewriter) const final {
    // 비슷한 로직
    return failure();
  }
};

// Pattern 3: Add(Mul(x, a), Mul(x, b)) → Mul(x, Add(a, b))
struct DistributivityPattern : public OpRewritePattern<AddOp> {
  using OpRewritePattern::OpRewritePattern;

  LogicalResult matchAndRewrite(AddOp op, PatternRewriter &rewriter) const final {
    // 복잡한 패턴 매칭
    auto lhs = op.getLhs();
    auto rhs = op.getRhs();

    if (auto mul1 = dyn_cast_or_null<MulOp>(lhs.getDefiningOp())) {
      if (auto mul2 = dyn_cast_or_null<MulOp>(rhs.getDefiningOp())) {
        // 공통 인수 확인
        // if (mul1.getLhs() == mul2.getLhs()) {
        //   // (x*a) + (x*b) → x*(a+b)
        // }
      }
    }

    return failure();
  }
};

} // namespace simple
```

### Greedy Pattern Rewriter

```cpp
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"

namespace simple {

class SimplifyPass : public mlir::OperationPass<> {
public:
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(SimplifyPass)

  void runOnOperation() final {
    Operation *op = getOperation();
    MLIRContext *ctx = &getContext();

    // Pattern 목록 생성
    RewritePatternSet patterns(ctx);
    patterns.add<AddZeroPattern, MulOnePattern, DistributivityPattern>(ctx);

    // GreedyPatternRewriter: 패턴을 반복해서 적용
    if (failed(applyPatternsAndFoldGreedily(op, std::move(patterns)))) {
      return signalPassFailure();
    }
  }
};

} // namespace simple
```

---

## 📚 **Day 5-6: 구체적인 최적화 Pass**

### 상수 폴딩 (Constant Folding)

```cpp
// Add 연산의 두 피연산자가 모두 상수면 미리 계산
class ConstantFoldingPass : public mlir::OperationPass<> {
public:
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ConstantFoldingPass)

  void runOnOperation() final {
    Operation *op = getOperation();
    MLIRContext *ctx = &getContext();

    op->walk([ctx](AddOp add_op) {
      // LHS와 RHS가 모두 상수인지 확인
      auto lhs = dyn_cast_or_null<arith::ConstantOp>(add_op.getLhs().getDefiningOp());
      auto rhs = dyn_cast_or_null<arith::ConstantOp>(add_op.getRhs().getDefiningOp());

      if (lhs && rhs) {
        // 상수값 추출 및 계산
        // auto result_val = lhs_val + rhs_val;

        // 새로운 상수 생성
        // auto result = builder.create<arith::ConstantOp>(loc, result_val);

        // 사용처 모두 대체
        // add_op.replaceAllUsesWith(result);
      }
    });
  }
};
```

### 데드 코드 제거 (Dead Code Elimination)

```cpp
class DeadCodeEliminationPass : public mlir::OperationPass<> {
public:
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(DeadCodeEliminationPass)

  void runOnOperation() final {
    Operation *op = getOperation();

    op->walk([](Operation *inner_op) {
      // 사용처가 없으면 제거
      if (inner_op->use_empty() && !mlir::isOpTriviallyDead(inner_op)) {
        inner_op->erase();
      }
    });
  }
};
```

### 루프 최적화 (Loop Optimization)

```cpp
// Affine Dialect의 루프 타일링
class LoopTilingPass : public mlir::OperationPass<> {
public:
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(LoopTilingPass)

  void runOnOperation() final {
    Operation *op = getOperation();

    op->walk([](affine::AffineForOp for_op) {
      // 타일 크기
      int tile_size = 32;

      // affine::affineParallelForOp로 변환
      // 또는 스케줄링 변환
      // affine::tilePerfectlyNested(for_op, ...);
    });
  }
};
```

---

## 📚 **Day 7: 통합 - 완전한 최적화 Pass**

### 실전 최적화 Pass: MatMul Fusion

```cpp
#include "mlir/Pass/Pass.h"
#include "mlir/IR/PatternMatch.h"
#include "SimpleDSLOps.h"

namespace simple {

// Pass: MatMul + ReLU → MatMulReLU로 Fusion
class MatMulFusionPass : public mlir::OperationPass<> {
public:
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(MatMulFusionPass)

  StringRef getArgument() const final { return "simple-matmul-fusion"; }
  StringRef getDescription() const final {
    return "Fuse MatMul + ReLU into single operation";
  }

  void runOnOperation() final {
    Operation *op = getOperation();
    MLIRContext *ctx = &getContext();

    // Pattern: MatMul의 출력이 ReLU의 입력인 경우
    op->walk([ctx](MatMulOp matmul_op) {
      // 사용처 찾기
      for (auto user : matmul_op.getResult().getUsers()) {
        if (auto relu_op = dyn_cast<ReluOp>(user)) {
          // Fusion 가능
          performFusion(matmul_op, relu_op);
        }
      }
    });
  }

private:
  void performFusion(MatMulOp matmul, ReluOp relu) {
    // 새로운 Fused Operation 생성
    // OpBuilder builder(relu);
    // auto fused = builder.create<MatMulReluFusedOp>(...);
    // relu.replaceAllUsesWith(fused);
  }
};

// Pass 등록
void registerSimplePasses() {
  PassRegistration<MatMulFusionPass>();
  PassRegistration<SimplifyPass>();
  PassRegistration<DeadCodeEliminationPass>();
}

} // namespace simple
```

### Pass Manager로 Pass 실행

```cpp
#include "mlir/Pass/PassManager.h"

int main() {
  mlir::MLIRContext ctx;
  // ... IR 로드 ...

  // PassManager 생성
  mlir::PassManager pm(&ctx);

  // Pass 추가 (순서 중요!)
  pm.addPass(simple::createMatMulFusionPass());
  pm.addPass(simple::createSimplifyPass());
  pm.addPass(simple::createDeadCodeEliminationPass());

  // Pass 실행
  if (failed(pm.run(module))) {
    llvm::errs() << "Pass execution failed!\n";
    return 1;
  }

  // 최적화된 IR 출력
  module.print(llvm::outs());

  return 0;
}
```

---

## 🎯 **Week 7 체크리스트**

- [ ] Pass 시스템 이해 (OperationPass, FunctionPass)
- [ ] Pattern과 Rewriter 개념 이해
- [ ] OpRewritePattern 작성 가능
- [ ] GreedyPatternRewriter 사용 가능
- [ ] 상수 폴딩 구현 가능
- [ ] 데드 코드 제거 구현 가능
- [ ] 루프 최적화 개념 이해
- [ ] 완전한 최적화 Pass 구현
- [ ] PassManager로 여러 Pass 조합 가능
- [ ] 최적화 효과 측정 가능

---

## 💾 **빌드 & 실행**

```bash
# Pass 등록 및 실행
mlir-opt -simple-matmul-fusion -simple-simplify -simple-dce input.mlir -o output.mlir

# 결과 확인
cat output.mlir
```

---

## 📊 **최적화 성과 예시**

```
입력 IR (최적화 전):
%0 = simple.matmul %A, %B
%1 = simple.relu %0
%2 = simple.mul %1, 2.0

출력 IR (MatMulFusion Pass 후):
%0 = simple.matmul_relu %A, %B
%1 = simple.mul %0, 2.0

출력 IR (상수 폴딩 Pass 후):
%0 = simple.matmul_relu %A, %B
%1 = simple.mul %0, 2.0

출력 IR (DCE Pass 후):
%0 = simple.matmul_relu %A, %B
%1 = simple.mul %0, 2.0

성과:
✓ 연산 개수: 3개 → 2개 (33% 감소)
✓ 메모리 접근: 2회 → 1회 (50% 감소)
✓ 실행 시간: ~1.5배 개선
```

---

## 📖 **추가 학습**

- MLIR Pass Framework: https://mlir.llvm.org/docs/PassesAndAnalyses/
- PatternMatch Tutorial: https://mlir.llvm.org/docs/PatternRewriter/
- GreedyPatternRewriter: https://mlir.llvm.org/docs/GreedyPatternRewriter/
- "MLIR Code Generation and Optimization" (CppCon 2021)

---

**다음**: Week 8 - 통합 프로젝트 (SimpleDSL 완성)

