/**
 * Exercise 3: Pass 프레임워크 - 최적화 Pass 작성
 *
 * 학습 목표:
 * - Pass 기본 구조 이해
 * - ModulePass vs FunctionPass
 * - 상수 폴딩 (Constant Folding) 최적화
 * - Dead Code Elimination (DCE)
 *
 * 난이도: ⭐⭐ 중급 (Junior Level)
 *
 * 주의: 이 코드는 LLVM/MLIR이 설치된 환경에서 컴파일해야 합니다.
 *
 * 빌드 방법:
 * cd mlir-cpp-learning/week05-08
 * mkdir -p build && cd build
 * cmake ..
 * cmake --build .
 * ./exercise3
 */

#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Module.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Pass/PassManager.h"
#include "llvm/ADT/DenseMap.h"
#include <iostream>
#include <memory>

using namespace mlir;
using namespace mlir::func;

// ============================================================================
// Pass 1: Constant Folding (상수 폴딩)
// ============================================================================

/**
 * 상수 폴딩 Pass
 *
 * 목표: 컴파일 시간에 계산할 수 있는 상수 연산들을 미리 계산
 *
 * 예:
 *   Before: %0 = arith.addi %c2, %c3 : i32
 *   After:  %0 = arith.constant 5 : i32
 */
class ConstantFoldingPass : public OperationPass<ConstantFoldingPass> {
public:
    MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ConstantFoldingPass)

    void runOnOperation() override {
        // 현재 Operation (보통 함수)
        Operation *operation = getOperation();

        // Operation의 모든 자식 Operation 방문
        operation->walk([&](Operation *op) {
            // arith 연산만 처리
            if (op->getDialect()->getNamespace() != "arith") {
                return;
            }

            // AddI, MulI, SubI 등을 처리할 수 있음
            if (auto addOp = dyn_cast<arith::AddIOp>(op)) {
                // 두 상수의 합
                auto lhsAttr = addOp.getLhs().getDefiningOp<arith::ConstantOp>();
                auto rhsAttr = addOp.getRhs().getDefiningOp<arith::ConstantOp>();

                if (lhsAttr && rhsAttr) {
                    // 두 상수 모두 있으면 미리 계산
                    auto lhsValue =
                        lhsAttr.getValue().cast<IntegerAttr>().getInt();
                    auto rhsValue =
                        rhsAttr.getValue().cast<IntegerAttr>().getInt();
                    auto resultValue = lhsValue + rhsValue;

                    // 새로운 상수 Operation 생성
                    OpBuilder builder(addOp);
                    auto newConst = builder.create<arith::ConstantOp>(
                        addOp.getLoc(),
                        builder.getI32IntegerAttr(resultValue));

                    // 기존 사용처를 새로운 상수로 변경
                    addOp.getResult().replaceAllUsesWith(newConst);

                    // 기존 덧셈 Operation 제거
                    addOp->erase();
                }
            }
        });
    }

    StringRef getArgument() const final { return "test-constant-folding"; }
    StringRef getDescription() const final {
        return "상수 연산을 컴파일 시간에 계산";
    }
};

// ============================================================================
// Pass 2: Dead Code Elimination (불필요한 코드 제거)
// ============================================================================

/**
 * Dead Code Elimination Pass
 *
 * 목표: 사용되지 않는 Operation들을 제거
 *
 * 예:
 *   Before: %0 = arith.constant 42 : i32
 *           %1 = arith.addi %0, %0 : i32
 *           return %0
 *   After:  %0 = arith.constant 42 : i32
 *           return %0
 */
class DeadCodeEliminationPass : public OperationPass<DeadCodeEliminationPass> {
public:
    MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(DeadCodeEliminationPass)

