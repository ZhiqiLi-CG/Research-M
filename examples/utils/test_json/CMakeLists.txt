# Set project_name as the directory name
file(RELATIVE_PATH project_name ${CMAKE_CURRENT_LIST_DIR}/.. ${CMAKE_CURRENT_LIST_DIR})
file(RELATIVE_PATH folder_name ${CMAKE_CURRENT_LIST_DIR}/../.. ${CMAKE_CURRENT_LIST_DIR}/..)
###############################################
# Include deps
###############################################
include(${CMAKE_CURRENT_LIST_DIR}/../../../ResearchM/ResearchM.cmake)
ResearchM_Deps(${project_name})
###############################################
# Add the project 
###############################################

if(CUDA_ENABLE)
	project(${project_name} LANGUAGES CXX CUDA)
	file(GLOB  SOURCES
		"./*.h"
		"./*.cpp"
		"./*.cu"
	)
else()
	project(${project_name} LANGUAGES CXX)
	file(GLOB  SOURCES
		"./*.h"
		"./*.cpp"
	)
endif()
add_executable(${project_name} ${SOURCES})
###############################################
# Add this project to corresponding folder
###############################################
set_target_properties(${project_name} PROPERTIES FOLDER ${folder_name})

message(STATUS "added project: ${folder_name}/${project_name}")
