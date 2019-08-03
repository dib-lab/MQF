cmake_minimum_required(VERSION 3.2)
project(MQF C CXX)

set(SOURCE_FILES
  bufferedMQF.cpp
  gqf.cpp
  onDiskMQF.cpp
  utils.cpp
)

set(CMAKE_CXX_FLAGS "-fPIC -Wall ${DEBUG} ${PROFILE} ${OPT} -m64 -Wno-unused-result -Wno-strict-aliasing -Wno-unused-function -fopenmp  -fpermissive")

add_definitions(${CMAKE_CXX_FLAGS})

add_library(MQF SHARED STATIC ${SOURCE_FILES})
target_link_libraries(MQF ${STXXL_LIBRARIES})

install(TARGETS MQF DESTINATION ${MQF_INSTALL_LIB_DIR})
#install(FILES division.h DESTINATION ${DIVISIBLE_INSTALL_INCLUDE_DIR})
