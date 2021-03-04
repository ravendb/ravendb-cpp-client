if(NOT DEFINED ARCH)
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	  MESSAGE("+ 64 bit architecture") 
	  set(ARCH "x64") 
	 else() 
	  MESSAGE("+ 32 bit architecture")  
	  set(ARCH "x86") 
	endif()
endif() 

if(WIN32)
#this is most likely due to a bug in vcpkg configuration, we need to set these manually...
	set(VCPKG_TARGET_TRIPLET "${ARCH}-windows-static")	
	set(OS "win")
elseif(UNIX)
	set(VCPKG_TARGET_TRIPLET "${ARCH}-linux")
	set(OS "linux")
elseif(APPLE)
	set(VCPKG_TARGET_TRIPLET "${ARCH}-osx")
	set(OS "osx")
else()
#TODO: add arm triplets too, test the builds on arm dockers
	message(FATAL_ERROR "Can compile only on Unix, Windows or MacOS operating systems.")
endif()

message("Build target: ${VCPKG_TARGET_TRIPLET}")

#make sure to install and build vcpkg to enable them for the solution
if(NOT DEFINED ${CMAKE_TOOLCHAIN_FILE})

#first find proper path for vcpkg
  if(NOT DEFINED ENV{VCPKG_ROOT})
	if(WIN32)	
	  if(NOT EXISTS $ENV{HOMEDRIVE} OR NOT EXISTS $ENV{HOMEPATH})
		#fallback for Windows (admin users may not hasve home path defined if it is a network drive)
		set(VCPKG_ROOT "${PROJECT_SOURCE_DIR}/.vcpkg")
	  else()
		set(VCPKG_ROOT $ENV{HOMEDRIVE}$ENV{HOMEPATH}/vcpkg)
	  endif()
	else()
	  set(VCPKG_ROOT $ENV{HOME}/.vcpkg)
	endif()
  else()
	set(VCPKG_ROOT $ENV{VCPKG_ROOT})
  endif()


#then make sure vcpkg is up to date
  if(NOT EXISTS ${VCPKG_ROOT})
	message("Cloning vcpkg in ${VCPKG_ROOT}")
	execute_process(COMMAND git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT})
	# If a reproducible build is desired (and potentially old libraries are # ok), uncomment the
	# following line and pin the vcpkg repository to a specific githash.
	# execute_process(COMMAND git checkout 745a0aea597771a580d0b0f4886ea1e3a94dbca6 WORKING_DIRECTORY ${VCPKG_ROOT})
  else()
	# The following command has no effect if the vcpkg repository is in a detached head state.
	message("Auto-updating vcpkg in ${VCPKG_ROOT}")
	execute_process(COMMAND git pull WORKING_DIRECTORY ${VCPKG_ROOT})
	execute_process(COMMAND ${VCPKG_EXEC} update)
  endif()

  if(NOT EXISTS ${VCPKG_ROOT}/README.md)
	message(FATAL_ERROR "***** FATAL ERROR: Could not clone vcpkg, make sure git is installed and configured properly *****")
  endif()
   
  #compile vcpkg if this is a fresh installation
  if(WIN32)
	set(BOOST_INCLUDEDIR ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include)
	set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg.exe)
	set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.bat)
  else()
	set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg)
	set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.sh)
  endif()

  if(NOT EXISTS ${VCPKG_EXEC})
	message("Bootstrapping vcpkg in ${VCPKG_ROOT}")
	execute_process(COMMAND ${VCPKG_BOOTSTRAP} WORKING_DIRECTORY ${VCPKG_ROOT})
  endif()

  if(NOT EXISTS ${VCPKG_EXEC})
	message(FATAL_ERROR "***** FATAL ERROR: Could not bootstrap vcpkg *****")
  endif()

  #make sure to include third party stuff with CMake compilation
  set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")

  message(STATUS "***** Checking project third party dependencies in ${VCPKG_ROOT} *****")

  #actually install vcpkg libraries
  execute_process(COMMAND ${VCPKG_EXEC} install --triplet ${VCPKG_TARGET_TRIPLET} ${PROJECT_DEPENDENCIES} WORKING_DIRECTORY ${VCPKG_ROOT})
endif()

IF(CMAKE_BUILD_TYPE MATCHES Debug)
	message("++ This is a Debug build")
	set (CMAKE_CONFIGURATION_TYPES, "Debug" CACHE INTERNAL "")
	list (APPEND CMAKE_LIBRARY_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/debug/lib")
	list (APPEND CMAKE_LIBRARY_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/debug/lib/manual-link")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
	message("++ This is a Release build")
	set (CMAKE_CONFIGURATION_TYPES, "Release" CACHE INTERNAL "")
	list (APPEND CMAKE_LIBRARY_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib")
	list (APPEND CMAKE_LIBRARY_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib/manual-link")
ELSE()
	message("++ No build type specified, defaulting to release build")
	list (APPEND CMAKE_LIBRARY_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib")
	list (APPEND CMAKE_LIBRARY_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib/manual-link")
	set (CMAKE_BUILD_TYPE "Release" CACHE INTERNAL "")
ENDIF()

list (APPEND CMAKE_PREFIX_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/share")
list (APPEND CMAKE_INCLUDE_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include")

set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} CACHE INTERNAL "CMAKE_INCLUDE_PATH")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} CACHE INTERNAL "CMAKE_LIBRARY_PATH")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} CACHE INTERNAL "CMAKE_PREFIX_PATH")

set(ROOT_INCLUDE_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include" CACHE INTERNAL "")
