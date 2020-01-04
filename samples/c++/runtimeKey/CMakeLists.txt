cmake_minimum_required(VERSION 3.0)

project(runtimeKey VERSION 1.0.0 LANGUAGES CXX C)

#### ultimate Libraries ####
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../../SDK_dev/lib build/ultimateMRZ/SDK_dev)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../../../../SDK_dev/lib/include
)

set(runtimeKey_SOURCES 
	main.cxx
)

###### The executable ######
add_executable(runtimeKey ${runtimeKey_SOURCES})

###### 3rd parties libs ######
target_link_libraries(runtimeKey ${LIB_LINK_SCOPE} ultimate_mrz-sdk)
add_dependencies(runtimeKey ultimate_mrz-sdk)

###### Install Libs ######
install(TARGETS runtimeKey DESTINATION bin)
