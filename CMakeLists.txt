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

if(UNIX)
	  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--add-needed")
endif()

if(WIN32)

	set(ROPER_SDK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sdk/msvc)
else()
	set(ROPER_SDK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sdk/linux)
endif()

#SOURCEs
set(roper_srcs src/RoperScientificRoiCtrlObj.cpp src/RoperScientificSyncCtrlObj.cpp
			src/RoperScientificBinCtrlObj.cpp src/RoperScientificDetInfoCtrlObj.cpp
			src/RoperScientificInterface.cpp src/RoperScientificCamera.cpp)

file(STRINGS "VERSION" roper_vers)

add_library(limaroperscientific SHARED ${roper_srcs})

#INCLUDEs
target_include_directories(limaroperscientific PUBLIC "${ROPER_SDK_DIRECTORY}")
target_include_directories(limaroperscientific PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(WIN32)
        target_compile_definitions(limaroperscientific PRIVATE LIBROPERSCIENTIFIC_EXPORTS)
endif()

#LIBRARIES
if(WIN32)
	#only for 32bits
	if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
		message(FATAL_ERROR "camera can't be use under windows 64bits")
	else()
		find_library(ROPER_LIB_PVCAM Pvcam32 ${ROPER_SDK_DIRECTORY}/bin/x86)
		find_library(ROPER_LIB_PVICL pv_icl32 ${ROPER_SDK_DIRECTORY}/bin/x86)
		find_library(ROPER_LIB_FF ff ${ROPER_SDK_DIRECTORY}/bin/x86)
		find_library(ROPER_LIB_RSEMCALIB RSEMCalib ${ROPER_SDK_DIRECTORY}/bin/x86)
	endif()
	
else()
	add_library(raw1394 SHARED IMPORTED)
	set_property(TARGET raw1394 PROPERTY IMPORTED_LOCATION "/usr/lib/libraw1394.so.11")
	add_library(pvcam SHARED IMPORTED)
	set_property(TARGET pvcam PROPERTY IMPORTED_LOCATION "/usr/lib/libpvcam.so")
endif()

target_link_libraries(limaroperscientific limacore)
if(WIN32)
	target_link_libraries(limaroperscientific ${ROPER_LIB_PVCAM})
	target_link_libraries(limaroperscientific ${ROPER_LIB_PVICL})
	target_link_libraries(limaroperscientific ${ROPER_LIB_FF})
	target_link_libraries(limaroperscientific ${ROPER_LIB_RSEMCALIB})
else()
	target_link_libraries(limaroperscientific raw1394 pvcam)
endif()

set_target_properties(limaroperscientific PROPERTIES VERSION ${roper_vers} SOVERSION "1")

if(WIN32)
    set_target_properties(limaroperscientific PROPERTIES PREFIX "lib")
    install(TARGETS limaroperscientific DESTINATION lib)
else()
    install(TARGETS limaroperscientific LIBRARY DESTINATION lib)
endif()


if(COMPILE_SIP)
    set(NAME "roperscientific")
    include(CameraRunSIP)
    install(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/RoperScientific")
endif()
