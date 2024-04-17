cmake_minimum_required(VERSION 2.8)

project(Lunar)

# find_package(yaml-cpp REQUIRED)
find_library(yaml yaml-cpp)
find_library(pthread-lib pthread)

include_directories(./inc)

set(LIB_LIST ${pthread-lib} dl ${yaml})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(SRC_LIST
)
message(STATUS "-----SRC_LIST:${SRC_LIST}-----LIB_LIST:${LIB_LIST}---YAML-CPP:${yaml}")
add_compile_options(-std=c++11 -Wall -g)

# set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# add_library (lunarLib STATIC ${SRC_LIST})
# # set_target_properties (lunarLib PROPERTIES OUTPUT_NAME "lunarLib")
# target_link_libraries(lunarLib yaml-cpp ${LIB_LIST})

# add_executable(test_log tests/test_log.cc)
# target_link_libraries(test_log lunarLib)