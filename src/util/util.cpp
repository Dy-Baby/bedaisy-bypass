#include "pch.h"
#include "util.h"

namespace util {
  uint64_t resolve(const wchar_t* str) {
		UNICODE_STRING query_info = RTL_CONSTANT_STRING(str);
		RtlInitUnicodeString(&query_info, str);
		return (uint64_t)MmGetSystemRoutineAddress(&query_info);
	}

  uint32_t hash_syscall_name(const char* name) {
    char* pname = (char*)name + 2;
    uint32_t hash = 0;

    while (*pname) {
      hash = ((1025 * (hash + (char)*pname)) >> 6) ^ (1025 * (hash + (char)*pname));
      ++pname;
    }

    return hash;
  }

  uint64_t resolve_syscall(pair<uint64_t, uint32_t> range, const char* str) {
    uint32_t hash = hash_syscall_name(str);

    syscall_table* table = (syscall_table*)range.first;
    for (uint32_t i = 0; i < range.second; i++) {
      if (table[i].m_hash == hash)
        return table[i].m_function;
    }

    return 0;
  }

  PVOID allocate_memory(SIZE_T size) {
    return ExAllocatePoolWithTag(NonPagedPool, size, (ULONG)'BEBE');
  }

  uint64_t read_instruction(uint64_t address, int size, int length) {
    return (uint64_t)(*(int*)(address + size) + address + length);
  }

  bool data_compare(const uint8_t* data, const uint8_t* mask, const char* maskstr) {
    for (; *maskstr; ++maskstr, ++data, ++mask)
      if (*maskstr == 'x' && *data != *mask)
        return 0;
    return (*maskstr) == 0;
  }

  uint64_t find_pattern(uint8_t* pattern, uint32_t length, uint64_t base, uint32_t size) {
    if (!pattern || length == 0)
      return 0;
    
    char* mask = (char*)ALLOCATE(length + 1);
    if (!mask)
      return 0;

    memset(mask, 0, length + 1);

    for (uint32_t i = 0; i < length; i++)
      mask[i] = pattern[i] == 0xCC ? '?' : 'x';

    for (uint64_t i = 0; i < size; i++) {
      if (!MmIsAddressValid((uint8_t*)(base + i)))
        continue;

      if (!data_compare((uint8_t*)(base + i), pattern, mask))
        continue;

      ExFreePoolWithTag(mask, (ULONG)'BEBE');
      return (uint64_t)(base + i);
    }

    ExFreePoolWithTag(mask, (ULONG)'BEBE');
    return 0;
  }
}