set(ResearchM_DIR ${CMAKE_CURRENT_LIST_DIR})
include(${ResearchM_DIR}/../zqBasicUtils/zqBasicUtils.cmake)
include(${ResearchM_DIR}/../zqBasicMath/zqBasicMath.cmake)
include_directories(${zqBasicUtils_INCLUDE_DIRS})
include_directories(${zqBasicMath_INCLUDE_DIRS})
get_filename_component(ResearchM_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../ ABSOLUTE)
set(ResearchM_INCLUDE_DIRS
	${ResearchM_INCLUDE_DIR}
	${PROJECT_BINARY_DIR})
	
