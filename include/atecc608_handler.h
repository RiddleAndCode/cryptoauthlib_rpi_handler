#include <iostream>
#include <vector>
#include <bitset>



// Exemple of configuration for ATECC608A. Look at page 8 on the datasheet for ATECC608A-TFLXTLS
const uint8_t ECCX08_DEFAULT_CONFIGURATION_VALS[112] = {
  // Read only - end
  // I2C_Address
  0xC0,
  // Reserved
  0x00,
  // OTPmode
  0x55,
  // ChipMode
  0x00,
  // SlotConfig
  0x83, 0x60, 
  0x87, 0x60, 
  0x8F, 0x60, 
  0x83, 0x60, 
  0x83, 0x60,
  0x8F, 0x8F,
  0x9F, 0x8F, 
  0xAF, 0x8F,
  0x00, 0x00, 
  0x00, 0x00,
  0x00, 0x00, 
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00, 
  0xAF, 0x8F,
  // Counter[0]
  0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
  // Counter[1]
  0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
  // LastKeyUse
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
// Write via commands only - start
  // UserExtra
  0x00, 
  // Selector
  0x00,
  // LockValue
  0x55,
  // LockConfig
  0x55,
  // SlotLocked
  0xFF, 0xFF,
// Write via commands only - end
  // RFU
  0x00, 0x00,
  // X509format
  0x00, 0x00, 0x00, 0x00,
  // KeyConfig
  0x33, 0x00, // Private | Public | P256 NIST ECC key, Default: 0x33, 0x00,
  0x33, 0x00, // Private | Public | P256 NIST ECC key, Default: 0x33, 0x00,
  0x33, 0x00, // Private | Public | P256 NIST ECC key, Default: 0x33, 0x00,
  0x1C, 0x00, // Private | Public | P256 NIST ECC key, Default: 0x1C, 0x00,
  0x33, 0x00, // Private | Public | P256 NIST ECC key, Default: 0x1C, 0x00,
  0x1C, 0x00,
  0x1C, 0x00,
  0x1C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x3C, 0x00,
  0x1C, 0x00
};

extern "C" int atecc_handler_write_configuration(const uint8_t *config, size_t len);
extern "C" int atecc_handler_lock_zone(uint8_t zone);
extern "C" int atecc_handler_init(int i2cAddr, int bus);
extern "C" int atecc_handler_read_configuration(uint8_t *config_data);
extern "C" int atecc_handler_inject_priv_key(int slot, uint8_t * priv_key);
extern "C" int atecc_handler_get_public_key(int slot, uint8_t * pub_key);
extern "C" int atecc_handler_sign(int slot, const uint8_t * msg, uint8_t * signature);
extern "C" int atecc_handler_verify(int slot, const uint8_t * msg, const uint8_t * signature, const uint8_t * pub_key);
extern "C" int atecc_handler_genkey(int slot, uint8_t * pub_key);
extern "C" int atecc_handler_lock_slot(int slot);
extern "C" int atecc_handler_write_data(int slot, uint8_t* data, size_t data_len);
extern "C" int atecc_handler_read_data(int slot, uint8_t* data, size_t data_len);
