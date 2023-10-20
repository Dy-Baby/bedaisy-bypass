# bedaisy-bypass

## Description

`bedaisy-bypass` is a kernel driver designed to bypass reports sent from `BEDaisy.sys` to `BEService.exe`. For users not on a UEFI, the project utilizes Infinity Hook. The driver intercepts data communicated through the named pipe between the two services. On receiving a request, the driver inspects the type of request, decrypts the returned data, and processes it accordingly. Due to the absence of expected data in the report buffers (as opposed to the main usermode shellcode), this mechanism can be exploited to decrypt, nullify, and then re-encrypt data. This ensures that no reports are sent back to the service, while still allowing for a response to be transmitted.

## Installation

1. Download Infinity Hook from [this repository](https://github.com/everdox/InfinityHook).
2. Integrate Infinity Hook into the project and configure it in `main.cpp` as per the provided instructions.
3. For UEFI users: If you are using UEFI, it's likely you have a mechanism in place to hook syscalls from a driver (utilizing `KiSystemServiceCopyEnd`). If so, you should be familiar with the subsequent steps.

## Credits

This project employs FindWDK for cmake, available at [this repository](https://github.com/SergiusTheBest/FindWDK).
