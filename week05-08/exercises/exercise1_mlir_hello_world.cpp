/**
 * Exercise 1: MLIR Hello World - Context, Builder, Operations
 *
 * 학습 목표:
 * - MLIR Context 생성 및 관리
 * - MLIRContext 기본 사용법
 * - Operation 빌더 패턴
 * - 간단한 IR 생성 및 출력
 *
 * 난이도: ⭐ 초급 (Junior Level 입문)
 *
 * 주의: 이 코드는 LLVM/MLIR이 설치된 환경에서 컴파일해야 합니다.
 *
 * 빌드 방법:
 * cd mlir-cpp-learning/week05-08
 * mkdir -p build && cd build
 * cmake ..
 * cmake --build .
 * ./exercise1
 */

#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Module.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/FileSystem.h"
#include <iostream>

using namespace mlir;
using namespace mlir::func;

/**
 * 1. 기본 Context와 Module 생성
 */
void testBasicContext() {
    std::cout << "\n=== 테스트 1: 기본 Context 및 Module ===" << std::endl;

    // Context 생성 (MLIR의 모든 작업은 Context 내에서 발생)
    MLIRContext ctx;

    // Func Dialect 로드 (함수 정의를 위해 필요)
    ctx.loadDialect<func::FuncDialect>();

    // Module 생성 (IR의 최상위 컨테이너)
    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    std::cout << "✓ MLIRContext 생성 완료" << std::endl;
    std::cout << "✓ Module 생성 완료" << std::endl;

    // Module 출력
    std::cout << "\n생성된 Module:" << std::endl;
    module.print(llvm::outs());
}

/**
 * 2. 간단한 함수 생성
 */
void testSimpleFunction() {
    std::cout << "\n=== 테스트 2: 간단한 함수 생성 ===" << std::endl;

    MLIRContext ctx;
    ctx.loadDialect<func::FuncDialect>();

    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    // 함수 타입: () -> i32 (인자 없음, i32 반환)
    auto funcType = FunctionType::get(&ctx,
                                      /* inputs */ {},
                                      /* results */ {IntegerType::get(&ctx, 32)});

    // 함수 생성
    FuncOp func = FuncOp::create(loc, "compute", funcType);
    module.body().push_back(func);

    // 함수 본체 생성
    Block &entryBlock = func.addEntryBlock();
    OpBuilder builder(&entryBlock);

    // 함수 본체: return 42
    auto constant = builder.create<arith::ConstantOp>(loc,
                                   builder.getI32IntegerAttr(42));
    builder.create<ReturnOp>(loc, ValueRange{constant});

    std::cout << "✓ compute() 함수 생성 완료" << std::endl;
    std::cout << "✓ 함수 본체: return 42" << std::endl;

    std::cout << "\n생성된 IR:" << std::endl;
    module.print(llvm::outs());
}

/**
 * 3. 여러 Operation으로 구성된 함수
 */
void testMultipleOperations() {
    std::cout << "\n=== 테스트 3: 여러 연산이 포함된 함수 ===" << std::endl;

    MLIRContext ctx;
    ctx.loadDialect<func::FuncDialect>();
    ctx.loadDialect<arith::ArithDialect>();

    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    // 함수 타입: (i32, i32) -> i32
    auto funcType = FunctionType::get(&ctx,
                                      {IntegerType::get(&ctx, 32),
                                       IntegerType::get(&ctx, 32)},
                                      {IntegerType::get(&ctx, 32)});

    FuncOp func = FuncOp::create(loc, "add_numbers", funcType);
    module.body().push_back(func);

    Block &entryBlock = func.addEntryBlock();
    OpBuilder builder(&entryBlock);

    // 함수 인자 가져오기
    Value arg0 = entryBlock.getArgument(0);
    Value arg1 = entryBlock.getArgument(1);

    // 덧셈 연산: result = arg0 + arg1
    auto addOp = builder.create<arith::AddIOp>(loc, arg0, arg1);

    // 결과 반환
    builder.create<ReturnOp>(loc, ValueRange{addOp});

    std::cout << "✓ add_numbers(i32, i32) -> i32 함수 생성 완료" << std::endl;
    std::cout << "✓ 연산: arg0 + arg1" << std::endl;

    std::cout << "\n생성된 IR:" << std::endl;
    module.print(llvm::outs());
}

