file(GLOB SRCS *.cpp catch.hpp)

ADD_EXECUTABLE(testMQF ${SRCS})

TARGET_LINK_LIBRARIES(testMQF MQF)

add_test(NAME testMQF
         COMMAND testMQF)