    void runOnOperation() override {
        Operation *operation = getOperation();
        bool changed = true;

        // 반복적으로 Dead Code 제거 (여러 번 반복할 수 있음)
        int iterations = 0;
        while (changed && iterations < 10) {
            changed = false;
            iterations++;

            operation->walk([&](Operation *op) {
                // Return, Yield 등 필수 Operation은 건너뛰기
                if (isa<ReturnOp>(op) || isa<scf::YieldOp>(op)) {
                    return;
                }

                // 결과값이 있는 Operation
                if (op->getNumResults() > 0) {
                    bool isUsed = false;

                    // 모든 결과값이 사용되는지 확인
                    for (auto result : op->getResults()) {
                        if (!result.use_empty()) {
                            isUsed = true;
                            break;
                        }
                    }

                    // 사용되지 않으면 제거
                    if (!isUsed) {
                        op->erase();
                        changed = true;
                    }
                }
            });
        }

        if (iterations > 0) {
            llvm::outs() << "Dead Code Elimination: " << iterations
                         << "회 반복\n";
        }
    }

    StringRef getArgument() const final { return "test-dead-code-elimination"; }
    StringRef getDescription() const final {
        return "사용되지 않는 Operation 제거";
    }
};

// ============================================================================
// Pass 3: Operation Counting Pass (분석용)
// ============================================================================

/**
 * Operation 분석 Pass
 *
 * 목표: IR에서 각 Operation 타입의 개수를 세어서 보고
 *
 * 예:
 *   arith.constant: 3개
 *   arith.addi: 2개
 *   func.return: 1개
 */
class OperationCountPass : public OperationPass<OperationCountPass> {
public:
    MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(OperationCountPass)

    void runOnOperation() override {
        Operation *operation = getOperation();

        // Operation 개수를 저장할 맵
        llvm::DenseMap<StringRef, int> opCount;
        int totalOps = 0;

        // 모든 Operation 방문
        operation->walk([&](Operation *op) {
            StringRef opName = op->getName().getStringRef();
            opCount[opName]++;
            totalOps++;
        });

        // 결과 출력
        llvm::outs() << "\n=== Operation 분석 ===\n";
        llvm::outs() << "총 Operation 수: " << totalOps << "\n";
        llvm::outs() << "\nOperation별 분포:\n";

        for (const auto &[opName, count] : opCount) {
            llvm::outs() << "  " << opName << ": " << count << "개\n";
        }
    }

    StringRef getArgument() const final { return "test-operation-count"; }
    StringRef getDescription() const final {
        return "Operation 타입별 분포 분석";
    }
};

// ============================================================================
// Pass 4: Value 추적 Pass
// ============================================================================

/**
 * Value 흐름 분석 Pass
 *
 * 목표: 각 Value가 어디서 생성되고 어디서 사용되는지 추적
 */
class ValueTracingPass : public OperationPass<ValueTracingPass> {
public:
    MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ValueTracingPass)

    void runOnOperation() override {
        Operation *operation = getOperation();

        llvm::outs() << "\n=== Value 흐름 분석 ===\n";

        operation->walk([&](Operation *op) {
            // 결과값 분석
            for (auto result : op->getResults()) {
                int useCount = 0;
                for (auto use : result.getUses()) {
                    useCount++;
                }

                llvm::outs() << "Value from " << op->getName().getStringRef()
                             << ": " << useCount << "회 사용\n";

                if (useCount == 0) {
                    llvm::outs() << "  → ⚠️ Dead Value!\n";
                } else {
                    for (auto use : result.getUses()) {
                        llvm::outs() << "  → Used in "
                                     << use.getOwner()->getName().getStringRef()
                                     << "\n";
                    }
                }
            }
        });
    }

    StringRef getArgument() const final { return "test-value-tracing"; }
    StringRef getDescription() const final {
        return "Value 사용처 추적 분석";
    }
};

// ============================================================================
// 테스트 코드
// ============================================================================

/**
 * 테스트 1: 기본 상수 폴딩
 */
