#include "pch.h"
#include "driver/driver.h"
#include "driver/hooks/hooks.h"
#include "util/util.h"

uint8_t KiSystemServiceTable[] = {
	0x4C, 0x8D, 0x3D, 0xCC, 0xCC, 0xCC, 0xCC, 0x41, 0xBC, 0xCC, 0xCC, 0xCC,
	0xCC, 0x4C, 0x8D, 0x77, 0x28, 0x41, 0x8B
};

uint8_t KiSystemServiceTable_Win11_22H2[] = {
  0x4C, 0x8D, 0x35, 0xCC, 0xCC, 0xCC, 0xCC, 0x41, 0xBC, 0xCC, 0xCC, 0xCC, 
  0xCC, 0x48, 0x8D, 0x70, 0x28, 0x41, 0x8B, 0x4E, 0xF8, 0x89, 0x4E, 0x0C, 
  0x4D, 0x8B, 0x3E
};

NTSTATUS driver_entry(_DRIVER_OBJECT* driver_object, PUNICODE_STRING registry_path) {
  UNREFERENCED_PARAMETER(driver_object);
  UNREFERENCED_PARAMETER(registry_path);

  // resolve functions
  RESOLVE(ZwQuerySystemInformation);
  RESOLVE(ZwQueryInformationProcess);

  system_module_information system_module_information;
  memset(&system_module_information, 0, sizeof(system_module_information));

  ULONG return_length {0};
  g_driver.ZwQuerySystemInformation(11, &system_module_information, (ULONG)sizeof(system_module_information), &return_length);
  if (!system_module_information.m_modules[0].m_image_base)
    return DriverEntryInvalidKernelBase;

  pair<uint64_t, uint32_t> kernel_range = { (uint64_t)system_module_information.m_modules[0].m_image_base, (uint32_t)system_module_information.m_modules[0].m_image_size };

  uint64_t ki_system_service_table = util::find_pattern(KiSystemServiceTable, sizeof(KiSystemServiceTable), kernel_range.first, kernel_range.second);
  if (!ki_system_service_table) {
    ki_system_service_table = util::find_pattern(KiSystemServiceTable_Win11_22H2, sizeof(KiSystemServiceTable_Win11_22H2), kernel_range.first, kernel_range.second);
    if (!ki_system_service_table)
      return DriverEntryFailedKiSystemServiceTable;
  }

  g_driver.m_syscall_table_range = {
    util::read_instruction(ki_system_service_table, 3, 7),
    *(uint32_t*)(ki_system_service_table + 0x9)
  };

  if (!g_driver.m_syscall_table_range.first)
    return DriverEntryFailedSyscallTable;

  g_driver.m_syscall_table_range.first -= 0x8;

  // resolve syscalls from the syscall table
  RESOLVE_SYSCALL(NtCreateFile);
  RESOLVE_SYSCALL(NtReadFile);

  // setup infinity hook here, pointing to `hooks::execute_syscall` :)

  return DriverEntrySuccess;
}