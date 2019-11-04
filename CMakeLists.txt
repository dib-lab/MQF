cmake_minimum_required(VERSION 3.2)


# Options for enabling/disabling MQF binaries and tests

option(SUPRESS_BIN "Suppress building MQF binaries" OFF)
option(SUPRESS_TESTS "Suppress building MQF tests" OFF)

if(SUPRESS_BIN)
  add_definitions(-DSUPRESS_BIN=1)
else()
  add_definitions(-DSUPRESS_BIN=0)
endif()
if(SUPRESS_TESTS)
  add_definitions(-DSUPRESS_TESTS=1)
else()
  add_definitions(-DSUPRESS_TESTS=0)
endif()

# ------------------ END Of Options ------------------


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(MQF_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(MQF_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(MQF_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)



include_directories(${MQF_INSTALL_INCLUDE_DIR})

## stxxl

# disallow in-source builds
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")

# enable warnings (always good)

set(CMAKE_CXX_FLAGS "-fPIC -Wall  ${PROFILE} ${OPT} -m64 -Wno-unused-result -Wno-strict-aliasing -Wno-unused-function -fopenmp  -fpermissive")

add_definitions(${CMAKE_CXX_FLAGS})


# include the STXXL library in my-project
add_subdirectory(ThirdParty/stxxl)

# apply STXXL CXXFLAGS to our configuration
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}")

# add STXXL includes path
include_directories(${STXXL_INCLUDE_DIRS})


add_subdirectory(src)


if (NOT SUPRESS_BIN)
    message("Building MQF binaries")
    add_subdirectory(bin)
endif()

if (NOT SUPRESS_BIN)
    message("Building MQF tests")
    enable_testing()
    add_subdirectory(tests)
endif()