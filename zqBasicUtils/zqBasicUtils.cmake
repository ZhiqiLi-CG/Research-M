set(zqBasicUtils_DIR ${CMAKE_CURRENT_LIST_DIR})
get_filename_component(zqBasicUtils_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../ ABSOLUTE)
set(CMAKE_CXX_FLAGS "-fopenmp")
message(STATUS "Set openmp for ResearchM and its heri")
set(zqBasicUtils_INCLUDE_DIRS
	${zqBasicUtils_INCLUDE_DIR}
	${PROJECT_BINARY_DIR})
	
