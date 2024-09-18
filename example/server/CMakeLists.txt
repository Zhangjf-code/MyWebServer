set(SRC_LIST1 testserver.cc)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/example/server)
add_executable(testserver ${SRC_LIST1})
target_link_libraries(testserver mytinymuduo pthread)

