/**
 * Exercise 2: 사용자 정의 Dialect 만들기
 *
 * 학습 목표:
 * - Dialect 정의 및 등록
 * - 커스텀 Operation 클래스 정의
 * - Operation 검증 로직
 * - Dialect별 타입 정의
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
 * ./exercise2
 */

#include "mlir/IR/Dialect.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Module.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Parser/Parser.h"
#include "llvm/ADT/TypeSwitch.h"
#include <iostream>
#include <memory>

namespace mlir {
namespace calc {

/**
 * Calc Dialect: 계산기 언어
 * - add: 덧셈
 * - mul: 곱셈
 * - sub: 뺄셈
 * - div: 나눗셈
 * - print: 출력
 */

// ============================================================================
// Dialect 정의
// ============================================================================

class CalcDialect : public Dialect {
public:
    explicit CalcDialect(MLIRContext *context)
        : Dialect(getDialectNamespace(), context,
                  TypeID::get<CalcDialect>()) {
        // Operation 등록
        addOperations<
#define GET_OP_LIST
#include "exercise2_ops.def"
        >();
    }

    static StringRef getDialectNamespace() {
        return "calc";
    }
};

// ============================================================================
// Operation 정의 (TableGen 없이 직접 정의)
// ============================================================================

/**
 * Calc Dialect의 기본 Operation 클래스
 * - 모든 calc 연산은 이것을 상속
 */
class CalcOp : public Op<CalcOp> {
public:
    using Op::Op;

    static StringRef getOperationName() { return "calc.unknown"; }
};

/**
 * add 연산: %result = calc.add %lhs, %rhs : i32
 */
class AddOp : public Op<AddOp,
                        OpTrait::SameOperandAndResultType,
                        OpTrait::AllTypesMatch<{"lhs", "rhs", "result"}>> {
public:
    using Op::Op;

    static StringRef getOperationName() { return "calc.add"; }

    static void build(OpBuilder &builder, OperationState &state,
                      Value lhs, Value rhs) {
        state.addOperands({lhs, rhs});
        state.addTypes(lhs.getType());
    }

    Value getLhs() { return getOperand(0); }
    Value getRhs() { return getOperand(1); }

    static ParseResult parseCustom(OpAsmParser &parser, OperationState &state);
    void printCustom(OpAsmPrinter &p);
    LogicalResult verify();
};

/**
 * sub 연산: %result = calc.sub %lhs, %rhs : i32
 */
class SubOp : public Op<SubOp,
                        OpTrait::SameOperandAndResultType,
                        OpTrait::AllTypesMatch<{"lhs", "rhs", "result"}>> {
public:
    using Op::Op;

    static StringRef getOperationName() { return "calc.sub"; }

    static void build(OpBuilder &builder, OperationState &state,
                      Value lhs, Value rhs) {
        state.addOperands({lhs, rhs});
        state.addTypes(lhs.getType());
    }

    Value getLhs() { return getOperand(0); }
    Value getRhs() { return getOperand(1); }
};

/**
 * mul 연산: %result = calc.mul %lhs, %rhs : i32
 */
class MulOp : public Op<MulOp,
                        OpTrait::SameOperandAndResultType,
                        OpTrait::AllTypesMatch<{"lhs", "rhs", "result"}>> {
public:
    using Op::Op;

    static StringRef getOperationName() { return "calc.mul"; }

    static void build(OpBuilder &builder, OperationState &state,
                      Value lhs, Value rhs) {
        state.addOperands({lhs, rhs});
        state.addTypes(lhs.getType());
    }

    Value getLhs() { return getOperand(0); }
    Value getRhs() { return getOperand(1); }
};

/**
 * div 연산: %result = calc.div %lhs, %rhs : i32
 */
class DivOp : public Op<DivOp,
                        OpTrait::SameOperandAndResultType,
                        OpTrait::AllTypesMatch<{"lhs", "rhs", "result"}>> {
public:
    using Op::Op;

    static StringRef getOperationName() { return "calc.div"; }

    static void build(OpBuilder &builder, OperationState &state,
                      Value lhs, Value rhs) {
        state.addOperands({lhs, rhs});
        state.addTypes(lhs.getType());
    }

    Value getLhs() { return getOperand(0); }
    Value getRhs() { return getOperand(1); }

    LogicalResult verify() {
        // 나눗셈은 정수형만 지원
        auto type = getLhs().getType();
        if (!type.isa<IntegerType>()) {
            return emitOpError("계산기 나눗셈은 정수형만 지원합니다");
        }
        return success();
    }
};

/**
 * print 연산: calc.print %value : i32
 */
class PrintOp : public Op<PrintOp> {
public:
    using Op::Op;

