#include "pch.h"
#include "driver.h"

bool driver::get_current_process_name(HANDLE proc, PUNICODE_STRING* name) {
  *name = (PUNICODE_STRING)ALLOCATE(512);
  if (*name != NULL) {
    if (NT_SUCCESS(g_driver.ZwQueryInformationProcess(proc, ProcessImageFileName, (PROCESS_BASIC_INFORMATION*)*name, 512, nullptr))) {
      return true;
    }
  }

  ExFreePoolWithTag(name, (ULONG)'BEBE');
  return false;
}