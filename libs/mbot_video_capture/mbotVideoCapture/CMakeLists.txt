project(mbotVideoCapture)
cmake_minimum_required(VERSION 3.0)
add_compile_options(-std=c++11)

aux_source_directory(${mbotVideoCapture_SOURCE_DIR}/src SRC_LIST)
FILE(GLOB_RECURSE INC_LIST ${mbotVideoCapture_SOURCE_DIR}/inc/*.h)

find_package (Threads)

add_library(mbotVideoCapture SHARED ${INC_LIST} ${SRC_LIST})

target_include_directories(mbotVideoCapture PRIVATE .)

install(TARGETS mbotVideoCapture LIBRARY DESTINATION "/usr/local/lib")
install(DIRECTORY ${mbotVideoCapture_SOURCE_DIR}/inc/ DESTINATION "/usr/local/include/mbot/videoCapture")
