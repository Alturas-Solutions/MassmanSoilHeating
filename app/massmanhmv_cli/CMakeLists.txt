cmake_minimum_required (VERSION 3.8)

project(massmanhmv_cli)

IF(MSVC)
	add_compile_definitions(BUILD_FMC_DLL _CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

add_executable(${PROJECT_NAME} HMV_Model.cpp icf_def.h icf_input.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC massmanhmv)
target_link_libraries(${PROJECT_NAME} PUBLIC massmanhmv)

if (UNIX AND NOT APPLE)
    set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION "${app_dest}")
