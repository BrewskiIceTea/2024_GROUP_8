cmake_minimum_required(VERSION 3.16)
project(VRModelViewer VERSION 0.1 LANGUAGES CXX)


# ----------------------------------------------------------------------------
# Enable Qt automatic tools
# ----------------------------------------------------------------------------

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ----------------------------------------------------------------------------
# C++ standard
# ----------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX
    "${CMAKE_BINARY_DIR}/Release"
    CACHE PATH "Where to install everything" FORCE)


# ----------------------------------------------------------------------------
# VTK: use external OpenGL
# ----------------------------------------------------------------------------

set(VTK_MODULE_USE_EXTERNAL_OpenGL ON CACHE BOOL "Use external/system OpenGL" FORCE)
find_package(VTK REQUIRED)

# ----------------------------------------------------------------------------
# Qt: widgets
# ----------------------------------------------------------------------------

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# ----------------------------------------------------------------------------
# windeployqt path
# ----------------------------------------------------------------------------

# Use QT_WINDEPLOY_PATH set in CMake GUI
set(QT_WINDEPLOY_PATH "" CACHE FILEPATH "Path to windeployqt executable")
if(WIN32 AND NOT QT_WINDEPLOY_PATH)
    message(WARNING "QT_WINDEPLOY_PATH not set. Please set it to the path of windeployqt.exe in CMake GUI.")
endif()

# ----------------------------------------------------------------------------
# OpenVR
# ----------------------------------------------------------------------------

set(OpenVR_INCLUDE_DIR "C:/OpenVR/headers")
find_library(OpenVR_LIBRARY
    NAMES openvr_api openvr_api64
    PATHS "C:/OpenVR/lib/win64"
)
if(NOT OpenVR_LIBRARY)
    message(FATAL_ERROR "Could not find OpenVR library in C:/OpenVR/lib/win64")
endif()

# ----------------------------------------------------------------------------
# Source files
# ----------------------------------------------------------------------------

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ModelPart.cpp
    ModelPart.h
    ModelPartList.cpp
    ModelPartList.h
    optiondialog.cpp
    optiondialog.ui
    optiondialog.h
    filterdialog.cpp
    filterdialog.ui
    filterdialog.h
    VRRenderThread.cpp
    VRRenderThread.h
    icons.qrc
)

# ----------------------------------------------------------------------------
# Create executable
# ----------------------------------------------------------------------------

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(VRModelViewer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_finalize_executable(VRModelViewer)
else()
    add_executable(VRModelViewer ${PROJECT_SOURCES})
endif()

# ----------------------------------------------------------------------------
# Link libraries
# ----------------------------------------------------------------------------

target_include_directories(VRModelViewer PRIVATE ${OpenVR_INCLUDE_DIR})
target_link_libraries(VRModelViewer PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${VTK_LIBRARIES}
    ${OpenVR_LIBRARY}
)

# ----------------------------------------------------------------------------
# Installation rules
# ----------------------------------------------------------------------------

include(GNUInstallDirs)

# Install the application
install(TARGETS VRModelViewer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Always copy the OpenVR DLL
install(FILES "C:/OpenVR/bin/win64/openvr_api.dll"
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Copy VTK release DLLs
file(GLOB VTK_DLLS "${VTK_DYNAMIC_LIB_DIR}/*.dll")
install(FILES ${VTK_DLLS}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ----------------------------------------------------------------------------
# Deploy Qt dependencies using QT_WINDEPLOY_PATH
# ----------------------------------------------------------------------------

if(WIN32 AND QT_WINDEPLOY_PATH)
    # Use file(GENERATE) to create a batch script that will run windeployqt
    # This avoids issues with CMake's execute_process during install
	file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/deploy_qt.bat" CONTENT
"@echo off
echo Running Qt deployment tool...
\"${QT_WINDEPLOY_PATH}\" \"%~dp0/Release/VRModelViewer.exe\" --release --compiler-runtime --no-translations
if %ERRORLEVEL% NEQ 0 (
    echo Qt deployment failed with error %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)
echo Qt deployment completed successfully
")


    # Add custom command to run the batch file after build
    add_custom_command(TARGET VRModelViewer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Running Qt deployment tool..."
        COMMAND ${CMAKE_BINARY_DIR}/deploy_qt.bat
        COMMENT "Deploying Qt dependencies with windeployqt"
    )
    
    # Note: We're using POST_BUILD instead of during INSTALL step
    # This ensures the Qt DLLs are deployed right after successful build
    # Install step will simply copy everything from bin directory
else()
    if(WIN32)
        message(WARNING "QT_WINDEPLOY_PATH not set. Qt dependencies will not be automatically deployed.")
    endif()
endif()

# ----------------------------------------------------------------------------
# CPack for NSIS installer
# ----------------------------------------------------------------------------
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "VRModelViewer")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Christian Ogatis, Kai Brewster, Ben Harle")
set(CPACK_PACKAGE_EXECUTABLES "VRModelViewer" "VRModelViewer")
include(CPack)