set(zqBasicMath_DIR ${CMAKE_CURRENT_LIST_DIR})
include(${zqBasicMath_DIR}/../zqBasicUtils/zqBasicUtils.cmake)
get_filename_component(zqBasicMath_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../ ABSOLUTE)
set(zqBasicMath_INCLUDE_DIRS
	${zqBasicMath_INCLUDE_DIR}
	${zqBasicUtils_INCLUDE_DIRS}
	${PROJECT_BINARY_DIR})
	
