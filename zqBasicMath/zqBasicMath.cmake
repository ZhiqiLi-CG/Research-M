set(zqBasicMath_DIR ${CMAKE_CURRENT_LIST_DIR})
include(${zqBasicMath_DIR}/../zqBasicUtils/zqBasicUtils.cmake)
include_directories(${zqBasicUtils_INCLUDE_DIRS})
get_filename_component(zqBasicMath_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../ ABSOLUTE)
set(zqBasicMath_INCLUDE_DIRS
	${zqBasicMath_INCLUDE_DIR}
	${PROJECT_BINARY_DIR})
	