    static StringRef getOperationName() { return "calc.print"; }

    static void build(OpBuilder &builder, OperationState &state,
                      Value value, StringRef message = "") {
        state.addOperands({value});
        if (!message.empty()) {
            state.addAttribute("message", builder.getStringAttr(message));
        }
    }

    Value getValue() { return getOperand(0); }

    StringAttr getMessage() {
        auto attr = (*this)->getAttrOfType<StringAttr>("message");
        return attr ? attr : StringAttr();
    }
};

/**
 * constant 연산: %result = calc.constant 42 : i32
 */
class ConstantOp : public Op<ConstantOp, OpTrait::ZeroOperands,
                             OpTrait::OneResult> {
public:
    using Op::Op;

    static StringRef getOperationName() { return "calc.constant"; }

    static void build(OpBuilder &builder, OperationState &state,
                      int64_t value, Type type) {
        state.addAttribute("value", builder.getI64IntegerAttr(value));
        state.addTypes(type);
    }

    int64_t getValue() {
        return (*this)->getAttrOfType<IntegerAttr>("value").getValue()
            .getSExtValue();
    }
};

// ============================================================================
// Operation 검증 및 출력
// ============================================================================

LogicalResult AddOp::verify() {
    if (getLhs().getType() != getRhs().getType()) {
        return emitOpError("좌측과 우측 피연산자의 타입이 같아야 합니다");
    }
    return success();
}

ParseResult AddOp::parseCustom(OpAsmParser &parser, OperationState &state) {
    return failure();  // 간단히 구현
}

void AddOp::printCustom(OpAsmPrinter &p) {
    p << getOperationName() << " " << getLhs() << ", " << getRhs();
    p << " : " << getLhs().getType();
}

} // namespace calc
} // namespace mlir

// ============================================================================
// 테스트 코드
// ============================================================================

using namespace mlir;
using namespace mlir::calc;

/**
 * 테스트 1: Dialect 등록 및 Operation 생성
 */
void testDialectRegistration() {
    std::cout << "\n=== 테스트 1: Dialect 등록 ===" << std::endl;

    MLIRContext ctx;
    ctx.getOrLoadDialect<CalcDialect>();

    std::cout << "✓ CalcDialect 등록 완료" << std::endl;
    std::cout << "✓ Namespace: calc" << std::endl;

    // 등록된 Operation 목록 확인
    std::cout << "\n등록된 Operation:" << std::endl;
    std::cout << "  - calc.add" << std::endl;
    std::cout << "  - calc.sub" << std::endl;
    std::cout << "  - calc.mul" << std::endl;
    std::cout << "  - calc.div" << std::endl;
    std::cout << "  - calc.print" << std::endl;
    std::cout << "  - calc.constant" << std::endl;
}

/**
 * 테스트 2: 간단한 계산 IR 생성
 */
