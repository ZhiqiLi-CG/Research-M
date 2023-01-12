set(ResearchM_DIR ${CMAKE_CURRENT_LIST_DIR})
include(${ResearchM_DIR}/../zqBasicUtils/zqBasicUtils.cmake)
include(${ResearchM_DIR}/../zqBasicMath/zqBasicMath.cmake)
get_filename_component(ResearchM_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../ ABSOLUTE)
#----------- Set Option-------
option(CUDA_ENABLE "whether use cuda" OFF)

# ---------- CUDA ----------
if(CUDA_ENABLE)
	find_package(CUDA  REQUIRED)
	if(CUDA_FOUND)
		message(STATUS "CUDA version: ${CUDA_VERSION}")
		include_directories(${CUDA_INCLUDE_DIRS})  
		link_directories(${CUDA_TOOLKIT_ROOT_DIR}/lib/x64)
	else()
		message(ERROR "CUDA not found, you need to install CUDA, or disable CUDA_ENABL")
	endif()
endif()
#--------------- Config-----------------
set(RESEARCHM_ENABLE_CUDA ${CUDA_ENABLE})
configure_file(
"${ResearchM_DIR}/ResearchM_config.h.in"
"${PROJECT_BINARY_DIR}/ResearchM_config.h")
message(STATUS "${PROJECT_BINARY_DIR}/ResearchM_config.h created")
set(ResearchM_INCLUDE_DIRS
	${ResearchM_INCLUDE_DIR}
	${zqBasicUtils_INCLUDE_DIRS}
	${zqBasicMath_INCLUDE_DIRS}
	${PROJECT_BINARY_DIR})
	message(STATUS "Note here:${ResearchM_INCLUDE_DIRS}")

set(ResearchM_DIR ${CMAKE_CURRENT_LIST_DIR})
include(${ResearchM_DIR}/../zqBasicUtils/zqBasicUtils.cmake)
include(${ResearchM_DIR}/../zqBasicMath/zqBasicMath.cmake)
include_directories(${zqBasicUtils_INCLUDE_DIRS})
include_directories(${zqBasicMath_INCLUDE_DIRS})
get_filename_component(ResearchM_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../ ABSOLUTE)
set(ResearchM_INCLUDE_DIRS
	${ResearchM_INCLUDE_DIR}
	${PROJECT_BINARY_DIR})
	
macro(ResearchM_Deps project_name)
	set(CMAKE_CXX_STANDARD 17)
	link_directories(${ResearchM_DIR}/../deps/lib/win64)	
	if(NOT EXISTS ${ResearchM__DIR}/../deps/inc)
		message(FATAL_ERROR "deps not found, update submodule to get zqLibraryDeps")
	else()
		message(STATUS "zqLibraryDeps found as submodule")
		include_directories(${ResearchM__DIR}/../deps/inc)
		if(${MSVC}) 
			set(BIN_PATH "${ResearchM__DIR}/../deps/bin/win64;${ResearchM__DIR}/../deps/bin/win32")
		endif()
	endif()
	# begin to find opengl
	if(${MSVC})
		# we already included OpenGL, so don't need to do anything more
	else()
		# add fmt lib
		#target_link_libraries(${project_name} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
		#target_link_libraries(${project_name} ${GLEW_LIBRARIES})
	endif()
endmacro()
