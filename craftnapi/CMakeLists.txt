#
# Copyright (c) 2024 Dmitry Poroh
# All rights reserved.
# Distributed under the terms of the MIT License. See the LICENSE file.
#

set(SOURCES
  error.cpp
  wrapper.cpp
)

file(GLOB HEADERS "*.hpp")

add_library(craftnapi STATIC ${SOURCES} ${HEADERS})
target_include_directories(craftnapi PUBLIC ${NODE_JS_ROOT_DIR}/include/node)
target_include_directories(craftnapi PUBLIC ${CMAKE_BINARY_DIR}/craftpp/include)
target_include_directories(craftnapi PRIVATE ${CMAKE_SOURCE_DIR})

install(TARGETS craftnapi
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(FILES ${HEADERS} DESTINATION include/craftnapi)


