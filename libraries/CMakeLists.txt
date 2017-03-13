PROJECT(pwdmanlib)
cmake_minimum_required(VERSION 3.0)

# Version info
set(PWDMANLIB_MAJOR_VERSION 0)
set(PWDMANLIB_MINOR_VERSION 0)
set(PWDMANLIB_PATCH_VERSION 0)
set(PWDMANLIB_RELEASE_STAGE A)
set(PWDMANLIB_VERSION ${PWDMANLIB_MAJOR_VERSION}.${PWDMANLIB_MINOR_VERSION}.${PWDMANLIB_PATCH_VERSION}PWDMANLIB_RELEASE_STAGE)


# Set project layout variables
SET(PROJECT_ROOT "/pwdmanlib/libraries")
SET(BII_BLOCKS_DIR blocks)
SET(BII_DEPS_DIR deps)
SET(BII_LIB_DIR lib)
SET(BII_CMAKE_DIR cmake-build-debug)
SET(BII_BIN_DIR bin)
set(CMAKE_MODULE_PATH "${PROJECT_ROOT}/blocks" "${PROJECT_ROOT}/deps")

set(LOCAL_INCLUDES_DIR /usr/local/include)
set(LOCAL_LIBS_DIR /usr/local/lib)
set(LOCAL_OPT_DIR /opt)

# inclusion of general biicode macros, biicode.cmake
INCLUDE(biicode.cmake)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_ROOT}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_ROOT}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_ROOT}/bin)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_ROOT}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_ROOT}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_ROOT}/lib)

# Including /blocks and /deps directories
INCLUDE_DIRECTORIES(${PROJECT_ROOT}/blocks)
INCLUDE_DIRECTORIES(${PROJECT_ROOT}/deps)

ADD_DEFINITIONS(-DBIICODE)
SET(BIICODE_ENV_DIR "/root/.biicode")

# Including source files
include_directories(BEFORE ${PROJECT_ROOT}/src)

# Enable tests with CTest
enable_testing()
# Creating a custom target which will associate test targets
add_custom_target(biitest)

# Inclusion of the blocks affected by the build

# Define vars and locations
set(	SOURCE_FILES
#		src/library.cpp
		src/server/server.c
		src/gui/frontend.cpp
		src/crypto/auth.cpp
		src/crypto/crypto.cpp
#		src/util/osinfo.c
		src/util/vector.h
		src/util/vector.c
		src/server/server.h
		src/util/oop.h
		src/util/oop.c
		src/test/vector-test.c
#		src/client/client.c
#		src/client/client.h
		src/server/server_socket.c
		src/test/oop_test.c
		src/util/constants.h
		src/util/arithmetic.h
		src/server/fawkes_proto.h src/test/test_fawkes_proto.c)

##### Define settings for dependencies #####

# Boost #
set(BOOST_ROOT_DIR "/usr/local/boost_1_63_0")
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(Boost_INCLUDE_DIR ${BOOST_ROOT_DIR})
set(Boost_LIBRARY_DIR ${BOOST_ROOT_DIR}/libs)

find_package(Boost 1.63.0)
if (NOT Boost_FOUND)
	message(FATAL_ERROR "Could not find Boost: ${BOOST_ROOT_DIR}")
else()
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
endif()

# PostrgreSQL #
set(PostgreSQL_ROOT "/usr/include/postgresql")
set(PostgreSQL_INCLUDE_DIR "/usr/include/postgresql")
set(PostgreSQL_LIBRARY_DIR "/usr/lib/postgresql")
set(PostgreSQL_ADDITIONAL_VERSIONS "9.6.2" "9.5.6")
set(PostgreSQL_TYPE_INCLUDE_DIR "/usr/include/postgresql/9.6/server/catalog")

find_package(PostgreSQL REQUIRED)
if (NOT PostgreSQL_FOUND)
	message(FATAL_ERROR "Could not find PostgreSQL: ${PostgreSQL_INCLUDE_DIR}")
else()
	include_directories(${PostgreSQL_INCLUDE_DIRS})
	link_directories(${PostgreSQL_LIBRARY_DIRS})
endif()

# cJSON #
set(cJSON_ROOT_DIR "${LOCAL_INCLUDES_DIR}/cjson")

if(NOT EXISTS ${cJSON_ROOT_DIR})
	message(FATAL_ERROR "Could not find cJSON: ${cJSON_ROOT_DIR}")
else()
	include_directories(PRIVATE ${cJSON_ROOT_DIR})
endif()

# Crypto++ #
#set(CryptoPP_ROOT_DIR "${PROJECT_ROOT}/deps/cryptopp565")
#
#if(NOT EXISTS ${CryptoPP_ROOT_DIR})
#	message(FATAL_ERROR "Could not find Crypto++: ${CryptoPP_ROOT_DIR}")
#else()
#	include_directories(PRIVATE ${CryptoPP_ROOT_DIR})
#endif()

# Kore #
#set(KORE_ROOT_DIR "${PROJECT_ROOT}/deps/kore")
#
#if(NOT EXISTS ${Kore_ROOT_DIR})
#	message(FATAL_ERROR "Could not find Kore: ${KORE_ROOT_DIR}")
#else()
#	include_directories(${Kore_INCLUDE_DIRS})
#	link_directories(${Kore_INCLUDE_DIRS})
#endif()

# Poco #
set(POCO_ROOT_DIR "${PROJECT_ROOT}/deps/poco-1.7.8")
#find_package(poco REQUIRED)

#if(NOT EXISTS ${POCO_ROOT_DIR})
#	message(FATAL_ERROR "Could not find Poco: ${POCO_ROOT_DIR}")
#else()
#	include_directories(${POCO_INCLUDE_DIRS})
#	link_directories(${POCO_LIBRARY_DIRS})
#endif()

# Add libraries
include_directories(${LOCAL_LIBS_DIR})
#add_library (my_library STATIC|SHARED|MODULE ${SOURCE_FILES})
#find_package (my_library COMPONENTS REQUIRED component1 component2  OPTIONAL_COMPONENTS opt_component)
#target_link_libraries (my_target ${my_library};${another_library})

# Define build targets and link libraries
add_executable(main ${SOURCE_FILES})
target_link_libraries(main
		PRIVATE ${Boost_LIBRARIES}
		PRIVATE ${PostgreSQL_LIBRARIES}
		PRIVATE ${cJSON_ROOT_DIR}
#		PRIVATE ${CryptoPP_ROOT_DIR}
#		PRIVATE ${Kore_ROOT_DIR}
#		PRIVATE ${POCO_LIBRARIES}
		)

