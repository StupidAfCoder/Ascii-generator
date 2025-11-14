cmake_minimum_required(VERSION 3.16)
project(ascii_art VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VENDOR_CXXOPTS "Use vendored cxxopts in third_party/ to avoid network fetch" OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if(NOT VENDOR_CXXOPTS)
  include(FetchContent)
  FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG v3.0.0
  )
  FetchContent_MakeAvailable(cxxopts)
  set(CXXOPTS_TARGET cxxopts::cxxopts)
else()
  add_library(cxxopts INTERFACE)
  target_include_directories(cxxopts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cxxopts/include)
  set(CXXOPTS_TARGET cxxopts)
endif()

set(SRC
    src/main.cpp
    src/ascii-converter.cpp
)

add_executable(ascii-generator ${SRC})
target_include_directories(ascii-generator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(ascii-generator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)
target_link_libraries(ascii-generator PRIVATE ${CXXOPTS_TARGET})

if(NOT MSVC)
  target_compile_options(ascii-generator PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(GNUInstallDirs)
install(TARGETS ascii-generator
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}      
  COMPONENT runtime
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/ascii-generator
          FILES_MATCHING PATTERN "*")
endif()

set(CPACK_PACKAGE_NAME "ascii-generator")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "TGZ;ZIP")
include(CPack)
