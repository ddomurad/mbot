project(mbotVideoStreamer)

cmake_minimum_required(VERSION 3.0)
add_compile_options(-std=c++11)

find_package (Threads)

aux_source_directory(. SRC_LIST)
add_executable(mbotVideoStreamer ${SRC_LIST})

target_link_libraries (mbotVideoStreamer ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(mbotVideoStreamer mbotCore mbotVideoCapture)
install(TARGETS mbotVideoStreamer DESTINATION "/usr/local/bin")
