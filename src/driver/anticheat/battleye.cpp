#include "battleye.h"

namespace battleye {
  struct hwid_packet {
    uint8_t m_key; // 0x00
    int m_valid; // 0x01
    int m_process_id; // 0x05
    int m_windows_version_major; // 0x09
    int m_windows_version_minor; // 0x0D
    int m_windows_version; // 0x11
    double m_average_cpu_time; // 0x15
    double m_average_avx_time; // 0x1D
    double m_average_efer_time; // 0x25
    double m_average_cpu_time_delay_1; // 0x2D
    double m_average_avx_time_delay_1; // 0x35
    double m_average_efer_time_delay_1; // 0x3D
    double m_average_cpu_time_delay_2; // 0x45
    double m_average_avx_time_delay_2; // 0x4D
    double m_average_efer_time_delay_2; // 0x55
    char _0x005D[0x7C]; // all hv detection stuff
    uint32_t m_ia32_feature_control; // 0xD9
    int m_hypervisor_vendor_id_signature[3]; // 0xE1
    uint64_t m_cr4; //0xED
    char m_processor_name[0x30]; // 0xF5
    uint8_t m_hdd_serial_length; // 0x125

    char* serial() { return (char*)((uint64_t)&m_hdd_serial_length + 1); }
    char* smart_serial() { return (char*)((uint64_t)&m_hdd_serial_length + 1 + m_hdd_serial_length); }
  };

  struct thread_creation_data_packet {
    bool m_abnormal; // 0x00
    uint32_t m_thread_start_address; // 0x01
    uint64_t m_stack_info[8]; // 0x05
  };

  void crypt_read_irp(uint8_t* data, int size, int key) {
    bool end = false;
    int index = 0;
    int pre_key = key;

    while (index + 3 < size) {
      *(uint32_t*)&data[index] ^= pre_key ^ ~index;
      if (end) break;

      int post_key = pre_key;
      for (uint32_t i = 0; i < (index & 0x8000001F); i++)
        post_key /= 2;

      if (((uint8_t)(post_key) & 1) == 1)
        pre_key = ~(key + 1);
      else pre_key = key;

      uint8_t* pre_key_b = (uint8_t*)&pre_key;
      index += (pre_key_b[index & 0x80000003] & 3) + 1;

      while (index < size && index + 3 >= size) {
        index--;
        end = true;
      }
    }
  }

  void decrypt_report(uint8_t* data, int size, uint8_t init_key) {
    for (int i = size - 1; i >= 0; i--) {
      uint8_t key = 0;
      if (i > 0)
        key = ~(data[i - 1]);
      else key = init_key;
      data[i] ^= 0xA5 ^ (uint8_t)i ^ key;
    }
  }

  void decrypt_report_into(uint8_t* data, int size, uint8_t init_key, uint8_t* target) {
    for (int i = size - 1; i >= 0; i--) {
      uint8_t key = 0;
      if (i > 0)
        key = ~(data[i - 1]);
      else key = init_key;
      target[i] = data[i] ^ (uint8_t)0xA5 ^ (uint8_t)i ^ key;
    }
  }

  void decrypt_hardware_information(uint8_t* data) {
    for (uint32_t i = 0x16C; i > 0; i--) {
      uint8_t key = ~(data[i - 1]);
      if (i == 1) key = data[0];
      data[i] ^= 0xA5 ^ uint8_t(i - 1) ^ key;
    }
  }

  void encrypt_hardware_information(uint8_t* data) {
    uint8_t key = data[0];
    uint32_t index = 0;

    for (uint32_t i = 0; i < 0x16C; i++) {
      uint8_t next_key = (uint8_t)index ^ data[i + 1] ^ key ^ 0xA5;
      data[i + 1] = next_key;
      key = ~next_key;
      index = i + 1;
    }
  }

  void handle_packet(uint8_t* buffer, uint32_t size, int key) {
    switch (size) {
      case 0x208: { // report buffer
        // decrypt
        crypt_read_irp(buffer, size, key);

/*#ifdef DEV
        uint8_t* report_buffer = buffer;
        while (true) {
          uint16_t iteration_size = *(uint16_t*)&report_buffer[0];
          if (iteration_size == 0)
            break;
          
          report_buffer = &report_buffer[2];
          int report_key = *(int*)&report_buffer[0];

          decrypt_report(&report_buffer[4], iteration_size - 4, (uint8_t)report_key);

          LOG("[BattlEye - Report Packet] Report: %i", report_buffer[4]);

          report_buffer = &report_buffer[iteration_size];
        }
#endif*/

        // null out the buffer
        memset(buffer, 0, size);

        // encrypt
        crypt_read_irp(buffer, size, key);
        break;
      }

      /*case 0x3FC: { // hwid buffer
        // decrypt irp
        crypt_read_irp(buffer, size, key);

        // decrypt hwid
        decrypt_hardware_information(buffer);

        // modify if needed
        if (g_driver.m_spoofing.first) {
          hwid_packet* hpacket = (hwid_packet*)buffer;
          strcpy(hpacket->serial(), g_driver.m_serial_spoof_list.get(hpacket->serial()));
          strcpy(hpacket->smart_serial(), g_driver.m_serial_spoof_list.get(hpacket->smart_serial()));
        }

        // encrypt hwid
        encrypt_hardware_information(buffer);

        // encrypt irp
        crypt_read_irp(buffer, size, key);
        break;
      }*/

      case 0x45: { // latest thread created outside of game process data buffer
        // decrypt
        crypt_read_irp(buffer, size, key);

/*#ifdef DEV
        thread_creation_data_packet* packet = (thread_creation_data_packet*)buffer;
        LOG("[BattlEye - Thread Creation Packet] Abnormal: %i", packet->m_abnormal);
        LOG("[BattlEye - Thread Creation Packet] Thread Start Address: 0x%X", packet->m_thread_start_address);
#endif*/

        // null out the buffer
        memset(buffer, 0, size);

        // encrypt
        crypt_read_irp(buffer, size, key);
        break;
      }

      case 0x212: { // blocked file open info buffer
        // no handler needed
        break;
      }

      case 0x4: { // target process id
        // no handler needed
        break;
      }
    }
  }
};