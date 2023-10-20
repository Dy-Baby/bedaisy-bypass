#pragma once
#include "pch.h"

template<typename T1, typename T2>
struct pair {
  T1 first;
  T2 second;
};

struct syscall_table {
  uint32_t m_hash {0};
  uint32_t m_arg_count {0};
  uint64_t m_function {0};
};

struct system_module {
  char _0x0000[0x10];
	PVOID m_image_base;
	ULONG m_image_size;
  char _0x001C[0x10C];
};

struct system_module_information {
	ULONG m_module_count;
	system_module m_modules[1];
};

namespace util {
  PVOID allocate_memory(SIZE_T size);
  uint64_t resolve(const wchar_t* str);
  uint64_t resolve_syscall(pair<uint64_t, uint32_t> range, const char* str);
  uint64_t find_pattern(uint8_t* pattern, uint32_t length, uint64_t base, uint32_t size);
  uint64_t read_instruction(uint64_t address, int size, int length);
};