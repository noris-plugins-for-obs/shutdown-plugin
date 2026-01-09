cmake_minimum_required(VERSION 3.12)

project(shutdown-plugin VERSION 0.3.0)

set(PLUGIN_AUTHOR "Norihiro Kamae")

set(PLUGIN_URL "https://obsproject.com/forum/resources/shutdown-plugin.1725/")
set(ID_PREFIX "net.nagater.obs-shutdown-plugin.")
set(MACOS_BUNDLEID "net.nagater.obs-shutdown-plugin")
set(MACOS_PACKAGE_UUID   "EBAA5BAD-711D-45E4-A721-583791B9AB00")
set(MACOS_INSTALLER_UUID "98C3A489-4744-48D2-94E5-A82405586247")
set(WIN_INSTALLER_UUID   "1A81D505-34BE-49C4-8070-BE7DC1DBFD65")

set(LINUX_MAINTAINER_EMAIL "norihiro@nagater.net")

option(ENABLE_COVERAGE "Enable coverage option for GCC" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
	find_package(libobs REQUIRED)
	find_package(obs-frontend-api REQUIRED)
	include(cmake/ObsPluginHelpers.cmake)
	add_library(OBS::frontend-api ALIAS OBS::obs-frontend-api)
	find_qt(VERSION ${QT_VERSION} COMPONENTS Widgets Core Gui)
endif()

include(cmake/versionconfig.cmake)

configure_file(
	src/plugin-macros.h.in
	plugin-macros.generated.h
)

set(PLUGIN_SOURCES
	src/plugin-main.c
	src/shutdown-plugin.cpp
)

add_library(${PROJECT_NAME} MODULE ${PLUGIN_SOURCES})

target_link_libraries(${PROJECT_NAME}
	OBS::libobs
	OBS::frontend-api
	Qt::Core
	Qt::Widgets
	Qt::Gui
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

if(OS_WINDOWS)
	# Enable Multicore Builds and disable FH4 (to not depend on VCRUNTIME140_1.DLL when building with VS2019)
	if (MSVC)
		add_definitions(/MP /d2FH4-)
	endif()

	target_link_libraries(${PROJECT_NAME} OBS::w32-pthreads)

	set(LICENSE_DESTINATION "data/obs-plugins/${PROJECT_NAME}")
endif()

if(OS_LINUX)
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
	target_link_options(${PROJECT_NAME} PRIVATE -Wl,-z,defs)

	set(LICENSE_DESTINATION ${CMAKE_INSTALL_DOCDIR})

	if(ENABLE_COVERAGE)
		target_compile_options(${PROJECT_NAME} PRIVATE -coverage -fprofile-update=atomic)
		target_link_options(${PROJECT_NAME} PRIVATE -coverage)
	endif()
endif()

if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fvisibility=default")

	set(LICENSE_DESTINATION "${PROJECT_NAME}.plugin/Contents/Resources")

	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
	set(MACOSX_PLUGIN_GUI_IDENTIFIER "${MACOS_BUNDLEID}")
	set(MACOSX_PLUGIN_BUNDLE_VERSION "${PROJECT_VERSION}")
	set(MACOSX_PLUGIN_SHORT_VERSION_STRING "1")
endif()

file(GENERATE OUTPUT .gitignore CONTENT "*\n")

setup_plugin_target(${PROJECT_NAME})

option(INSTALL_LICENSE_FILES "Install license files" ON)
if(INSTALL_LICENSE_FILES)
	install(FILES
		LICENSE
		DESTINATION ${LICENSE_DESTINATION}
	)
endif()

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
	configure_file(
		installer/installer-Windows.iss.in
		installer-Windows.generated.iss
	)

	configure_file(
		ci/ci_includes.sh.in
		ci/ci_includes.generated.sh
	)
	configure_file(
		ci/ci_includes.cmd.in
		ci/ci_includes.generated.cmd
	)

	configure_file(
		installer/installer-macOS.pkgproj.in
		installer-macOS.generated.pkgproj
	)
	configure_file(
		installer/installer-macOS-post.sh.in
		installer-macOS-post.generated.sh
	)
endif()
