cmake_minimum_required(VERSION 3.31)
project(miv C)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_executable(miv src/main.c)

target_link_libraries(miv PRIVATE
  SDL3::SDL3
  SDL3_image::SDL3_image
)

install(TARGETS miv RUNTIME DESTINATION bin)
