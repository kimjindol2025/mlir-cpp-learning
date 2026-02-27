# 🎓 Week 7: Pass 프레임워크 (온라인 혼자 학습용)

**목표**: IR 변환과 최적화 Pass 구현

---

## 📖 핵심: Pass는 IR 변환기

```
IR 입력 → [Pass 1] → [Pass 2] → [Pass 3] → 최적화된 IR

Pass 1: 상수 폴딩
Pass 2: MatMul-ReLU Fusion
Pass 3: 데드 코드 제거
```

## Pattern Matching & Rewriting

```cpp
#include "mlir/IR/PatternMatch.h"

// Pattern: Add(x, 0) → x
struct AddZeroPattern : public OpRewritePattern<AddOp> {
    LogicalResult matchAndRewrite(AddOp op, PatternRewriter &rewriter) const final {
        // 우측이 0인지 확인
        // 맞으면 op를 lhs로 대체
        // rewriter.replaceOp(op, op.getLhs());
        return failure();  // 매치 안 됨
    }
};

// Pass 등록
class SimplifyPass : public OperationPass<> {
public:
    void runOnOperation() final {
        Operation *op = getOperation();
        RewritePatternSet patterns(&getContext());
        patterns.add<AddZeroPattern>(&getContext());
        
        if (failed(applyPatternsAndFoldGreedily(op, std::move(patterns)))) {
            return signalPassFailure();
        }
    }
};
```

## ✅ **Week 7 체크리스트**

- [ ] Pass 시스템 이해
- [ ] Pattern 매칭 구현
- [ ] Rewriter 사용
- [ ] 4개 최적화 Pass 완성
- [ ] Pass 조합 및 실행

---

**다음**: Week 8 - 통합 프로젝트
