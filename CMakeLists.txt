############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2017
# European Synchrotron Radiation Facility
# BP 220, Grenoble 38043
# FRANCE
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
#CMakeLists.txt for RoperScientific camera

if(UNIX)
	  SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--add-needed")
ENDIF()

IF(WIN32)

	SET(ROPER_SDK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sdk/msvc)
ELSE()
	SET(ROPER_SDK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sdk/linux)
ENDIF()

#SOURCEs
SET(roper_srcs src/RoperScientificRoiCtrlObj.cpp src/RoperScientificSyncCtrlObj.cpp
			src/RoperScientificBinCtrlObj.cpp src/RoperScientificDetInfoCtrlObj.cpp
			src/RoperScientificInterface.cpp src/RoperScientificCamera.cpp)

FILE(STRINGS "VERSION" roper_vers)

ADD_LIBRARY(limaroperscientific SHARED ${roper_srcs})

#INCLUDEs
TARGET_INCLUDE_DIRECTORIES(limaroperscientific PUBLIC "${ROPER_SDK_DIRECTORY}")
TARGET_INCLUDE_DIRECTORIES(limaroperscientific PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

IF(WIN32)
        TARGET_COMPILE_DEFINITIONS(limaroperscientific PRIVATE LIBROPERSCIENTIFIC_EXPORTS)
ENDIF()

#LIBRARIES
IF(WIN32)
	#only for 32bits
	IF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
		MESSAGE(FATAL_ERROR "camera can't be use under windows 64bits")
	ELSE()
		FIND_LIBRARY(ROPER_LIB_PVCAM Pvcam32 ${ROPER_SDK_DIRECTORY}/bin/x86)
		FIND_LIBRARY(ROPER_LIB_PVICL pv_icl32 ${ROPER_SDK_DIRECTORY}/bin/x86)
		FIND_LIBRARY(ROPER_LIB_FF ff ${ROPER_SDK_DIRECTORY}/bin/x86)
		FIND_LIBRARY(ROPER_LIB_RSEMCALIB RSEMCalib ${ROPER_SDK_DIRECTORY}/bin/x86)
	ENDIF()
	
ELSE()
	ADD_LIBRARY(raw1394 SHARED IMPORTED)
	SET_PROPERTY(TARGET raw1394 PROPERTY IMPORTED_LOCATION "/usr/lib/libraw1394.so.11")
	ADD_LIBRARY(pvcam SHARED IMPORTED)
	SET_PROPERTY(TARGET pvcam PROPERTY IMPORTED_LOCATION "/usr/lib/libpvcam.so")
ENDIF()

TARGET_LINK_LIBRARIES(limaroperscientific limacore)
IF(WIN32)
	TARGET_LINK_LIBRARIES(limaroperscientific ${ROPER_LIB_PVCAM})
	TARGET_LINK_LIBRARIES(limaroperscientific ${ROPER_LIB_PVICL})
	TARGET_LINK_LIBRARIES(limaroperscientific ${ROPER_LIB_FF})
	TARGET_LINK_LIBRARIES(limaroperscientific ${ROPER_LIB_RSEMCALIB})
ELSE()
	TARGET_LINK_LIBRARIES(limaroperscientific raw1394 pvcam)
ENDIF()

SET_TARGET_PROPERTIES(limaroperscientific PROPERTIES VERSION ${roper_vers} SOVERSION "1")

IF(WIN32)
    SET_TARGET_PROPERTIES(limaroperscientific PROPERTIES PREFIX "lib")
    INSTALL(TARGETS limaroperscientific DESTINATION lib)
ELSE()
    INSTALL(TARGETS limaroperscientific LIBRARY DESTINATION lib)
ENDIF()


IF(COMPILE_SIP)
    SET(NAME "roperscientific")
    include(CameraRunSIP)
    INSTALL(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/RoperScientific")
ENDIF()
