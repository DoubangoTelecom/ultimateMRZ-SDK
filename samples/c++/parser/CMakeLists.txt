cmake_minimum_required(VERSION 3.0)

project(parser VERSION 1.0.0 LANGUAGES CXX C)

#### ultimate Libraries ####
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../../SDK_dev/lib build/ultimateMRZ/SDK_dev)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../../../../SDK_dev/lib/include
)

set(parser_SOURCES 
	main.cxx
)

###### The executable ######
add_executable(parser ${parser_SOURCES})

###### 3rd parties libs ######
target_link_libraries(parser ${LIB_LINK_SCOPE} ultimate_mrz-sdk)
add_dependencies(parser ultimate_mrz-sdk)

###### Install Libs ######
install(TARGETS parser DESTINATION bin)
