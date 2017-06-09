#CMakeLists.txt for RoperScientific camera

if(UNIX)
	  SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--add-needed")
ENDIF()

IF(WIN32)
	SET(ROPER_SDK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sdk/msvc)
ELSE()
	SET(ROPER_SDK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sdk/linux)
ENDIF()

SET(roper_srcs src/RoperScientificRoiCtrlObj.cpp src/RoperScientificSyncCtrlObj.cpp
			src/RoperScientificBinCtrlObj.cpp src/RoperScientificDetInfoCtrlObj.cpp
			src/RoperScientificInterface.cpp src/RoperScientificCamera.cpp)

FILE(STRINGS "VERSION" roper_vers)

ADD_LIBRARY(limaroper SHARED ${roper_srcs})

#INCLUDEs
#no need to check if unix or windows because path is already set
TARGET_INCLUDE_DIRECTORIES(limaroper PUBLIC "${ROPER_SDK_DIRECTORY}/include")
TARGET_INCLUDE_DIRECTORIES(limaroper PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

#LIBRARIES
IF(WIN32)
	#only for 32bits proc
	IF(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
		MESSAGE(FATAL_ERROR "camera can't be use under windows 64bits")
	ELSE()
		FIND_LIBRARY(ROPER_LIB_PVCAM NAMES Pvcam32.lib ${ROPER_SDK_DIRECTORY}/bin/x86)
		FIND_LIBRARY(ROPER_LIB_PVICL NAMES pv_icl32.lib ${ROPER_SDK_DIRECTORY}/bin/x86)
		FIND_LIBRARY(ROPER_LIB_FF NAMES pv_icl32.lib ${ROPER_SDK_DIRECTORY}/bin/x86)
		FIND_LIBRARY(ROPER_LIB_RSEMCALIB NAMES RSEMCalib.lib ${ROPER_SDK_DIRECTORY}/bin/x86)
	ENDIF()
	
ELSE()
	FIND_LIBRARY(ROPER_LIB_PVCAM pvcam)
	FIND_LIBRARY(ROPER_LIB_RAW1394 NAMES libraw1394.so.11 ${COMMON_PATH}/../lib/x86_64-linux-gnu)
	IF("${ROPER_LIB_PVCAM}" STREQUAL "ROPER_LIB_PVCAM-NOTFOUND")
		MESSAGE(FATAL_ERROR "roper scientifique : libpvcam not found, you can search it in cmake-gui or try to install it if it isn't here.")
	ENDIF()
	IF("${ROPER_LIB_RAW1394}" STREQUAL "ROPER_LIB_RAW1394-NOTFOUND")
		MESSAGE(FATAL_ERROR "roper scientifique : libraw1934 not found, you can search it in cmake-gui or try to install it if it isn't here.")
	ENDIF()
ENDIF()

TARGET_LINK_LIBRARIES(limaroper limacore)
IF(WIN32)
	TARGET_LINK_LIBRARIES(limaroper ${ROPER_LIB_PVCAM})
	TARGET_LINK_LIBRARIES(limaroper ${ROPER_LIB_PVICL})
	TARGET_LINK_LIBRARIES(limaroper ${ROPER_LIB_FF})
	TARGET_LINK_LIBRARIES(limaroper ${ROPER_LIB_RSEMCALIB})
ELSE()
	TARGET_LINK_LIBRARIES(limaroper ROPER_LIB_PVCAM)
	TARGET_LINK_LIBRARIES(limaroper ROPER_LIB_RAW1394)	
ENDIF()

SET_TARGET_PROPERTIES(limaroper PROPERTIES VERSION ${roper_vers} SOVERSION "1")

IF(WIN32)
    SET_TARGET_PROPERTIES(limaroper PROPERTIES PREFIX "lib")
ENDIF()

IF(WIN32)
    INSTALL(TARGETS limaroper DESTINATION lib)
ELSE()
    INSTALL(TARGETS limaroper LIBRARY DESTINATION lib)
ENDIF()


IF(COMPILE_SIP)
    SET(NAME "roperscientific")
    SET(INCLUDES)
    FILE(GLOB sipfiles
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/sip/*.sip")
    FOREACH(sipfile ${sipfiles})
        SET(INCLUDES
"${INCLUDES}
%Include ${sipfile}")
    ENDFOREACH()

    CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/sip/limamodules.sip.in sip/limaroper.sip)
    SET(SIP_CONCAT_PARTS 1)
    SET(SIP_INCLUDES ${SIP_INCLUDES}
        "${CMAKE_SOURCE_DIR}/third-party/Processlib/sip"
        "${CMAKE_BINARY_DIR}/sip/core"
        "${CMAKE_SOURCE_DIR}/third-party/Processlib/tasks/sip"
        "${CMAKE_SOURCE_DIR}/common/sip"
        "${CMAKE_SOURCE_DIR}/hardware/sip"
        "${CMAKE_SOURCE_DIR}/control/sip"
        "${CMAKE_SOURCE_DIR}/control/software_operation/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/sip")
    set(SIP_DISABLE_FEATURES WITH_CONFIG)
    set(SIP_CHECK_EXC ON)
    ADD_SIP_PYTHON_MODULE(limaroper ${CMAKE_CURRENT_BINARY_DIR}/sip/limaroper.sip)
    TARGET_INCLUDE_DIRECTORIES(python_module_limaroper PRIVATE
                               ${PYTHON_INCLUDE_DIRS}
                               "${CMAKE_SOURCE_DIR}/sip"
                               "${CMAKE_SOURCE_DIR}/sip/core")
    TARGET_LINK_LIBRARIES(python_module_limaroper limaroper)

    INSTALL(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Roperscientific")
ENDIF()
