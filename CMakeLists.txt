cmake_minimum_required(VERSION 2.8)

project(myClang)

# Need Clang and LLVM to compile
find_package(Clang REQUIRED CONFIG)

# Add path for custom modules
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${LLVM_CMAKE_DIR}"
  )
  
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})

add_definitions(${LLVM_DEFINITIONS})
add_definitions(${CLANG_DEFINITIONS})

# Import LLVM cmake functions for simplier linking
include(AddLLVM)

add_llvm_executable(myClangExe
 sample.cpp
 )
target_link_libraries(myClangExe
 clangTooling
 )
