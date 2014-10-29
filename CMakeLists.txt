cmake_minimum_required(VERSION 2.6)

project(abzsubmit)
set(abzsubmit_VERSION 0.6)

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

set(abzsubmit_HEADERS
	checkabledirmodel.h
	progressdialog.h
	extractor.h
	mainwindow.h
	loadfilelisttask.h
	analyzefiletask.h
)
set(abzsubmit_SOURCES
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
#set(abzsubmit_UIS abzsubmit.ui)
#set(abzsubmit_RESOURCES extractor.qrc)

qt4_wrap_cpp(abzsubmit_MOC ${abzsubmit_HEADERS})
qt4_wrap_ui(abzsubmit_UIS_H ${abzsubmit_UIS})
qt4_add_resources(abzsubmit_RESOURCES_CPP ${abzsubmit_RESOURCES})

if(WIN32)
	set(abzsubmit_SOURCES ${abzsubmit_SOURCES} extractor.rc)
endif()

add_definitions(
	-D__STDC_LIMIT_MACROS
	-D__STDC_CONSTANT_MACROS
	-DVERSION="${abzsubmit_VERSION}"
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
    set(abzsubmit_ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/images/acoustid-fp.icns)
    set_source_files_properties(${abzsubmit_ICON_FILE}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)
	set(abzsubmit_SOURCES ${abzsubmit_SOURCES} ${abzsubmit_ICON_FILE})
endif()

add_executable(abzsubmit ${GUI_TYPE}
    ${abzsubmit_SOURCES}
    ${abzsubmit_MOC}
    ${abzsubmit_UIS_H}
    ${abzsubmit_RESOURCES_CPP}
)

set_target_properties(abzsubmit PROPERTIES
	OUTPUT_NAME acousticbrainz-gui
    MACOSX_BUNDLE_ICON_FILE acoustid-fp.icns
	MACOSX_BUNDLE_INFO_STRING "AcousticBrainz Extractor ${abzsubmit_VERSION}"
	MACOSX_BUNDLE_BUNDLE_NAME "AcousticBrainz Extractor"
)

target_link_libraries(abzsubmit
	${QT_LIBRARIES}
    ${QJSON_LIBRARIES}
)

if(APPLE)
	target_link_libraries(abzsubmit "-framework Accelerate -lz")
endif()

if(UNIX)
	install(FILES acousticbrainz-gui.desktop DESTINATION share/applications)
endif()

install(TARGETS abzsubmit DESTINATION ${BIN_INSTALL_DIR})
