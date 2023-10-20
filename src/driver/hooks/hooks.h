#pragma once
#include "pch.h"

namespace hooks {
  void execute_syscall(uint32_t index, void** function);
  
  NTSTATUS nt_create_file(PHANDLE file_handle, ACCESS_MASK desired_access, POBJECT_ATTRIBUTES object_attributes, PIO_STATUS_BLOCK io_status_block, PLARGE_INTEGER allocation_size, ULONG file_attributes, ULONG share_access, ULONG create_disposition, ULONG create_options, PVOID ea_buffer, ULONG ea_length);
  NTSTATUS nt_read_file(HANDLE file_handle, HANDLE event, PIO_APC_ROUTINE apc_routine, PVOID apc_context, PIO_STATUS_BLOCK io_status_block, PVOID buffer, ULONG length, PLARGE_INTEGER byte_offset, PULONG key);
};