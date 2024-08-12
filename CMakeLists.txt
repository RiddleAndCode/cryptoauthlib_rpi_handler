cmake_minimum_required(VERSION 3.10)

# Project name
project(ATECC608_RPI_Handler)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Find the cryptoauthlib library
find_library(CRYPTOAUTH_LIB cryptoauth HINTS /usr/lib /usr/local/lib)

# Find the cryptoauthlib include directory
find_path(CRYPTOAUTH_INCLUDE_DIR
  NAMES atca_basic.h
  HINTS /usr/include /usr/local/include /usr/include/cryptoauthlib /usr/local/include/cryptoauthlib
)

# If the library or include directory is not found, display an error message
if(NOT CRYPTOAUTH_LIB)
  message(FATAL_ERROR "cryptoauth library not found")
endif()

if(NOT CRYPTOAUTH_INCLUDE_DIR)
  message(FATAL_ERROR "cryptoauth include directory not found")
endif()

# Include directories
include_directories(${CRYPTOAUTH_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/include)

# Add the library
add_library(atecc608_handler SHARED src/atecc608_handler.cpp)

# Link the library with cryptoauthlib
target_link_libraries(atecc608_handler ${CRYPTOAUTH_LIB})

# Add the test executable
add_executable(test_atecc608 test/test_atecc608.cpp)

# Link the test executable with the atecc608_handler library
target_link_libraries(test_atecc608 atecc608_handler)

