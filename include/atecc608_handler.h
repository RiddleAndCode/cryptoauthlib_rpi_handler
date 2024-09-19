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

const uint8_t ENERGY_AGENT_ATEC_CONFIG[128] = {
    // Bytes 0-3: Device-specific settings (I2C_Address, Reserved, OTPmode, ChipMode)
    0xC0, 0x00, 0x55, 0x00,

    // Bytes 4-35: SlotConfig for slots 0-15 (2 bytes per slot)
    // Slots 0-7: Unchanged SlotConfig settings
    0x83, 0x60, // Slot 0 (Bytes 4-5)
    0x87, 0x60, // Slot 1 (Bytes 6-7)
    0x8F, 0x60, // Slot 2 (Bytes 8-9)
    0x83, 0x60, // Slot 3 (Bytes 10-11)
    0x83, 0x60, // Slot 4 (Bytes 12-13)
    0x8F, 0x8F, // Slot 5 (Bytes 14-15)
    0x9F, 0x8F, // Slot 6 (Bytes 16-17)
    0xAF, 0x8F, // Slot 7 (Bytes 18-19)

    // Slot 8: Configured for plaintext read/write
    0x00, 0x00, // Slot 8 (Bytes 20-21)

    // Slots 9-15: Unchanged SlotConfig settings
    0x00, 0x00, // Slot 9 (Bytes 22-23)
    0x00, 0x00, // Slot 10 (Bytes 24-25)
    0x00, 0x00, // Slot 11 (Bytes 26-27)
    0x00, 0x00, // Slot 12 (Bytes 28-29)
    0x00, 0x00, // Slot 13 (Bytes 30-31)
    0x00, 0x00, // Slot 14 (Bytes 32-33)
    0xAF, 0x8F, // Slot 15 (Bytes 34-35)

    // Bytes 36-43: Counter[0]
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,

    // Bytes 44-51: Counter[1]
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,

    // Bytes 52-67: LastKeyUse (16 bytes)
    0xFF, 0xFF, 0xFF, 0xFF, // Slots 0-3
    0xFF, 0xFF, 0xFF, 0xFF, // Slots 4-7
    0xFF, 0xFF, 0xFF, 0xFF, // Slots 8-11
    0xFF, 0xFF, 0xFF, 0xFF, // Slots 12-15

    // Bytes 68-71: Write via commands only - start
    0x00, // UserExtra (Byte 68)
    0x00, // Selector (Byte 69)
    0x55, // LockValue (Byte 70)
    0x55, // LockConfig (Byte 71)

    // Bytes 72-73: SlotLocked
    0xFF, 0xFF,

    // Bytes 74-75: RFU
    0x00, 0x00,

    // Bytes 76-79: X509format
    0x00, 0x00, 0x00, 0x00,

    // Bytes 80-111: KeyConfig for slots 0-15 (2 bytes per slot)
    // Slots 0-6: Unchanged KeyConfig settings
    0x33, 0x00, // Slot 0 (Bytes 80-81)
    0x33, 0x00, // Slot 1 (Bytes 82-83)
    0x33, 0x00, // Slot 2 (Bytes 84-85)
    0x1C, 0x00, // Slot 3 (Bytes 86-87)
    0x33, 0x00, // Slot 4 (Bytes 88-89)
    0x1C, 0x00, // Slot 5 (Bytes 90-91)
    0x1C, 0x00, // Slot 6 (Bytes 92-93)
    0x1C, 0x00, // Slot 7 (Bytes 94-95)

    // Slot 8: KeyConfig adjusted for plaintext read/write
    0x00, 0x00, // Slot 8 (Bytes 96-97)

    // Slots 9-15: Unchanged KeyConfig settings
    0x3C, 0x00, // Slot 9 (Bytes 98-99)
    0x3C, 0x00, // Slot 10 (Bytes 100-101)
    0x3C, 0x00, // Slot 11 (Bytes 102-103)
    0x3C, 0x00, // Slot 12 (Bytes 104-105)
    0x3C, 0x00, // Slot 13 (Bytes 106-107)
    0x3C, 0x00, // Slot 14 (Bytes 108-109)
    0x1C, 0x00, // Slot 15 (Bytes 110-111)

    // Bytes 112-127: Remaining bytes (RFU or padding)
    0x00, 0x00, 0x00, 0x00, // Bytes 112-115
    0x00, 0x00, 0x00, 0x00, // Bytes 116-119
    0x00, 0x00, 0x00, 0x00, // Bytes 120-123
    0x00, 0x00, 0x00, 0x00  // Bytes 124-127
};


extern "C" int atecc_handler_write_configuration(const uint8_t *config, size_t len);
extern "C" int atecc_handle_write_default_configuration();
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
