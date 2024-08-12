# cryptoauthlib_rpi_handler
A handler to use atecc chip library on Raspberry Pi


# Prerequests
Connect the ATECC608 to the Raspberry Pi.

Enable I2C on the Raspberry Pi;

  ```
  sudo raspi-config
  ```
- Go to Interfacing Options
- Select I2C and enable it

```
sudo apt-get install -y i2c-tools
```
then reboot device
```
sudo reboot
```

Install the ATECC608 Library:
```
sudo apt-get install -y libssl-dev
git clone https://github.com/MicrochipTech/cryptoauthlib.git
cd cryptoauthlib
mkdir build
cd build
cmake -DATCA_HAL_I2C=ON -DATCA_I2C_IFACE=ON ..
make
sudo make install
```

Use i2cdetect to verify the ATECC608 is detected:
```
sudo i2cdetect -y 1
```
Ensure you have the correct I2C address. The default for the ATECC608 is usually 0xC0 (7-bit address 0x60).

# Build Project

```
git clone git@github.com:RiddleAndCode/cryptoauthlib_rpi_handler.git
mkdir build
cd build
cmake ..
make
```
