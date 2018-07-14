#project(mbotCore VERSION 0.0.1 DESCRIPTION "Rpi video capture lib")
project(mbotCore)

cmake_minimum_required(VERSION 3.0)

add_compile_options(-std=c++11)

aux_source_directory(${mbotCore_SOURCE_DIR}/src SRC_LIST)
FILE(GLOB_RECURSE INC_LIST ${mbotCore_SOURCE_DIR}/inc/*.h)

add_library(mbotCore SHARED ${INC_LIST} ${SRC_LIST})

target_include_directories(mbotCore PRIVATE .)

install(TARGETS mbotCore LIBRARY DESTINATION "/usr/local/lib")
install(DIRECTORY ${mbotCore_SOURCE_DIR}/inc/ DESTINATION "/usr/local/include/mbot/core")
