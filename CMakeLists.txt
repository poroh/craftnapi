cmake_minimum_required(VERSION 3.0)

project(craftnapi)

set(CMAKE_CXX_STANDARD 20)

option(CODE_COVERAGE "Enable coverage reporting" OFF)
option(ADDRESS_SANITIZER "Enable address sanitizer" OFF)

set(craftnapi_VERSION_MAJOR 1)
set(craftnapi_VERSION_MINOR 0)

add_compile_options(-Werror -Wall -Wunused-variable -Wunused-parameter -pedantic)

if(ADDRESS_SANITIZER)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
  endif()
endif()

if(CODE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0  -fprofile-instr-generate -fcoverage-mapping")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0  -fprofile-instr-generate -fcoverage-mapping")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -O0  -fprofile-instr-generate -fcoverage-mapping")
  endif()
endif()


function(is_valid_napi_path path result)
  message(STATUS "Node.js: find: ${path}")
  if(EXISTS "${path}/include/node/node_api.h" AND EXISTS "${path}/include/node/openssl/err.h")
    set(${result} TRUE PARENT_SCOPE)
  else()
    set(${result} FALSE PARENT_SCOPE)
  endif()
endfunction()

# ================================================================================
set(NODE_JS_SEARCH_PATHS
  # Default installation of openssl
  "/usr"
  # Local installation of openssl
  "/usr/local"
)

if(NOT DEFINED NODE_JS_ROOT_DIR)
  set(default_node_js_path_is_valid FALSE)
else()
  is_valid_napi_path(${NODE_JS_ROOT_DIR} default_node_js_path_is_valid)
endif()

if(NOT default_openssl_path_is_valid)
  foreach (path IN LISTS NODE_JS_SEARCH_PATHS)
    is_valid_napi_path(${path} path_is_valid)
    if (path_is_valid)
      set(NODE_JS_ROOT_DIR "${path}")
      set(NODE_JS_FOUND 1)
      break()
    endif()
  endforeach()
else()
  set(NODE_JS_FOUND 1)
endif()

if(NOT NODE_JS_FOUND)
  error( "node.js is not found")
else()
  message(STATUS "node.js found in ${NODE_JS_ROOT_DIR}")
endif()

# ================================================================================
include(ExternalProject)
ExternalProject_Add(
  craftpp
  GIT_REPOSITORY https://github.com/poroh/craftpp.git
  GIT_TAG        main
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/craftpp
)


add_subdirectory(craftnapi)

target_include_directories(craftnapi PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_dependencies(craftnapi craftpp)

install(TARGETS craftnapi
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# Add tests
# enable_testing()
# add_subdirectory(tests)

