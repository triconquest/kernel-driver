#pragma once
#include "../Utils/Memory.h"

namespace Hook {
	
	bool CallKernelFunction(void* kernelFunctionAddress);
	NTSTATUS HookHandler(PVOID calledParam);
}