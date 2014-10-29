cmake_minimum_required(VERSION 2.6)

project(acousticbrainz-gui)
set(acousticbrainz-gui_VERSION 0.1)

set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
set(EXEC_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "Installation prefix for executables and object code libraries" FORCE)
set(BIN_INSTALL_DIR ${EXEC_INSTALL_PREFIX}/bin CACHE PATH "Installation prefix for user executables" FORCE)
set(LIB_INSTALL_DIR ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX} CACHE PATH  "Installation prefix for object code libraries" FORCE)
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include CACHE PATH "Installation prefix for C header files" FORCE)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork REQUIRED)
find_package(QJSON REQUIRED)

include(${QT_USE_FILE})

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
    utils.cpp
	crc.c
	gzip.cpp
)
#set(acousticbrainz-gui_UIS acousticbrainz-gui.ui)
set(acousticbrainz-gui_RESOURCES extractor.qrc)

qt4_wrap_cpp(acousticbrainz-gui_MOC ${acousticbrainz-gui_HEADERS})
qt4_wrap_ui(acousticbrainz-gui_UIS_H ${acousticbrainz-gui_UIS})
qt4_add_resources(acousticbrainz-gui_RESOURCES_CPP ${acousticbrainz-gui_RESOURCES})

if(WIN32)
	set(acousticbrainz-gui_SOURCES ${acousticbrainz-gui_SOURCES} extractor.rc)
endif()

add_definitions(
	-D__STDC_LIMIT_MACROS
	-D__STDC_CONSTANT_MACROS
	-DVERSION="${acousticbrainz-gui_VERSION}"
)

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
)

set(CMAKE_REQUIRED_LIBRARIES
	${EXTRA_LIBS})
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

add_executable(acousticbrainz-gui ${GUI_TYPE}
    ${acousticbrainz-gui_SOURCES}
    ${acousticbrainz-gui_MOC}
    ${acousticbrainz-gui_UIS_H}
    ${acousticbrainz-gui_RESOURCES_CPP}
)

set_target_properties(acousticbrainz-gui PROPERTIES
	OUTPUT_NAME acousticbrainz-gui
    MACOSX_BUNDLE_ICON_FILE acousticbrainz-gui.icns
	MACOSX_BUNDLE_INFO_STRING "AcousticBrainz Extractor ${acousticbrainz-gui_VERSION}"
	MACOSX_BUNDLE_BUNDLE_NAME "AcousticBrainz Extractor"
)

target_link_libraries(acousticbrainz-gui
	${QT_LIBRARIES}
    ${QJSON_LIBRARIES}
)

if(APPLE)
	target_link_libraries(acousticbrainz-gui "-framework Accelerate -lz")
endif()

if(UNIX)
	install(FILES acousticbrainz-gui.desktop DESTINATION share/applications)
endif()

install(TARGETS acousticbrainz-gui DESTINATION ${BIN_INSTALL_DIR})
