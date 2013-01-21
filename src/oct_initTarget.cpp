#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/Support/TargetSelect.h"

extern "C"
void oct_initJITTarget() {
    llvm::InitializeNativeTarget();
}
