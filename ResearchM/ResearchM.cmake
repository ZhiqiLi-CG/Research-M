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


