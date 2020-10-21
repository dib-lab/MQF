cmake_minimum_required(VERSION 3.2)
project(MQF C CXX)

set(SOURCE_FILES
  bufferedMQF.cpp
  gqf.cpp
  onDiskMQF.cpp
  utils.cpp
)

set(INCLUDE_FILES
        ../include/bufferedMQF.h
        ../include/gqf.h
        ../include/onDiskMQF.h
        ../include/utils.h
)



add_library(MQF SHARED STATIC ${SOURCE_FILES})
target_link_libraries(MQF ${STXXL_LIBRARIES})

install(TARGETS MQF DESTINATION ${MQF_INSTALL_LIB_DIR})
#install(FILES ${INCLUDE_FILES}  DESTINATION ${MQF_INSTALL_LIB_DIR})
#install(FILES division.h DESTINATION ${DIVISIBLE_INSTALL_INCLUDE_DIR})
