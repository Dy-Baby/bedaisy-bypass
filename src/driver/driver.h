#pragma once
#include "pch.h"
#include "util/util.h"

enum eDriverEntryStatus : NTSTATUS {
  DriverEntrySuccess = 1,
  DriverEntryFailedResolvingFunctions,
  DriverEntryInvalidKernelBase,
  DriverEntryFailedKiSystemServiceTable,
  DriverEntryFailedSyscallTable,
};

template<typename R, typename T>
struct exported_function {
  T(*m_function);

  exported_function& operator=(uint64_t address) {
    m_function = (decltype(m_function))address;
    return *this;
  }

  bool operator==(uint64_t address) {
    return (uint64_t)m_function == address;
  }

  template<typename... Args>
  R operator()(Args... args) const {
    return m_function(static_cast<Args>(args)...);
  };
};

struct driver {
  pair<uint64_t, uint32_t> m_syscall_table_range {0, 0};
  HANDLE m_battleye {};

  // kernel
  exported_function<NTSTATUS, NTSTATUS(int, PVOID, ULONG, PULONG)> ZwQuerySystemInformation;
  exported_function<NTSTATUS, NTSTATUS(HANDLE, PROCESSINFOCLASS, PROCESS_BASIC_INFORMATION*, ULONG, PULONG)> ZwQueryInformationProcess;

  // syscall table
  exported_function<NTSTATUS, NTSTATUS(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PIO_STATUS_BLOCK, PLARGE_INTEGER, ULONG, ULONG, ULONG, ULONG, PVOID, ULONG)> NtCreateFile;
  exported_function<NTSTATUS, NTSTATUS(HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, PIO_STATUS_BLOCK, PVOID, ULONG, PLARGE_INTEGER, PULONG)> NtReadFile;

  bool get_current_process_name(HANDLE proc, PUNICODE_STRING* name);
};

inline driver g_driver {};

#define RESOLVE(var) \
  g_driver.var = util::resolve(L#var); \
  if (g_driver.var == 0) \
    return DriverEntryFailedResolvingFunctions;

#define RESOLVE_SYSCALL(var) \
  g_driver.var = util::resolve_syscall(g_driver.m_syscall_table_range, #var); \
  if (g_driver.var == 0) \
    return DriverEntryFailedResolvingFunctions;