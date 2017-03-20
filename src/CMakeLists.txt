PROJECT(pwdmanlib)
cmake_minimum_required(VERSION 3.0)

# Version info
set(PWDMANLIB_MAJOR_VERSION 0)
set(PWDMANLIB_MINOR_VERSION 0)
set(PWDMANLIB_PATCH_VERSION 0)
set(PWDMANLIB_RELEASE_STAGE A)
set(PWDMANLIB_VERSION ${PWDMANLIB_MAJOR_VERSION}.${PWDMANLIB_MINOR_VERSION}.${PWDMANLIB_PATCH_VERSION}PWDMANLIB_RELEASE_STAGE)


# Set project layout variables
SET(PROJECT_ROOT "/pwdmanlib")

set(LOCAL_INCLUDES_DIR /usr/local/include)
set(LOCAL_LIBS_DIR /usr/local/lib)
set(LOCAL_OPT_DIR /opt)
set(INCLUDES_DIR /usr/include)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_ROOT}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_ROOT}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_ROOT}/bin)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_ROOT}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_ROOT}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_ROOT}/lib)

# Including dirs
include_directories(BEFORE ${INCLUDES_DIR}/openssl)
include_directories(/opt/openssl-1.1.0e)
#include_directories(${LOCAL_OPT_DIR})
# Including source files
include_directories(BEFORE ${PROJECT_ROOT}/src)

# Enable tests with CTest
enable_testing()

# Define src files
set(	SOURCE_FILES
#		library.cpp
#		gui/frontend.cpp

#		crypto/auth.cpp
#		crypto/crypto.cpp
		crypto/ssl_funcs.h

#		util/osinfo.c
#		util/vector.h
#		util/vector.c
		util/oop.h
#		util/oop.c
		util/constants.h
		util/arithmetic.h
		util/netops.h

		server/server.c
		server/server.h
		server/response.h
#		server/server_socket.c
		server/fawkes_proto.h

#		client/client.c
#		client/client_v2.c
#		client/client.h

#		test/vector-test.c
#		test/oop_test.c
#		test/db_test.c
#		test/test_response.c
#		test/test_fawkes_proto.c

		db/database.h

        validation/openssl_validation.h
)

set(	CLIENT_FILES
		util/oop.h
		util/constants.h
		util/arithmetic.h
		crypto/ssl_funcs.h
#		client/client.c
		client/client_v2.c
		client/client.h
)

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
include_directories("/usr/local/lib")
#link_directories("/usr/local/lib")

add_library(ssl SHARED IMPORTED) # or STATIC instead of SHARED
set_property(TARGET ssl PROPERTY IMPORTED_LOCATION "/usr/local/lib/libssl.so")
add_library(crypto SHARED IMPORTED) # or STATIC instead of SHARED
set_property(TARGET crypto PROPERTY IMPORTED_LOCATION "/usr/local/lib/libcrypto.so")
#include_directories("/opt/openssl-1.1.0e")

#find_package (my_library COMPONENTS REQUIRED component1 component2  OPTIONAL_COMPONENTS opt_component)

# Define build targets and link libraries
add_executable(server ${SOURCE_FILES})
target_include_directories(server PUBLIC /usr/include/openssl)
target_link_libraries(server
		PRIVATE ${Boost_LIBRARIES}
		PRIVATE ${PostgreSQL_LIBRARIES}
		PRIVATE ${cJSON_ROOT_DIR}
#		PRIVATE ${CryptoPP_ROOT_DIR}
#		PRIVATE ${Kore_ROOT_DIR}
#		PRIVATE ${POCO_LIBRARIES}
		PRIVATE ssl
		PRIVATE crypto
)

add_executable(client ${CLIENT_FILES})
target_include_directories(client PUBLIC /usr/include/openssl)
target_link_libraries(client
##		PRIVATE ${Boost_LIBRARIES}
##		PRIVATE ${PostgreSQL_LIBRARIES}
		PRIVATE ssl
		PRIVATE crypto
)