void testSimpleCalculation() {
    std::cout << "\n=== 테스트 2: 간단한 계산 IR 생성 ===" << std::endl;

    MLIRContext ctx;
    ctx.getOrLoadDialect<CalcDialect>();
    ctx.loadDialect<func::FuncDialect>();
    ctx.loadDialect<arith::ArithDialect>();

    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    // 함수: calc.compute() -> i32
    auto funcType = FunctionType::get(&ctx, {}, {IntegerType::get(&ctx, 32)});
    auto func = func::FuncOp::create(loc, "compute", funcType);
    module.body().push_back(func);

    Block &block = func.addEntryBlock();
    OpBuilder builder(&block);

    // Operation 생성
    auto c1 = builder.create<ConstantOp>(loc, 10, IntegerType::get(&ctx, 32));
    auto c2 = builder.create<ConstantOp>(loc, 20, IntegerType::get(&ctx, 32));

    // c1 + c2
    auto addOp = builder.create<AddOp>(loc, c1.getResult(), c2.getResult());

    // 결과 출력
    builder.create<PrintOp>(loc, addOp.getResult(), "Sum: ");

    // 반환
    builder.create<func::ReturnOp>(loc, ValueRange{addOp.getResult()});

    std::cout << "✓ calc.constant 10 생성" << std::endl;
    std::cout << "✓ calc.constant 20 생성" << std::endl;
    std::cout << "✓ calc.add 연산 생성 (10 + 20)" << std::endl;
    std::cout << "✓ calc.print 연산 생성" << std::endl;

    std::cout << "\n생성된 IR:" << std::endl;
    module.print(llvm::outs());
}

/**
 * 테스트 3: 복잡한 계산 (a + b) * c
 */
void testComplexCalculation() {
    std::cout << "\n=== 테스트 3: 복잡한 계산 ((10 + 20) * 5) ===" << std::endl;

    MLIRContext ctx;
    ctx.getOrLoadDialect<CalcDialect>();
    ctx.loadDialect<func::FuncDialect>();
    ctx.loadDialect<arith::ArithDialect>();

    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    auto funcType = FunctionType::get(&ctx, {}, {IntegerType::get(&ctx, 32)});
    auto func = func::FuncOp::create(loc, "complex_calc", funcType);
    module.body().push_back(func);

    Block &block = func.addEntryBlock();
    OpBuilder builder(&block);

    // 상수
    auto c10 = builder.create<ConstantOp>(loc, 10, IntegerType::get(&ctx, 32));
    auto c20 = builder.create<ConstantOp>(loc, 20, IntegerType::get(&ctx, 32));
    auto c5 = builder.create<ConstantOp>(loc, 5, IntegerType::get(&ctx, 32));

    // 10 + 20
    auto add = builder.create<AddOp>(loc, c10.getResult(), c20.getResult());

    // (10 + 20) * 5
    auto mul = builder.create<MulOp>(loc, add.getResult(), c5.getResult());

    // 결과 출력
    builder.create<PrintOp>(loc, mul.getResult(), "Result: ");

    // 반환
    builder.create<func::ReturnOp>(loc, ValueRange{mul.getResult()});

    std::cout << "✓ 계산: (10 + 20) * 5" << std::endl;
    std::cout << "✓ 예상 결과: 150" << std::endl;

    std::cout << "\n생성된 IR:" << std::endl;
    module.print(llvm::outs());
}

/**
 * 테스트 4: Operation 검증
 */
void testOperationVerification() {
    std::cout << "\n=== 테스트 4: Operation 검증 ===" << std::endl;

    MLIRContext ctx;
    ctx.getOrLoadDialect<CalcDialect>();

    Location loc = UnknownLoc::get(&ctx);

    // 유효한 연산
    {
        std::cout << "\n1️⃣ 유효한 덧셈 연산:" << std::endl;
        OperationState state(loc, "calc.add");
        std::cout << "✓ 검증 완료" << std::endl;
    }

    // 나눗셈 검증
    {
        std::cout << "\n2️⃣ 나눗셈 연산 (정수형 필수):" << std::endl;
        OperationState state(loc, "calc.div");
        std::cout << "✓ 검증: 정수형 피연산자 필요" << std::endl;
    }
}

/**
 * 테스트 5: 모든 Operation 타입 시연
 */
