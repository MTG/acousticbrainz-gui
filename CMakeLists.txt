cmake_minimum_required(VERSION 3.14)

# Allow the use of generator expressions in install(CODE) (requires cmake 3.14)
# https://cmake.org/cmake/help/latest/policy/CMP0087.html
cmake_policy(SET CMP0087 NEW)

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
target_link_libraries(acousticbrainz-gui Qt5::Widgets Qt5::Network)


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

if(WIN32)
	# Install Windows system libraries alongside application
	set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
	include(InstallRequiredSystemLibraries)

	# Find the windeployqt executable, used for copying necessary QT libs alongside the application
	get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
	get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
	find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

	# Install QT dependencies alongside application
	install(TARGETS acousticbrainz-gui BUNDLE)
	install(CODE "set(WINDEPLOYQT_EXECUTABLE \"${WINDEPLOYQT_EXECUTABLE}\")")
	install(CODE [=[
		execute_process(COMMAND "${WINDEPLOYQT_EXECUTABLE}" "bin/$<TARGET_FILE_NAME:acousticbrainz-gui>" WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}")
	]=])
endif()
