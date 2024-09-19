extern "C"
{
#include "cryptoauthlib.h"
}
#include "atecc608_handler.h"

ATCAIfaceCfg cfg;

/** \brief Write a new configuration to the chip.
 *  \param[in] config Array uint8_t of configuration (length 112)
 *  \param[in] len Size of the configuration array
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handler_write_configuration(const uint8_t *config, size_t len)
{
    if (len != 112)
        return ATCA_BAD_PARAM;

    ATCA_STATUS status;

    status = atcab_init(&cfg);
    if (status == ATCA_SUCCESS)
    {

        // Write the configuration Array to the chip
        // Padding of 16 byte (16 first bytes cannot be overwritten)
        status = atcab_write_bytes_zone(ATCA_ZONE_CONFIG, 0, 16, (uint8_t *)config, len);
        return status;
    }
    return status;
}

/** \brief Write default configuration to the chip.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handle_write_default_configuration(){
    ATCA_STATUS status;
    status = atcab_init(&cfg);
    if (status == ATCA_SUCCESS)
    {
        status = atecc_handler_write_configuration(ENERGY_AGENT_ATEC_CONFIG, sizeof(ENERGY_AGENT_ATEC_CONFIG));
        return status;
    }
    return status;
}


/** \brief Check if a the DATA_ZONE or CONFIG_ZONE is locked
 *  \param[in] zone LOCK_ZONE_DATA or LOCK_ZONE_CONFIG
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int check_lock_zone(uint8_t zone)
{
    ATCA_STATUS status;
    bool lock = false;

    if (zone != (uint8_t)LOCK_ZONE_CONFIG && zone != (uint8_t)LOCK_ZONE_DATA)
        return ATCA_BAD_PARAM;

    status = atcab_init(&cfg);
    if (status == ATCA_SUCCESS)
    {
        if (ATCA_SUCCESS != (status = atcab_is_locked(zone, &lock)))
        {
            return ATCA_FUNC_FAIL;
        }
        if (!lock)
        {
            return ATCA_NOT_LOCKED;
        }
        return ATCA_SUCCESS;
    }
    return ATCA_BAD_PARAM;
}


/** \brief Lock the CONFIG_ZONE or the DATA_ZONE
 *  \param[in] zone LOCK_ZONE_DATA or LOCK_ZONE_CONFIG
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handler_lock_zone(uint8_t zone)
{
    if (zone != (uint8_t)LOCK_ZONE_CONFIG && zone != (uint8_t)LOCK_ZONE_DATA)
        return ATCA_BAD_PARAM;

    ATCA_STATUS status = atcab_init(&cfg);

    if (status == ATCA_SUCCESS)
    {
        if (zone == (uint8_t)LOCK_ZONE_DATA)
        {
            return atcab_lock_data_zone();
        }
        else if (zone == (uint8_t)LOCK_ZONE_CONFIG)
        {
            return atcab_lock_config_zone();
        }
        else
        {
            return ATCA_BAD_PARAM;
        }
    }
    return ATCA_BAD_PARAM;
}


/** \brief Write AES key in a given slot.
 *  \param[in] key      key slot number
 *  \param[in] datakey  key array uint8_t
 *  \param[in] len      Size of the key array
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int write_key_slot(uint8_t key, uint8_t *datakey, size_t len)
{
    if (key < 1 && key > 16)
        return ATCA_BAD_PARAM;

    if (len != 32)
        return ATCA_BAD_PARAM;

    ATCA_STATUS status = atcab_init(&cfg);

    if (status == ATCA_SUCCESS)
    {
        status = atcab_write_zone(ATCA_ZONE_DATA, (uint16_t)key, 0, 0, datakey, 32);
        if (status != ATCA_SUCCESS)
            return status;
    }else {
        std::cout << "Device initialization was not successful." << "\n";
    }
    return status;
}


/** \brief Initialize atecc object and bus
 *  \param[in] i2cAddr i2c address of the slave chip
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handler_init(int i2cAddr, int bus = 1){
    // Init the constuctor for the library
    cfg.iface_type = ATCA_I2C_IFACE;  // Type of communication -> I2C mode
    cfg.devtype = ATECC608B;          // Type of chip
    cfg.atcai2c.address = i2cAddr; // I2C address of Adafruit device
    cfg.atcai2c.bus = bus;
    // cfg.atcai2c.baud = 400000;
    cfg.atcai2c.baud = 100000;
    cfg.wake_delay = 1500; // Delay of wake up (1500 ms)
    cfg.rx_retries = 20;

    ATCA_STATUS status = ATCA_GEN_FAIL;
    status = atcab_init(&cfg);
    
    return status;
}


/** \brief Initialize atecc object and bus
 *  \param[in] config_data configuration array, read vals will be written here
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handler_read_configuration(uint8_t* config_data){
    ATCA_STATUS status = ATCA_GEN_FAIL;
    status = atcab_read_config_zone(config_data);

    return status;
}


/** \brief Initialize atecc object and bus. if Config zone is not locked write
 *         ECCX08_DEFAULT_CONFIGURATION_VALS into config zone and lock it. Otherwise,
 *         private key injection will fail.
 *  \param[in] slot slot number of key to be written
 *  \param[in] priv_key private key byte array
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handler_inject_priv_key(int slot, uint8_t* priv_key){
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t config_data[128];

    // Wake up the device
    status = atcab_wakeup();
    if (status != ATCA_SUCCESS) {
        return status;
    }

    status = atecc_handler_read_configuration(config_data);
    if (status != ATCA_SUCCESS) {
        return status;
    }

    /* Check is key injection allowed for the given slot */
    std::bitset<8> slotConfig_H = config_data[21+(slot*2)];
    if(!slotConfig_H[6])
        return ATCA_EXECUTION_ERROR;

    /* Config Zone should be locked for this process */
    status = check_lock_zone(LOCK_ZONE_CONFIG);
    if(status == ATCA_NOT_LOCKED){
        return status;
    }

    uint8_t num_in[NONCE_NUMIN_SIZE] = { 0 };
    status = atcab_priv_write(slot, priv_key, 0, NULL, num_in);
    if (status != ATCA_SUCCESS) {
        return status;
    }

    return status;
}

