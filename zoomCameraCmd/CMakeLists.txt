cmake_minimum_required(VERSION 3.26)
project(zoomCameraCmd)

message(${PROJECT_NAME})
# output a dll
add_library(${PROJECT_NAME} SHARED zoomCameraCmd.cpp)

# set the suffix of the output dynamic library to .mll, 
# and directly put it into ourself plugin dirs
set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
            SUFFIX ".mll"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${MAYA_PLUG_IN_PATH}/plug-ins") # debug

# These two libs are only the core modules of Maya. If you need to use other modules, you need to add other libs, such as OpenMayaUI.lib. You can find all of them in `${MAYA_PATH}/lib`
target_link_libraries(${PROJECT_NAME} Foundation OpenMaya)