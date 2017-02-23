PROJECT(pwdmanlib)
cmake_minimum_required(VERSION 3.0)

# Set project layout variables
SET(PROJECT_ROOT "/pwdmanlib/libraries")
SET(BII_BLOCKS_DIR blocks)
SET(BII_DEPS_DIR deps)
SET(BII_LIB_DIR lib)
SET(BII_CMAKE_DIR cmake-build-debug)
SET(BII_BIN_DIR bin)
set(CMAKE_MODULE_PATH "${PROJECT_ROOT}/blocks" "${PROJECT_ROOT}/deps")

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
		src/library.cpp
		src/server.c
		src/server.c
		src/frontend.cpp
		src/auth.cpp
		src/crypto.cpp
)
set(BOOST_ROOT_DIR "${PROJECT_ROOT}/deps/boost_1_60_0")
set(KORE_ROOT_DIR "${PROJECT_ROOT}/deps/kore")
set(POCO_ROOT_DIR "${PROJECT_ROOT}/deps/poco")

##### Define settings for dependencies #####

# Boost #
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(Boost_INCLUDE_DIR ${BOOST_ROOT_DIR})
set(Boost_LIBRARY_DIR ${BOOST_ROOT_DIR}/libs)

find_package(Boost 1.60.0)
if (NOT Boost_FOUND)
	message(FATAL_ERROR "Could not find Boost: ${BOOST_ROOT_DIR}")
else()
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
endif()

## PostrgreSQL #
set(PostgreSQL_ROOT )
set(PostgreSQL_INCLUDE_DIR "/usr/include/postgresql")
set(PostgreSQL_LIBRARY_DIR "/usr/lib/postgresql")
set(PostgreSQL_ADDITIONAL_VERSIONS "9.6.2" "9.5.6")

find_package(PostgreSQL)
if (NOT PostgreSQL_FOUND)
	message(FATAL_ERROR "Could not find PostgreSQL: ${PostgreSQL_INCLUDE_DIR}")
else()
	include_directories(${PostgreSQL_INCLUDE_DIRS})
	link_directories(${PostgreSQL_LIBRARY_DIRS})
endif()

# Crypto++ #
set(CryptoPP_ROOT_DIR "${PROJECT_ROOT}/deps/cryptopp565")

if(NOT EXISTS ${CryptoPP_ROOT_DIR})
	message(FATAL_ERROR "Could not find Crypto++: ${CryptoPP_ROOT_DIR}")
endif()

# Add libraries
#add_library (my_library STATIC|SHARED|MODULE ${SOURCE_FILES})
#find_package (my_library COMPONENTS REQUIRED component1 component2  OPTIONAL_COMPONENTS opt_component)
#target_link_libraries (my_target ${my_library};${another_library})

# Define build targets and link libraries
add_executable(main ${SOURCE_FILES})
target_link_libraries(main ${Boost_LIBRARY_DIRS})
target_link_libraries(main ${PostgreSQL_LIBRARIES})
target_include_directories(main PRIVATE ${CryptoPP_ROOT_DIR})
target_link_libraries(main ${CryptoPP_ROOT_DIR})


