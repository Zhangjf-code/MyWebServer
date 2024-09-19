set(SRC_LIST3 echoserver.cc)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/example/server-timequeue)
add_executable(echoserver ${SRC_LIST3})
target_link_libraries(echoserver mytinymuduo pthread)