/**
 * 4. 조건부 분기 (if-else)
 */
void testConditionalBranch() {
    std::cout << "\n=== 테스트 4: 조건부 분기 ===" << std::endl;

    MLIRContext ctx;
    ctx.loadDialect<func::FuncDialect>();
    ctx.loadDialect<arith::ArithDialect>();
    ctx.loadDialect<scf::SCFDialect>();

    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    // 함수: (i32, i32) -> i32
    auto funcType = FunctionType::get(&ctx,
                                      {IntegerType::get(&ctx, 32),
                                       IntegerType::get(&ctx, 32)},
                                      {IntegerType::get(&ctx, 32)});

    FuncOp func = FuncOp::create(loc, "max_value", funcType);
    module.body().push_back(func);

    Block &entryBlock = func.addEntryBlock();
    OpBuilder builder(&entryBlock);

    Value arg0 = entryBlock.getArgument(0);
    Value arg1 = entryBlock.getArgument(1);

    // 조건: arg0 > arg1 인지 확인
    auto cmpOp = builder.create<arith::CmpIOp>(
        loc, arith::CmpIPredicate::sgt, arg0, arg1);

    // if-else: cmp가 true면 arg0, false면 arg1
    auto ifOp = builder.create<scf::IfOp>(loc,
                                          TypeRange{IntegerType::get(&ctx, 32)},
                                          cmpOp,
                                          true);

    // then 블록
    Block &thenBlock = ifOp.getThenRegion().front();
    OpBuilder thenBuilder(&thenBlock);
    thenBuilder.create<scf::YieldOp>(loc, ValueRange{arg0});

    // else 블록
    Block &elseBlock = ifOp.getElseRegion().front();
    OpBuilder elseBuilder(&elseBlock);
    elseBuilder.create<scf::YieldOp>(loc, ValueRange{arg1});

    builder.create<ReturnOp>(loc, ValueRange{ifOp.getResults()[0]});

    std::cout << "✓ max_value(i32, i32) -> i32 함수 생성 완료" << std::endl;
    std::cout << "✓ 연산: if (arg0 > arg1) return arg0 else return arg1" << std::endl;

    std::cout << "\n생성된 IR:" << std::endl;
    module.print(llvm::outs());
}

/**
 * 5. 루프 생성 (for 루프)
 */
void testLoop() {
    std::cout << "\n=== 테스트 5: 루프 (합계 계산) ===" << std::endl;

    MLIRContext ctx;
    ctx.loadDialect<func::FuncDialect>();
    ctx.loadDialect<arith::ArithDialect>();
    ctx.loadDialect<scf::SCFDialect>();

    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    // 함수: () -> i32 (0부터 9까지의 합 계산)
    auto funcType = FunctionType::get(&ctx,
                                      {},
                                      {IntegerType::get(&ctx, 32)});

    FuncOp func = FuncOp::create(loc, "sum_0_to_9", funcType);
    module.body().push_back(func);

    Block &entryBlock = func.addEntryBlock();
    OpBuilder builder(&entryBlock);

    // 루프 범위
    auto c0 = builder.create<arith::ConstantOp>(loc, builder.getI32IntegerAttr(0));
    auto c10 = builder.create<arith::ConstantOp>(loc, builder.getI32IntegerAttr(10));
    auto c1 = builder.create<arith::ConstantOp>(loc, builder.getI32IntegerAttr(1));

    // 초기값: sum = 0
    auto initSum = builder.create<arith::ConstantOp>(loc, builder.getI32IntegerAttr(0));

    // for 루프: i = 0 to 10, step 1
    auto forOp = builder.create<scf::ForOp>(loc,
                                            c0, c10, c1,
                                            ValueRange{initSum});

    // 루프 본체
    Block &loopBlock = forOp.getLoopBody().front();
    OpBuilder loopBuilder(loopBlock.getTerminator());

    Value loopVar = loopBlock.getArgument(0);  // i
    Value accum = loopBlock.getArgument(1);    // 누적 합

    // 합: accum + i
    auto addOp = loopBuilder.create<arith::AddIOp>(loc, accum, loopVar);

    // 다음 반복으로 넘기기
    loopBuilder.create<scf::YieldOp>(loc, ValueRange{addOp});

    builder.create<ReturnOp>(loc, ValueRange{forOp.getResults()[0]});

    std::cout << "✓ sum_0_to_9() 함수 생성 완료" << std::endl;
    std::cout << "✓ 연산: for i = 0 to 10 { sum += i }" << std::endl;

    std::cout << "\n생성된 IR:" << std::endl;
    module.print(llvm::outs());
}

