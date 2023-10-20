#include "hooks.h"
#include "driver/driver.h"
#include "driver/anticheat/battleye.h"

NTSTATUS hooks::nt_read_file(HANDLE file_handle, HANDLE event, PIO_APC_ROUTINE apc_routine, PVOID apc_context, PIO_STATUS_BLOCK io_status_block, PVOID buffer, ULONG length, PLARGE_INTEGER byte_offset, PULONG key) {
  int battleye_key = 0;

  if (file_handle == g_driver.m_battleye && g_driver.m_battleye > 0 && buffer) {
    if (length > 0x04) {
      battleye_key = *(int*)buffer;
    }
  }
  
  NTSTATUS status = g_driver.NtReadFile(file_handle, event, apc_routine, apc_context, io_status_block, buffer, length, byte_offset, key);

  if (!NT_SUCCESS(status))
    return status;

  if (battleye_key > 0) {
    uint8_t* buffer_bytes = (uint8_t*)buffer;
    battleye::handle_packet(buffer_bytes, (uint32_t)length, battleye_key);
  }

  return status;
}