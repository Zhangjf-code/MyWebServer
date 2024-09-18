# 加载子目录
set(SRC_HTTP
  HttpServer.cc
  HttpResponse.cc
  HttpContext.cc
  main.cc
)

add_executable(HttpServer ${SRC_HTTP})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/src/http)

target_link_libraries(HttpServer mytinymuduo)

