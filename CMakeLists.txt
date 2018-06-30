cmake_minimum_required(VERSION 2.8)
project( radascii )
add_executable( radascii radascii.c)

set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)

find_package( OpenCV REQUIRED )

target_link_libraries( radascii ${OpenCV_LIBS} ${CURL_LIBRARIES})