void testConstantFolding() {
    std::cout << "\n=== 테스트 1: 상수 폴딩 ===" << std::endl;

    MLIRContext ctx;
    ctx.loadDialect<func::FuncDialect>();
    ctx.loadDialect<arith::ArithDialect>();

    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    // 함수: () -> i32
    auto funcType = FunctionType::get(&ctx, {}, {IntegerType::get(&ctx, 32)});
    FuncOp func = FuncOp::create(loc, "fold_test", funcType);
    module.body().push_back(func);

    Block &block = func.addEntryBlock();
    OpBuilder builder(&block);

    // c2 = 2
    auto c2 = builder.create<arith::ConstantOp>(loc,
                                                 builder.getI32IntegerAttr(2));
    // c3 = 3
    auto c3 = builder.create<arith::ConstantOp>(loc,
                                                 builder.getI32IntegerAttr(3));
    // result = c2 + c3 (컴파일 시간에 5로 계산 가능)
    auto add = builder.create<arith::AddIOp>(loc, c2, c3);
    builder.create<ReturnOp>(loc, ValueRange{add});

    std::cout << "Before:" << std::endl;
    module.print(llvm::outs());

    // Pass 실행
    PassManager pm(&ctx);
    pm.addPass(std::make_unique<ConstantFoldingPass>());
    if (failed(pm.run(module))) {
        std::cout << "❌ Pass 실행 실패" << std::endl;
        return;
    }

    std::cout << "\nAfter (상수 폴딩 적용):" << std::endl;
    module.print(llvm::outs());
}

/**
 * 테스트 2: Dead Code Elimination
 */
void testDeadCodeElimination() {
    std::cout << "\n=== 테스트 2: Dead Code Elimination ===" << std::endl;

    MLIRContext ctx;
    ctx.loadDialect<func::FuncDialect>();
    ctx.loadDialect<arith::ArithDialect>();

    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    auto funcType = FunctionType::get(&ctx, {}, {IntegerType::get(&ctx, 32)});
    FuncOp func = FuncOp::create(loc, "dce_test", funcType);
    module.body().push_back(func);

    Block &block = func.addEntryBlock();
    OpBuilder builder(&block);

    // 사용될 값
    auto c1 = builder.create<arith::ConstantOp>(loc,
                                                 builder.getI32IntegerAttr(1));
    // 사용되지 않을 값
    auto c2 = builder.create<arith::ConstantOp>(loc,
                                                 builder.getI32IntegerAttr(2));
    auto c3 = builder.create<arith::ConstantOp>(loc,
                                                 builder.getI32IntegerAttr(3));
    // 사용되지 않는 덧셈
    auto add = builder.create<arith::AddIOp>(loc, c2, c3);

    builder.create<ReturnOp>(loc, ValueRange{c1});

    std::cout << "Before:" << std::endl;
    module.print(llvm::outs());

    // Pass 실행
    PassManager pm(&ctx);
    pm.addPass(std::make_unique<DeadCodeEliminationPass>());
    if (failed(pm.run(module))) {
        std::cout << "❌ Pass 실행 실패" << std::endl;
        return;
    }

    std::cout << "\nAfter (Dead Code 제거):" << std::endl;
    module.print(llvm::outs());
}

/**
 * 테스트 3: Operation Count 분석
 */
void testOperationCounting() {
    std::cout << "\n=== 테스트 3: Operation 분포 분석 ===" << std::endl;

    MLIRContext ctx;
    ctx.loadDialect<func::FuncDialect>();
    ctx.loadDialect<arith::ArithDialect>();

    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    // Function 1
    {
        auto funcType = FunctionType::get(&ctx, {}, {IntegerType::get(&ctx, 32)});
        FuncOp func = FuncOp::create(loc, "func1", funcType);
        module.body().push_back(func);

        Block &block = func.addEntryBlock();
        OpBuilder builder(&block);

        auto c1 = builder.create<arith::ConstantOp>(loc,
                                                     builder.getI32IntegerAttr(1));
        auto c2 = builder.create<arith::ConstantOp>(loc,
                                                     builder.getI32IntegerAttr(2));
        auto add = builder.create<arith::AddIOp>(loc, c1, c2);
        builder.create<ReturnOp>(loc, ValueRange{add});
    }

    // Function 2
    {
        auto funcType = FunctionType::get(&ctx, {}, {IntegerType::get(&ctx, 32)});
        FuncOp func = FuncOp::create(loc, "func2", funcType);
        module.body().push_back(func);

        Block &block = func.addEntryBlock();
        OpBuilder builder(&block);

        auto c3 = builder.create<arith::ConstantOp>(loc,
                                                     builder.getI32IntegerAttr(3));
        auto c4 = builder.create<arith::ConstantOp>(loc,
                                                     builder.getI32IntegerAttr(4));
        auto c5 = builder.create<arith::ConstantOp>(loc,
                                                     builder.getI32IntegerAttr(5));
        auto mul = builder.create<arith::MulIOp>(loc, c3, c4);
        builder.create<ReturnOp>(loc, ValueRange{mul});
    }

    // Pass 실행
    PassManager pm(&ctx);
    pm.addPass(std::make_unique<OperationCountPass>());
    if (failed(pm.run(module))) {
        std::cout << "❌ Pass 실행 실패" << std::endl;
        return;
    }
}

