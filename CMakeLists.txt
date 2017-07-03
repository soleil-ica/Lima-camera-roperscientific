###########################################################################
# This file is part of LImA, a Library for Image Acquisition
#
#  Copyright (C) : 2009-2017
#  European Synchrotron Radiation Facility
#  CS40220 38043 Grenoble Cedex 9 
#  FRANCE
# 
#  Contact: lima@esrf.fr
# 
#  This is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
# 
#  This software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
set(NAME "roperscientific")

if(UNIX)
	  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--add-needed")
endif()

if(WIN32)
	set(ROPER_SDK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sdk/msvc)
else()
	set(ROPER_SDK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sdk/linux)
endif()

#SOURCEs
set(${NAME}_srcs src/RoperScientificRoiCtrlObj.cpp src/RoperScientificSyncCtrlObj.cpp
			src/RoperScientificBinCtrlObj.cpp src/RoperScientificDetInfoCtrlObj.cpp
			src/RoperScientificInterface.cpp src/RoperScientificCamera.cpp)


add_library(lima${NAME} SHARED ${${NAME}_srcs})

#INCLUDEs
target_include_directories(lima${NAME} PUBLIC "${ROPER_SDK_DIRECTORY}")
target_include_directories(lima${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(WIN32)
        target_compile_definitions(lima${NAME} PRIVATE LIBROPERSCIENTIFIC_EXPORTS)
endif()

#LIBRARIES
if(WIN32)
	#only for 32bits
	if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
		message(FATAL_ERROR "camera can't be use under windows 64bits")
	else()
		find_library(ROPER_PVCAM_LIB Pvcam32 ${ROPER_SDK_DIRECTORY}/bin/x86)
		find_library(ROPER_PVICL_LIB pv_icl32 ${ROPER_SDK_DIRECTORY}/bin/x86)
		find_library(ROPER_FF_LIB ff ${ROPER_SDK_DIRECTORY}/bin/x86)
		find_library(ROPER_RSEMCALIB_LIB RSEMCalib ${ROPER_SDK_DIRECTORY}/bin/x86)
	endif()
	
else()
	add_library(raw1394 SHARED IMPORTED)
	set_property(TARGET raw1394 PROPERTY IMPORTED_LOCATION "/usr/lib/libraw1394.so.11")
	add_library(pvcam SHARED IMPORTED)
	set_property(TARGET pvcam PROPERTY IMPORTED_LOCATION "/usr/lib/libpvcam.so")
endif()

target_link_libraries(lima${NAME} limacore)
if(WIN32)
	target_link_libraries(lima${NAME} ${ROPER_PVCAM_LIB})
	target_link_libraries(lima${NAME} ${ROPER_PVICL_LIB})
	target_link_libraries(lima${NAME} ${ROPER_FF_LIB})
	target_link_libraries(lima${NAME} ${ROPER_RSEMCALIB_LIB})
else()
	target_link_libraries(lima${NAME} raw1394 pvcam)
endif()

limatools_set_library_soversion(lima${NAME} "VERSION")
if(WIN32)
    set_target_properties(lima${NAME} PROPERTIES PREFIX "lib")
    install(TARGETS lima${NAME} DESTINATION lib)
else()
    install(TARGETS lima${NAME} LIBRARY DESTINATION lib)
endif()


if(LIMA_ENABLE_PYTHON)
    limatools_run_sip_for_camera(${NAME})
    install(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/RoperScientific")
endif()