/** save data in slot
 *  \param[in] slot slot number to which data is to be written
 *  \param[in] data data byte array to write
 *  \param[in] data_len length of the data byte array (must be a multiple of 32)
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handler_write_data(int slot, uint8_t* data, size_t data_len) {
    if (!data || data_len == 0 || data_len % 4 != 0) {
        return ATCA_BAD_PARAM;
    }

    size_t slot_size;
    ATCA_STATUS status = atcab_get_zone_size(ATCA_ZONE_DATA, slot, &slot_size);
    if (status != ATCA_SUCCESS) {
        return status;
    }

    if (data_len > slot_size) {
        return ATCA_BAD_PARAM; // Data length exceeds slot size
    }

    status = atcab_wakeup();
    if (status != ATCA_SUCCESS) {
        return status;
    }

    size_t bytes_written = 0;
    size_t remaining_bytes = data_len;

    // Write full 32-byte blocks
    while (remaining_bytes >= 32) {
        size_t block = bytes_written / 32;
        status = atcab_write_zone(ATCA_ZONE_DATA, slot, block, 0, &data[bytes_written], 32);
        if (status != ATCA_SUCCESS) {
            return status;
        }
        bytes_written += 32;
        remaining_bytes -= 32;
    }

    // Write remaining 4-byte words
    while (remaining_bytes >= 4) {
        size_t block = bytes_written / 32;
        size_t offset = (bytes_written % 32) / 4;
        status = atcab_write_zone(ATCA_ZONE_DATA, slot, block, offset, &data[bytes_written], 4);
        if (status != ATCA_SUCCESS) {
            return status;
        }
        bytes_written += 4;
        remaining_bytes -= 4;
    }

    return ATCA_SUCCESS;
}



/* read data from slot
 *  \param[in] slot slot number from which data is to be read
 *  \param[out] data buffer to store the read data
 *  \param[in] data_len length of the data byte array (must be a multiple of 32)
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handler_read_data(int slot, uint8_t* data, size_t data_len) {
    if (!data || data_len == 0 || data_len % 4 != 0) {
        return ATCA_BAD_PARAM;
    }

    size_t slot_size;
    ATCA_STATUS status = atcab_get_zone_size(ATCA_ZONE_DATA, slot, &slot_size);
    if (status != ATCA_SUCCESS) {
        return status;
    }

    if (data_len > slot_size) {
        return ATCA_BAD_PARAM; // Data length exceeds slot size
    }

    status = atcab_wakeup();
    if (status != ATCA_SUCCESS) {
        return status;
    }

    size_t num_blocks = data_len / 32;
    size_t remaining_bytes = data_len % 32;
    size_t bytes_read = 0;

    // Read full 32-byte blocks
    for (size_t i = 0; i < num_blocks; i++) {
            status = atcab_read_zone(ATCA_ZONE_DATA, slot, i, 0, &data[bytes_read], 32);
        if (status != ATCA_SUCCESS) {
            return status;
        }
        bytes_read += 32;
    }

    // Read remaining 4-byte words
    size_t num_words = remaining_bytes / 4;
    for (size_t w = 0; w < num_words; w++) {
        size_t block = num_blocks;
        size_t offset = w;
        status = atcab_read_zone(ATCA_ZONE_DATA, slot, block, offset, &data[bytes_read], 4);
        if (status != ATCA_SUCCESS) {
            return status;
        }
        bytes_read += 4;
    }

    return ATCA_SUCCESS;
}



/** \brief Initialize atecc object and bus
 *  \param[in] slot slot number of key to be written
 *  \param[in] pub_key public key will be written here
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handler_get_public_key(int slot, uint8_t* pub_key){
    ATCA_STATUS status = ATCA_GEN_FAIL;

    // Wake up the device
    status = atcab_wakeup();
    if (status != ATCA_SUCCESS) {
        return status;
    }

    status = atcab_get_pubkey(slot, pub_key);
    if(status != ATCA_SUCCESS){
        return status;
    }

    return status;
}


/** \brief Sign given message, return status and signature
 *  \param[in] slot slot number of key to be written
 *  \param[in] signature signature will be written here
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handler_sign(int slot, const uint8_t* msg, uint8_t* signature){
    ATCA_STATUS status = ATCA_GEN_FAIL;

    // Wake up the device
    status = atcab_wakeup();
    if (status != ATCA_SUCCESS) {
        return status;
    }

    status = atcab_sign(slot, msg, signature);
    if(status != ATCA_SUCCESS){
        return status;
    }

    return status;
}


/** \brief Verify given signature 
 *  \param[in] slot slot number of key to be written
 *  \param[in] signature signature will be written here
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handler_verify(int slot, const uint8_t* msg, const uint8_t* signature, const uint8_t* pub_key){
    ATCA_STATUS status = ATCA_GEN_FAIL;

    // Wake up the device
    status = atcab_wakeup();
    if (status != ATCA_SUCCESS) {
        return status;
    }

    bool is_verified = false;
    status = atcab_verify_extern(msg, signature, pub_key, &is_verified);
    if(status != ATCA_SUCCESS){
        return status;
    }

    status = (is_verified) ? ATCA_SUCCESS : ATCA_EXECUTION_ERROR;
    return status;
}


/** \brief Generate private key inside of secure element, return its pubkey
 *  \param[in] slot slot number of key to be written
 *  \param[in] pub_key public key will be written here
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handler_genkey(int slot, uint8_t* pub_key){
    ATCA_STATUS status = ATCA_GEN_FAIL;

    // Wake up the device
    status = atcab_wakeup();
    if (status != ATCA_SUCCESS) {
        return status;
    }

    status = atcab_genkey(slot, pub_key);
    
    return status;
}


/** \brief Lock an individual slot in the data zone
 *  \param[in] slot Slot to be locked in data zone.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
int atecc_handler_lock_slot(int slot){
    ATCA_STATUS status = ATCA_GEN_FAIL;

    // Wake up the device
    status = atcab_wakeup();
    if (status != ATCA_SUCCESS) {
        return status;
    }

    status = atcab_lock_data_slot(slot);
    
    return status;
}

