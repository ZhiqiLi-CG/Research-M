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
	project(${project_name} LANGUAGES CXX CUDA)
if(CUDA_ENABLE)
	file(GLOB  SOURCES
		"./*.h"
		"./*.cpp"
		"./*.cu"
	)
else()
	file(GLOB  SOURCES
		"./*.h"
		"./*.cpp"
	)
endif()
	include_directories(${ResearchM_INCLUDE_DIRS})
	add_executable(${project_name} ${SOURCES})
	###############################################
	# Add this project to corresponding folder
	###############################################
	set_target_properties(${project_name} PROPERTIES FOLDER ${folder_name})
	#target_link_libraries(${project_name} ${ResearchM_DIR}/../deps/lib/win64/fmt.lib)
	#target_link_libraries(${project_name} ${ResearchM_DIR}/../deps/lib/win64/fmtd.lib)
	message(STATUS "added project: ${folder_name}/${project_name}")