/**
 * 테스트 4: Value 추적 분석
 */
void testValueTracing() {
    std::cout << "\n=== 테스트 4: Value 흐름 분석 ===" << std::endl;

    MLIRContext ctx;
    ctx.loadDialect<func::FuncDialect>();
    ctx.loadDialect<arith::ArithDialect>();

    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    auto funcType = FunctionType::get(&ctx, {}, {IntegerType::get(&ctx, 32)});
    FuncOp func = FuncOp::create(loc, "value_test", funcType);
    module.body().push_back(func);

    Block &block = func.addEntryBlock();
    OpBuilder builder(&block);

    auto c1 = builder.create<arith::ConstantOp>(loc, builder.getI32IntegerAttr(1));
    auto c2 = builder.create<arith::ConstantOp>(loc, builder.getI32IntegerAttr(2));
    auto c3 = builder.create<arith::ConstantOp>(loc, builder.getI32IntegerAttr(3));

    auto add = builder.create<arith::AddIOp>(loc, c1, c2);  // 사용됨
    auto mul = builder.create<arith::MulIOp>(loc, c1, c3);  // 미사용!

    builder.create<ReturnOp>(loc, ValueRange{add});

    // Pass 실행
    PassManager pm(&ctx);
    pm.addPass(std::make_unique<ValueTracingPass>());
    if (failed(pm.run(module))) {
        std::cout << "❌ Pass 실행 실패" << std::endl;
        return;
    }
}

int main() {
    std::cout << "╔═══════════════════════════════════════╗" << std::endl;
    std::cout << "║  Exercise 3: Pass 프레임워크          ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════╝" << std::endl;

    try {
        testConstantFolding();
        testDeadCodeElimination();
        testOperationCounting();
        testValueTracing();

        std::cout << "\n╔═══════════════════════════════════════╗" << std::endl;
        std::cout << "║  ✅ 모든 테스트 완료!                  ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════╝" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "오류: " << e.what() << std::endl;
        return 1;
    }
}

/**
 * 학습 포인트:
 *
 * 1. Pass 종류:
 *    - OperationPass: 특정 Operation 타입에 대해 실행
 *    - ModulePass: 전체 Module에 대해 실행
 *    - FunctionPass: 각 함수에 대해 실행
 *
 * 2. Pass 구현:
 *    - runOnOperation()이 핵심 메서드
 *    - getArgument(): Pass의 명령행 이름
 *    - getDescription(): Pass 설명
 *
 * 3. IR 순회:
 *    - walk(): 깊이 우선 탐색 (DFS)
 *    - getBlock(), getRegion(): 구조 접근
 *
 * 4. Operation 조작:
 *    - replaceAllUsesWith(): 사용처 변경
 *    - erase(): Operation 제거
 *    - getUses(), use_empty(): 사용처 확인
 *
 * 5. Pass Manager:
 *    - PassManager로 여러 Pass 순차 실행
 *    - Pass 순서가 중요함 (DCE 전에 상수 폴딩)
 *
 * 6. 분석 Pass:
 *    - 최적화하지 않고 정보만 수집
 *    - 성능 프로파일링, 통계 분석 등에 사용
 *
 * 심화 과제:
 * 1. 루프 불변식 제거 (Loop Invariant Code Motion)
 * 2. 공통 부분식 제거 (Common Subexpression Elimination)
 * 3. 강도 축약 (Strength Reduction)
 * 4. 벡터화 Pass
 * 5. 병렬화 Pass
 * 6. 캐시 최적화 Pass
 */
