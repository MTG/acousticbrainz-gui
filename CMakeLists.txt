cmake_minimum_required(VERSION 2.8.11)

project(acousticbrainz-gui)
set(acousticbrainz-gui_VERSION 0.2dev)

set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
set(EXEC_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "Installation prefix for executables and object code libraries" FORCE)
set(BIN_INSTALL_DIR ${EXEC_INSTALL_PREFIX}/bin CACHE PATH "Installation prefix for user executables" FORCE)
set(LIB_INSTALL_DIR ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX} CACHE PATH  "Installation prefix for object code libraries" FORCE)
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include CACHE PATH "Installation prefix for C header files" FORCE)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets)
find_package(Qt5Network)

if(CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL MinSizeRel OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
	add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif(CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL MinSizeRel OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)

set(acousticbrainz-gui_HEADERS
	checkabledirmodel.h
	progressdialog.h
	extractor.h
	mainwindow.h
	loadfilelisttask.h
	analyzefiletask.h
)
set(acousticbrainz-gui_SOURCES
	checkabledirmodel.cpp
	progressdialog.cpp
	extractor.cpp
	mainwindow.cpp
	main.cpp
	loadfilelisttask.cpp
	analyzefiletask.cpp
	updatelogfiletask.cpp
	crc.c
	gzip.cpp
)
#set(acousticbrainz-gui_UIS acousticbrainz-gui.ui)
set(acousticbrainz-gui_RESOURCES extractor.qrc)

if(WIN32)
	set(acousticbrainz-gui_SOURCES ${acousticbrainz-gui_SOURCES} extractor.rc)
endif()

add_definitions(
	-D__STDC_LIMIT_MACROS
	-D__STDC_CONSTANT_MACROS
	-DVERSION="${acousticbrainz-gui_VERSION}"
)

include(CheckFunctionExists)

if(WIN32)
set(GUI_TYPE WIN32)
endif(WIN32)

if(APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
    set(acousticbrainz-gui_ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/images/acousticbrainz-gui.icns)
    set_source_files_properties(${acousticbrainz-gui_ICON_FILE}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)
	set(acousticbrainz-gui_SOURCES ${acousticbrainz-gui_SOURCES} ${acousticbrainz-gui_ICON_FILE})
endif()

add_executable(acousticbrainz-gui ${GUI_TYPE} ${acousticbrainz-gui_SOURCES})
qt5_use_modules(acousticbrainz-gui Widgets Network)


set_target_properties(acousticbrainz-gui PROPERTIES
	OUTPUT_NAME acousticbrainz-gui
    MACOSX_BUNDLE_ICON_FILE acousticbrainz-gui.icns
	MACOSX_BUNDLE_INFO_STRING "AcousticBrainz Extractor ${acousticbrainz-gui_VERSION}"
	MACOSX_BUNDLE_BUNDLE_NAME "AcousticBrainz Extractor"
)


if(APPLE)
	target_link_libraries(acousticbrainz-gui "-framework Accelerate -lz")
endif()

if(UNIX)
	install(FILES acousticbrainz-gui.desktop DESTINATION share/applications)
endif()

if(APPLE)
    install(TARGETS acousticbrainz-gui BUNDLE DESTINATION .)
elseif(UNIX)
    install(TARGETS acousticbrainz-gui RUNTIME DESTINATION ${BIN_INSTALL_DIR})
endif()
