set (XXHASH_SRC "${PROJECT_SOURCE_DIR}/External/xxHash")

include(FetchContent)
 
FetchContent_Declare(
  xxhash_cpp
  GIT_REPOSITORY "https://github.com/Cyan4973/xxHash"
  GIT_TAG        dev
  SOURCE_DIR ${XXHASH_SRC}/repository SUBBUILD_DIR ${XXHASH_SRC}/subbuild  BINARY_DIR ${XXHASH_SRC}/binary
  )

FetchContent_GetProperties(xxhash_cpp)
if(NOT xxhash_cpp_POPULATED)  
  FetchContent_Populate(xxhash_cpp)
endif()
  
message(STATUS "xxhash_cpp include dir: ${xxhash_cpp_SOURCE_DIR}")
add_custom_target(XXHASH_MAKE_INSTALL ALL COMMAND make install -C ${xxhash_cpp_SOURCE_DIR} DESTDIR=${xxhash_cpp_SOURCE_DIR})


list(APPEND CMAKE_INCLUDE_PATH "${xxhash_cpp_SOURCE_DIR}")
