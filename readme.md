This is an example collection of cpp maya plugins, sourced from [autodesk website](https://help.autodesk.com/view/MAYAUL/2022/ENU/?guid=Maya_SDK_cpp_ref_index_html).

[Setting-up-your-build](https://help.autodesk.com/view/MAYAUL/2022/ENU/?guid=Setting-up-your-build)

## how can I get these code?

I use a Python crawler, in `cpp_maya_plugins` to get these code.

## Develop

First, 

```cmake
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

set(MAYA_PLUG_IN_PATH "C:/Users/User/work/maya-plugins/out")

add_subdirectory(helloWorld)
add_subdirectory(pickCmd)
add_subdirectory(pluginCallbacks)
add_subdirectory(zoomCameraCmd)
```


for every single plugin in subdirectry:

```cmake
cmake_minimum_required(VERSION 3.26)
project(pickCmd)

message(${PROJECT_NAME})
# output a dll
add_library(${PROJECT_NAME} SHARED pickCmd.cpp)

# set the suffix of the output dynamic library to .mll, 
# and directly put it into ourself plugin dirs
set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
            SUFFIX ".mll"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${MAYA_PLUG_IN_PATH}/plug-ins") # debug

#set_target_properties(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX ".mll")

# These two libs are only the core modules of Maya. If you need to use other modules, you need to add other libs, such as OpenMayaUI.lib. You can find all of them in `${MAYA_PATH}/lib`
target_link_libraries(${PROJECT_NAME} Foundation OpenMaya)
```

## Build

