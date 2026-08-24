#pragma once
#include <ntifs.h>

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation,
    SystemProcessorInformation,
    SystemPerformanceInformation,
    SystemTimeOfDayInformation,
    SystemPathInformation,
    SystemProcessInformation,
    SystemCallCountInformation,
    SystemDeviceInformation,
    SystemProcessorPerformanceInformation,
    SystemFlagsInformation,
    SystemCallTimeInformation,
    SystemModuleInformation = 0x0B
} SYSTEM_INFORMATION_CLASS;

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
    ULONG Section;
    PVOID MappedBase;
    PVOID ImageBase;
    ULONG ImageSize;
    ULONG Flags;
    USHORT LoadOrderIndex;
    USHORT InitOrderIndex;
    USHORT LoadCount;
    USHORT OffsetToFileName;
    CHAR FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
    ULONG NumberOfModules;
    RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;

extern "C" __declspec(dllimport)
NTSTATUS NTAPI ZwProtectVirtualMemory(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    PULONG ProtectSize,
    ULONG NewProtect,
    PULONG OldProtect
);

extern "C" NTKERNELAPI 
PVOID NTAPI RtlFindExportedRoutineByName(_In_ PVOID ImageBase, _In_ PCCH RoutineName);

extern "C" NTSTATUS ZwQuerySystemInformation(ULONG InfoClass, PVOID Buffer, ULONG Length, PULONG ReturnLength);

extern "C" NTKERNELAPI
PPEB PsGetProcessPeb(IN PEPROCESS Process);

extern "C" {
    NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);
    
    NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);

    NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(PEPROCESS Process);
}

typedef PVOID(NTAPI* fnPsGetProcessSectionBaseAddress)(PEPROCESS Process);

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

    inline NTSTATUS Create(PDEVICE_OBJECT deviceObject, PIRP irp) {
        UNREFERENCED_PARAMETER(deviceObject);

        IoCompleteRequest(irp, IO_NO_INCREMENT);

        return irp->IoStatus.Status;
    }

    inline NTSTATUS Close(PDEVICE_OBJECT deviceObject, PIRP irp) {
        UNREFERENCED_PARAMETER(deviceObject);

        IoCompleteRequest(irp, IO_NO_INCREMENT);

        return irp->IoStatus.Status;
    }

    inline NTSTATUS DeviceControl(PDEVICE_OBJECT deviceObject, PIRP irp) {
        UNREFERENCED_PARAMETER(deviceObject);

        //DebugPrint("[+] Device control called\n");

        NTSTATUS status = STATUS_UNSUCCESSFUL;

        PIO_STACK_LOCATION stackIRP = IoGetCurrentIrpStackLocation(irp);

        auto request = reinterpret_cast<Request*>(irp->AssociatedIrp.SystemBuffer);

        if (stackIRP == nullptr || request == nullptr) {
            irp->IoStatus.Status = status;
            irp->IoStatus.Information = sizeof(Request);

            IoCompleteRequest(irp, IO_NO_INCREMENT);
            return status;
        }

        static PEPROCESS targetProcess = nullptr;

        const ULONG controlCode = stackIRP->Parameters.DeviceIoControl.IoControlCode;

        switch (controlCode) {
        case Codes::Attach:
            status = PsLookupProcessByProcessId(request->processId, &targetProcess);
            break;

        case Codes::Read:
            if (targetProcess != nullptr)
                status = MmCopyVirtualMemory(targetProcess, request->target, PsGetCurrentProcess(), request->buffer, request->size, KernelMode, &request->returnSize);
            break;

        case Codes::Write:
            if (targetProcess != nullptr)
                status = MmCopyVirtualMemory(PsGetCurrentProcess(), request->buffer, targetProcess, request->target, request->size, KernelMode, &request->returnSize);
            break;

        case Codes::ResolveModules: {
            PEPROCESS process = NULL;

            KdPrintEx((
                DPFLTR_IHVDRIVER_ID,
                DPFLTR_INFO_LEVEL,
                "[ResolveModules] PID=%llu\n",
                (ULONG_PTR)request->processId
                ));

            NTSTATUS resolveStatus = PsLookupProcessByProcessId(request->processId, &process);

            KdPrintEx((
                DPFLTR_IHVDRIVER_ID,
                DPFLTR_INFO_LEVEL,
                "[ResolveModules] Lookup status: 0x%08X\n",
                resolveStatus
                ));

            request->status = resolveStatus;

            if (NT_SUCCESS(resolveStatus)) {
                PVOID imageBase = PsGetProcessSectionBaseAddress(process);

                KdPrintEx((
                    DPFLTR_IHVDRIVER_ID,
                    DPFLTR_INFO_LEVEL,
                    "[ResolveModules] Image base: %p\n",
                    imageBase
                    ));

                request->buffer = imageBase;
                status = request->buffer ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;

                ObDereferenceObject(process);
            }
            else {
                status = resolveStatus;
            }

            break;
        }

        default:
            break;
        }

        irp->IoStatus.Status = status;
        irp->IoStatus.Information = sizeof(Request);

        IoCompleteRequest(irp, IO_NO_INCREMENT);

        return status;
    }
}