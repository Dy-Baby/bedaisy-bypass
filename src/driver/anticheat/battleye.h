#include "pch.h"
#include "driver/driver.h"

namespace battleye {
  void crypt_read_irp(uint8_t* data, int size, int key);
  void decrypt_report(uint8_t* data, int size, uint8_t init_key);
  void decrypt_report_into(uint8_t* data, int size, uint8_t init_key, uint8_t* target);
  void decrypt_hardware_information(uint8_t* data);
  void encrypt_hardware_information(uint8_t* data);

  void handle_packet(uint8_t* buffer, uint32_t size, int key);
};