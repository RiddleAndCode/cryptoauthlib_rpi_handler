#include <iomanip>
#include "atecc608_handler.h"


void printHexBuffer(std::vector<uint8_t> input){
    for (const auto & elem : input) {
        std::cout << std::setfill('0') 
                << std::setw(2) 
                << std::uppercase 
                << std::hex 
                << (0xFF & elem) 
                << " ";
    }
    std::cout << "\n";
}


void write_atecc_config()
{
  auto status = atecc_handler_init(0xC0, 1);
  if(status){
    std::cout << "atecc_handler_init Fail! ";
    std::cout << status << "\n";
    return;
  }

  status = atecc_handle_write_default_configuration();
  if(status){
    std::cout << "atecc_handler_write_configuration Fail! ";
    std::cout << status << "\n";
    return;
  }

  status = atecc_handler_lock_zone(0);
  if(status){
    std::cout << "atecc_handler_lock_zone Fail! ";
    std::cout << status << "\n";
    return;
  }
}


int general_test(){
    auto status = atecc_handler_init(0xC0, 1);
    if(status){
        std::cout << "atecc_handler_init Fail! ";
        std::cout << status << "\n";
        return 0;
    }

    int slotID = 1;
    std::vector<uint8_t> pub_key(64);
    status = atecc_handler_genkey(slotID, pub_key.data());
    if(status){
        std::cout << "atecc_handler_genkey Fail! ";
        std::cout << status << "\n";
        return 0;
    }
    std::cout << "Pub key generated: " << "\n";
    printHexBuffer(pub_key);

    std::vector<uint8_t> priv_key = {
    0x00, 0x00, 0x00, 0x00,
    0x39, 0xac, 0x9b, 0xf9, 0x17, 0x1d, 0xe8, 0x6f, 0xfa, 0x77, 0xe0, 0xb9, 0x05, 0x0b, 0xf6, 0xe0,
    0x6a, 0x2c, 0x1b, 0xc1, 0x76, 0x79, 0x36, 0xe6, 0xc7, 0x45, 0x79, 0xe4, 0x26, 0xa4, 0x47, 0x5f
    };

    status = atecc_handler_inject_priv_key(slotID, priv_key.data());
    if(status){
        std::cout << "atecc_handler_inject_priv_key Fail! ";
        std::cout << status << "\n";
        return 0;
    }


    status = atecc_handler_get_public_key(slotID, pub_key.data());
    if(status){
        std::cout << "atecc_handler_get_public_key Fail! ";
        std::cout << status << "\n";
        return 0;
    }

    std::cout << "Pub key: " << "\n";
    printHexBuffer(pub_key);

    std::vector<uint8_t> signature(64);
    std::vector<uint8_t> msg = {
    0xc8, 0x90, 0xf8, 0x65, 0xf3, 0xb0, 0x5f, 0x78, 0x27, 0x03, 0x4a, 0xae, 0x6a, 0xc2, 0x5c, 0xd5,
    0xcb, 0xca, 0x5d, 0x25, 0xeb, 0x0f, 0x0c, 0x35, 0xdf, 0x5d, 0x33, 0x90, 0x3e, 0x08, 0xfa, 0xbe
    };

    status = atecc_handler_sign(slotID, msg.data(), signature.data());
    if(status){
        std::cout << "atecc_handler_sign Fail! ";
        std::cout << status << "\n";
        return 0;
    }

    std::cout << "Signature: " << "\n";
    printHexBuffer(signature);

    status = atecc_handler_verify(slotID, msg.data(), signature.data(), pub_key.data());
    if(status){
        std::cout << "atecc_handler_verify Fail! ";
        std::cout << status << "\n";
        return 0;
    }

    // status = atecc_handler_lock_slot(slotID);
    // if(status){
    //   std::cout << "atecc_handler_lock_slot Fail!" << "\n";
    //   std::cout << status << "\n";
    //   return 0;
    // }

    std::cout << "TEST ENDED SUCCESSFULLY!" << "\n";

    return 0;
}


int main(){
    write_atecc_config();
    general_test();

    return 0;
}