/**
 * 6. 여러 함수를 가진 Module
 */
void testMultipleFunctions() {
    std::cout << "\n=== 테스트 6: 여러 함수를 가진 Module ===" << std::endl;

    MLIRContext ctx;
    ctx.loadDialect<func::FuncDialect>();
    ctx.loadDialect<arith::ArithDialect>();

    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    // 함수 1: square(i32) -> i32
    {
        auto funcType = FunctionType::get(&ctx,
                                          {IntegerType::get(&ctx, 32)},
                                          {IntegerType::get(&ctx, 32)});
        FuncOp func = FuncOp::create(loc, "square", funcType);
        module.body().push_back(func);

        Block &block = func.addEntryBlock();
        OpBuilder builder(&block);
        Value arg = block.getArgument(0);

        // arg * arg
        auto mulOp = builder.create<arith::MulIOp>(loc, arg, arg);
        builder.create<ReturnOp>(loc, ValueRange{mulOp});
    }

    // 함수 2: double(i32) -> i32
    {
        auto funcType = FunctionType::get(&ctx,
                                          {IntegerType::get(&ctx, 32)},
                                          {IntegerType::get(&ctx, 32)});
        FuncOp func = FuncOp::create(loc, "double", funcType);
        module.body().push_back(func);

        Block &block = func.addEntryBlock();
        OpBuilder builder(&block);
        Value arg = block.getArgument(0);

        // arg + arg
        auto addOp = builder.create<arith::AddIOp>(loc, arg, arg);
        builder.create<ReturnOp>(loc, ValueRange{addOp});
    }

    std::cout << "✓ square() 함수 생성 완료" << std::endl;
    std::cout << "✓ double() 함수 생성 완료" << std::endl;

    std::cout << "\n생성된 IR:" << std::endl;
    module.print(llvm::outs());
}

int main() {
    std::cout << "╔═══════════════════════════════════════╗" << std::endl;
    std::cout << "║  Exercise 1: MLIR Hello World         ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════╝" << std::endl;

    try {
        testBasicContext();
        testSimpleFunction();
        testMultipleOperations();
        testConditionalBranch();
        testLoop();
        testMultipleFunctions();

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
 * 1. MLIRContext:
 *    - 모든 MLIR 작업의 중심
 *    - Dialect 로드 (FuncDialect, ArithDialect 등)
 *
 * 2. Location:
 *    - Operation의 위치 정보
 *    - UnknownLoc: 위치 정보 없음
 *
 * 3. Module:
 *    - IR의 최상위 컨테이너
 *    - 함수들을 포함
 *
 * 4. OpBuilder:
 *    - Operation 생성의 편의 도구
 *    - 현재 삽입 위치 관리
 *    - create<OpType>(): 새로운 Operation 생성
 *
 * 5. Block과 Region:
 *    - Block: 순차 실행되는 Operation들의 리스트
 *    - Region: Block들의 컨테이너 (제어 흐름용)
 *
 * 6. FunctionType:
 *    - 함수의 입출력 타입 정의
 *    - 함수 서명 생성
 *
 * 7. Value:
 *    - Operation의 결과값
 *    - 다른 Operation에 입력으로 사용
 *
 * 8. Dialect:
 *    - Operation 집합의 그룹
 *    - Func: 함수 정의
 *    - Arith: 산술 연산
 *    - SCF: 제어 흐름 (if, for 등)
 *
 * 심화 과제:
 * 1. 부동소수점(f32, f64) 연산 추가
 * 2. 벡터(tensor) 연산 추가
 * 3. 사용자 정의 Operation 생성
 * 4. Pass 작성해서 IR 변환
 * 5. 여러 리전(region)을 가진 복잡한 구조
 */
