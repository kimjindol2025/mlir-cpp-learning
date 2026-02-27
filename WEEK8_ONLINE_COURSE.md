# 🎓 Week 8: 통합 프로젝트 - SimpleDSL 완성 (온라인 혼자 학습용)

**목표**: Dialect + Pass를 모두 통합한 완성도 높은 컴파일러

---

## 🎯 최종 프로젝트 구조

```
SimpleDSL 컴파일러
├─ Dialect: 6개 Operation
│  ├─ MatMul, Add, ReLU, Pool, Reshape, Fused
│  └─ 커스텀 Type
│
├─ Pass: 4개 최적화
│  ├─ ConstantFolding
│  ├─ MatMulFusion
│  ├─ DeadCodeElimination
│  └─ MemoryOptimization
│
└─ 테스트 & 벤치마킹
   ├─ 10+ 통합 테스트
   └─ 성능 비교
```

## 통합 실행 흐름

```cpp
// 1. IR 로드 또는 생성
auto module = parseMLIR(input);  // 또는 Builder로 생성

// 2. Pass Manager 생성
PassManager pm(&ctx);

// 3. Pass 추가 (순서 중요!)
pm.addPass(createConstantFoldingPass());
pm.addPass(createMatMulFusionPass());
pm.addPass(createDeadCodeEliminationPass());

// 4. Pass 실행
if (failed(pm.run(module))) {
    llvm::errs() << "Pass failed!\n";
    return 1;
}

// 5. 최적화된 IR 출력
module.print(llvm::outs());
```

## ✅ **Week 8 체크리스트**

- [ ] SimpleDSL Dialect 완성 (6개 Operation)
- [ ] 4개 최적화 Pass 모두 작동
- [ ] 10+ 통합 테스트 작성
- [ ] 성능 벤치마크 측정
- [ ] 최적화 효과 검증
- [ ] 완전한 컴파일러 파이프라인 동작

---

## 🏆 **Junior Level 완료!**

✅ MLIR 기초 이해
✅ Dialect 설계 및 구현
✅ Pass 프레임워크 마스터
✅ 완전한 컴파일러 구축

---

**다음**: Week 9 - Algorithm Porting (University Level)
