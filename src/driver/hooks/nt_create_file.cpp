#include "hooks.h"
#include "driver/driver.h"

NTSTATUS hooks::nt_create_file(PHANDLE file_handle, ACCESS_MASK desired_access, POBJECT_ATTRIBUTES object_attributes, PIO_STATUS_BLOCK io_status_block, PLARGE_INTEGER allocation_size, ULONG file_attributes, ULONG share_access, ULONG create_disposition, ULONG create_options, PVOID ea_buffer, ULONG ea_length) {
  NTSTATUS status = g_driver.NtCreateFile(file_handle, desired_access, object_attributes, io_status_block, allocation_size, file_attributes, share_access, create_disposition, create_options, ea_buffer, ea_length);
  if (!NT_SUCCESS(status))
    return status;

  if (object_attributes && object_attributes->ObjectName && object_attributes->ObjectName->Buffer) {
    if (wcsstr(object_attributes->ObjectName->Buffer, L"\\??\\BattlEye"))
      g_driver.m_battleye = *file_handle;
  }

  return status; 
}