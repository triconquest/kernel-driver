#pragma once

#include <Windows.h>
#include <iostream>

namespace Driver {
    namespace Codes {
        constexpr ULONG Attach = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

        constexpr ULONG Read = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

        constexpr ULONG Write = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

        constexpr ULONG ResolveModules = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
    }

    struct Request {
        HANDLE processId;

        PVOID target;
        PVOID buffer;

        SIZE_T size;
        SIZE_T returnSize;

        NTSTATUS status;
    };

    inline bool AttachToProcess(HANDLE driverHandle, const DWORD pID) {
        Request request = {};
        request.processId = reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(pID));

        return DeviceIoControl(driverHandle, Codes::Attach, &request, sizeof(request), &request, sizeof(request), nullptr, nullptr) != FALSE;
    }

    inline bool GetModuleBase(HANDLE driverHandle, const DWORD pID, uintptr_t& out) {
        Request request = {};
        request.processId = reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(pID));

        BOOL result = DeviceIoControl(
            driverHandle,
            Codes::ResolveModules,
            &request, sizeof(request),   // input buffer
            &request, sizeof(request),   // output buffer
            nullptr,
            nullptr
        );

        printf("[Kernel ModuleBase] resolveStatus: 0x%08X\n", request.status);

        if (result) {
            out = reinterpret_cast<uintptr_t>(request.buffer);
        }
        else {
            printf("DeviceIoControl failed: %lu\n", GetLastError());
        }

        return result;
    }

    template<typename T>
    bool TryRead(HANDLE driverHandle, const uintptr_t address, T& out) {
        if (driverHandle == nullptr || driverHandle == INVALID_HANDLE_VALUE || address == 0)
            return false;

        Request request = {};
        request.target = reinterpret_cast<PVOID>(address);
        request.buffer = &out;
        request.size = sizeof(T);

        if (!DeviceIoControl(driverHandle, Codes::Read, &request, sizeof(request), &request, sizeof(request), nullptr, nullptr))
            return false;

        return request.returnSize == sizeof(T);
    }

    template<typename T>
    T Read(HANDLE driverHandle, const uintptr_t address) {
        T temp = {};

        if (!TryRead(driverHandle, address, temp))
            return T{};

        return temp;
    }

    template<typename T>
    bool Write(HANDLE driverHandle, const uintptr_t address, const T& value) {
        if (driverHandle == nullptr || driverHandle == INVALID_HANDLE_VALUE || address == 0)
            return false;

        Request request = {};
        request.target = reinterpret_cast<PVOID>(address);
        request.buffer = const_cast<PVOID>(reinterpret_cast<const void*>(&value));
        request.size = sizeof(T);

        if (!DeviceIoControl(driverHandle, Codes::Write, &request, sizeof(request), &request, sizeof(request), nullptr, nullptr))
            return false;

        return request.returnSize == sizeof(T);
    }
}
