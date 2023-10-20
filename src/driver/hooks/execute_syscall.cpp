#include "hooks.h"
#include "driver/driver.h"

// function to pass to infinity hook
void hooks::execute_syscall(uint32_t index, void** function) {
  UNREFERENCED_PARAMETER(index);
  
  if (g_driver.NtCreateFile == (uint64_t)*function)
    *function = nt_create_file;
  
  if (g_driver.NtReadFile == (uint64_t)*function)
    *function = nt_read_file;
}