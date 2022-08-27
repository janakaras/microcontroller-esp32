cmake_minimum_required(VERSION 3.5)


set(BZ_BUILD_TARGETS fw-a fw-b)
set(BZ_BUILD_TARGET "" CACHE STRING "The firmware to build")
if (NOT DEFINED BZ_BUILD_TARGET)
	message(FATAL_ERROR "BZ_BUILD_TARGET must be defined")
endif()
if (NOT "${BZ_BUILD_TARGET}" IN_LIST BZ_BUILD_TARGETS)
	message(FATAL_ERROR "Invalid value for BZ_BUILD_TARGET")
endif()

# The auto generated sdkconfig goes into the build directory
set(SDKCONFIG ${CMAKE_BINARY_DIR}/sdkconfig)
# The sdkconfig files that contain overrides of the default settings
set(SDKCONFIG_DEFAULTS
	${CMAKE_SOURCE_DIR}/sdkconfig.defaults
	)
set(EXTRA_COMPONENT_DIRS
	${CMAKE_SOURCE_DIR}/src/components
	)

if (BZ_BUILD_TARGET STREQUAL "fw-a")
	list(APPEND SDKCONFIG_DEFAULTS "${CMAKE_SOURCE_DIR}/sdkconfig.fw_a")
	list(APPEND EXTRA_COMPONENT_DIRS ${CMAKE_SOURCE_DIR}/src/fw-a)
	list(APPEND EXTRA_COMPONENT_DIRS ${CMAKE_SOURCE_DIR}/src/fw-a/components)
elseif(BZ_BUILD_TARGET STREQUAL "fw-b")
	list(APPEND SDKCONFIG_DEFAULTS "${CMAKE_SOURCE_DIR}/sdkconfig.fw_b")
	list(APPEND EXTRA_COMPONENT_DIRS ${CMAKE_SOURCE_DIR}/src/fw-b)
	list(APPEND EXTRA_COMPONENT_DIRS ${CMAKE_SOURCE_DIR}/src/fw-b/components)
endif()

# We don't want all the components to be built!
# Set main as the only required component.
# additional components will be included as needed based on dependency graph
set(COMPONENTS main
	# although esptool_py does not generate static library,
	# the component is needed for flashing related targets and file generation
	esptool_py
	)

# Use esp32c3 as default IDF_TARGET
set(IDF_TARGET "esp32c3" CACHE STRING "")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(bz-project)
