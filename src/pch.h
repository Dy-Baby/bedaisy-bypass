#pragma once
#define _AMD64_
#define _WIN64
#define AMD64
#define _KERNEL_MODE

#pragma warning( disable : 4996 )

#include <ntifs.h>
#include <ntddk.h>
#include <windef.h>
#include <ntdef.h>
#include <strsafe.h>
#include <ntdddisk.h>
#include <ntimage.h>
#include <wdm.h>
#include <ntddscsi.h>
#include <ata.h>
#include <scsi.h>
#include <ntddndis.h>
#include <mountmgr.h>
#include <mountdev.h>
#include <classpnp.h>

extern "C" int _fltused = 1;

typedef UINT64 uint64_t;
typedef UINT8 uint8_t;
typedef UINT32 uint32_t;
typedef UINT16 uint16_t;

#define LOG(x, ...) DbgPrintEx(0, 0, x, __VA_ARGS__);

#define ALLOCATE(size) \
  util::allocate_memory((SIZE_T)size)