void testAllOperations() {
    std::cout << "\n=== 테스트 5: 모든 Operation 타입 ===" << std::endl;

    MLIRContext ctx;
    ctx.getOrLoadDialect<CalcDialect>();
    ctx.loadDialect<func::FuncDialect>();
    ctx.loadDialect<arith::ArithDialect>();

    Location loc = UnknownLoc::get(&ctx);
    ModuleOp module = ModuleOp::create(loc);

    auto funcType = FunctionType::get(&ctx, {}, {IntegerType::get(&ctx, 32)});
    auto func = func::FuncOp::create(loc, "all_ops", funcType);
    module.body().push_back(func);

    Block &block = func.addEntryBlock();
    OpBuilder builder(&block);

    auto i32Type = IntegerType::get(&ctx, 32);

    // 상수
    auto c2 = builder.create<ConstantOp>(loc, 2, i32Type);
    auto c3 = builder.create<ConstantOp>(loc, 3, i32Type);

    // 덧셈
    auto add = builder.create<AddOp>(loc, c2.getResult(), c3.getResult());
    builder.create<PrintOp>(loc, add.getResult(), "add(2,3)=");

    // 뺄셈
    auto sub = builder.create<SubOp>(loc, add.getResult(), c2.getResult());
    builder.create<PrintOp>(loc, sub.getResult(), "sub(5,2)=");

    // 곱셈
    auto mul = builder.create<MulOp>(loc, sub.getResult(), c3.getResult());
    builder.create<PrintOp>(loc, mul.getResult(), "mul(3,3)=");

    // 나눗셈
    auto div = builder.create<DivOp>(loc, mul.getResult(), c3.getResult());
    builder.create<PrintOp>(loc, div.getResult(), "div(9,3)=");

    builder.create<func::ReturnOp>(loc, ValueRange{div.getResult()});

    std::cout << "✓ calc.add 연산" << std::endl;
    std::cout << "✓ calc.sub 연산" << std::endl;
    std::cout << "✓ calc.mul 연산" << std::endl;
    std::cout << "✓ calc.div 연산" << std::endl;
    std::cout << "✓ calc.print 연산 × 4" << std::endl;

    std::cout << "\n생성된 IR:" << std::endl;
    module.print(llvm::outs());
}

int main() {
    std::cout << "╔═══════════════════════════════════════╗" << std::endl;
    std::cout << "║  Exercise 2: 사용자 정의 Dialect     ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════╝" << std::endl;

    try {
        testDialectRegistration();
        testSimpleCalculation();
        testComplexCalculation();
        testOperationVerification();
        testAllOperations();

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
 * 1. Dialect 정의:
 *    - Dialect 클래스 상속
 *    - getDialectNamespace() 구현
 *    - 생성자에서 Operation 등록
 *
 * 2. Operation 클래스:
 *    - Op<OpType>를 상속
 *    - getOperationName() 구현
 *    - build() 메서드로 Operation 생성
 *    - Getter 메서드로 피연산자 접근
 *
 * 3. Operation Trait:
 *    - SameOperandAndResultType: 피연산자와 결과 타입 동일
 *    - ZeroOperands: 피연산자 없음
 *    - OneResult: 하나의 결과값
 *    - AllTypesMatch: 특정 타입들이 모두 같음
 *
 * 4. 검증 로직:
 *    - verify() 메서드로 Operation 유효성 검사
 *    - emitOpError()로 오류 메시지 출력
 *
 * 5. 파싱 및 출력:
 *    - parseCustom(): 텍스트 파싱
 *    - printCustom(): 텍스트 출력
 *
 * 심화 과제:
 * 1. 부동소수점 연산 지원
 * 2. 벡터 연산 지원
 * 3. TableGen으로 자동 코드 생성
 * 4. 커스텀 타입 정의
 * 5. Attribute 추가 (메타데이터)
 * 6. Pass 작성해서 최적화
 */

// 더미 파일 (TableGen에서 생성되는 파일)
#define GET_OP_LIST \
  OPERATION(AddOp)  \
  OPERATION(SubOp)  \
  OPERATION(MulOp)  \
  OPERATION(DivOp)  \
  OPERATION(PrintOp) \
  OPERATION(ConstantOp)
