set(CMAKE_CXX_STANDARD 17)
set(VCPKG_TARGET_TRIPLET "x86-windows" CACHE STRING "VCPKG Target Triplet to use")
cmake_minimum_required(VERSION 3.0.0)
project(main VERSION 0.1.0)

include(CTest)
enable_testing()

find_path(DETOURS_INCLUDE_DIRS detours/detours.h)
set(DETOURS_INCLUDE_DIRS ${DETOURS_INCLUDE_DIRS}/detours)
message(${DETOURS_INCLUDE_DIRS})

find_library(DETOURS_LIBRARY detours)
message(${DETOURS_LIBRARY})

add_library(mydll SHARED trcapi.cpp)
set_target_properties(mydll PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
# target_compile_features(mydll PRIVATE cxx_std_17)
target_include_directories(mydll PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(mydll PRIVATE ${DETOURS_LIBRARY})

add_executable(main main.cpp)
set_target_properties(main PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
