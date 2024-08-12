import ctypes
from ctypes import c_int, c_uint8, c_size_t, POINTER, byref, create_string_buffer
import sys

# Load the shared library
atecc608_lib = ctypes.CDLL('../build/libatecc608_handler.so')

# Define argument and return types for the functions
atecc608_lib.atecc_handler_init.argtypes = [c_int, c_int]
atecc608_lib.atecc_handler_init.restype = c_int

atecc608_lib.atecc_handler_write_configuration.argtypes = [POINTER(c_uint8), c_size_t]
atecc608_lib.atecc_handler_write_configuration.restype = c_int

atecc608_lib.atecc_handler_lock_zone.argtypes = [c_uint8]
atecc608_lib.atecc_handler_lock_zone.restype = c_int

atecc608_lib.atecc_handler_genkey.argtypes = [c_int, POINTER(c_uint8)]
atecc608_lib.atecc_handler_genkey.restype = c_int

atecc608_lib.atecc_handler_inject_priv_key.argtypes = [c_int, POINTER(c_uint8)]
atecc608_lib.atecc_handler_inject_priv_key.restype = c_int

atecc608_lib.atecc_handler_get_public_key.argtypes = [c_int, POINTER(c_uint8)]
atecc608_lib.atecc_handler_get_public_key.restype = c_int

atecc608_lib.atecc_handler_sign.argtypes = [c_int, POINTER(c_uint8), POINTER(c_uint8)]
atecc608_lib.atecc_handler_sign.restype = c_int

atecc608_lib.atecc_handler_verify.argtypes = [c_int, POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8)]
atecc608_lib.atecc_handler_verify.restype = c_int

def print_hex_buffer(input):
    print(" ".join(f"{x:02X}" for x in input))

def write_atecc_config():
    status = atecc608_lib.atecc_handler_init(0xC0, 1)
    if status:
        print(f"atecc_handler_init Fail! {status}")
        return

    ECCX08_DEFAULT_CONFIGURATION_VALS = (c_uint8 * 112)()  # Adjust the size accordingly
    status = atecc608_lib.atecc_handler_write_configuration(ECCX08_DEFAULT_CONFIGURATION_VALS, 112)
    if status:
        print(f"atecc_handler_write_configuration Fail! {status}")
        return

    status = atecc608_lib.atecc_handler_lock_zone(0)
    if status:
        print(f"atecc_handler_lock_zone Fail! {status}")
        return

def general_test():
    status = atecc608_lib.atecc_handler_init(0xC0, 1)
    if status:
        print(f"atecc_handler_init Fail! {status}")
        return 0

    slotID = 1
    pub_key = (c_uint8 * 64)()
    status = atecc608_lib.atecc_handler_genkey(slotID, pub_key)
    if status:
        print(f"atecc_handler_genkey Fail! {status}")
        return 0
    print("Pub key generated:")
    print_hex_buffer(pub_key)

    priv_key = (c_uint8 * 36)(
        0x00, 0x00, 0x00, 0x00,
        0x39, 0xac, 0x9b, 0xf9, 0x17, 0x1d, 0xe8, 0x6f, 0xfa, 0x77, 0xe0, 0xb9, 0x05, 0x0b, 0xf6, 0xe0,
        0x6a, 0x2c, 0x1b, 0xc1, 0x76, 0x79, 0x36, 0xe6, 0xc7, 0x45, 0x79, 0xe4, 0x26, 0xa4, 0x47, 0x5f
    )

    status = atecc608_lib.atecc_handler_inject_priv_key(slotID, priv_key)
    if status:
        print(f"atecc_handler_inject_priv_key Fail! {status}")
        return 0

    status = atecc608_lib.atecc_handler_get_public_key(slotID, pub_key)
    if status:
        print(f"atecc_handler_get_public_key Fail! {status}")
        return 0
    print("Pub key:")
    print_hex_buffer(pub_key)

    signature = (c_uint8 * 64)()
    msg = (c_uint8 * 32)(
        0xc8, 0x90, 0xf8, 0x65, 0xf3, 0xb0, 0x5f, 0x78, 0x27, 0x03, 0x4a, 0xae, 0x6a, 0xc2, 0x5c, 0xd5,
        0xcb, 0xca, 0x5d, 0x25, 0xeb, 0x0f, 0x0c, 0x35, 0xdf, 0x5d, 0x33, 0x90, 0x3e, 0x08, 0xfa, 0xbe
    )

    status = atecc608_lib.atecc_handler_sign(slotID, msg, signature)
    if status:
        print(f"atecc_handler_sign Fail! {status}")
        return 0
    print("Signature:")
    print_hex_buffer(signature)

    status = atecc608_lib.atecc_handler_verify(slotID, msg, signature, pub_key)
    if status:
        print(f"atecc_handler_verify Fail! {status}")
        return 0

    print("TEST ENDED SUCCESSFULLY!")
    return 0


if __name__ == "__main__":
    # write_atecc_config()
    # Uncomment the following line to run the general test
    general_test()


    
