cmake_minimum_required(VERSION 3.26)
project(maya_plugins)

set(CMAKE_CXX_STANDARD 17)

# set maya include path
set(MAYA_PATH "C:/Program Files/Autodesk/Maya2022")
include_directories(${MAYA_PATH}/include)
link_directories(${MAYA_PATH}/lib)
set(LINK_FLAGS "/export:initializePlugin /export:uninitializePlugin")

# Maya plug-ins that consist of a single file and are intended for your own individual use or for use within your organization can be distributed by placing the files in a shared directory pointed at by the MAYA_PLUG_IN_PATH in users' Maya.env files.
# You can add several directories to MAYA_PLUG_IN_PATH. Each directory must be separated by a semicolon (;) on Windows or by a colon (:) on macOS and Linux.
set(MAYA_PLUG_IN_PATH "${PROJECT_SOURCE_DIR}/out")

add_subdirectory(helloWorld)
add_subdirectory(pickCmd)
add_subdirectory(pluginCallbacks)
add_subdirectory(zoomCameraCmd)