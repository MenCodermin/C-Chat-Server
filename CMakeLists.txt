cmake_minimum_required(VERSION 3.10)
project(SimpleChatServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Detect OS
if(WIN32)
    message(STATUS "Configuring for Windows")
    set(PLATFORM_LIBS ws2_32 iphlpapi)

    # Manually set PostgreSQL paths
    set(PostgreSQL_ROOT "C:/Program Files/PostgreSQL/17")  # Adjust version if needed
    set(PostgreSQL_INCLUDE_DIR "${PostgreSQL_ROOT}/include")
    set(PostgreSQL_LIBRARY "${PostgreSQL_ROOT}/lib/libpq.lib")

else()
    message(STATUS "Configuring for Linux")
    set(PLATFORM_LIBS pthread)

    # Manually set PostgreSQL paths for Linux
    set(PostgreSQL_INCLUDE_DIR "/usr/include/postgresql")
    set(PostgreSQL_LIBRARY "/usr/lib/x86_64-linux-gnu/libpq.so")
endif()

# Define source files
set(SOURCES
    src/main.cpp
    src/server.cpp
    src/client.cpp
    src/db.cpp
)

# Include directories
include_directories(src/include ${PostgreSQL_INCLUDE_DIR})
link_directories(${PostgreSQL_LIBRARY})

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${PLATFORM_LIBS} ${PostgreSQL_LIBRARY})

# Output build information
message(STATUS "PostgreSQL Include: ${PostgreSQL_INCLUDE_DIR}")
message(STATUS "PostgreSQL Libraries: ${PostgreSQL_LIBRARY}")
