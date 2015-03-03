cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bunsan_process)

find_package(BunsanCMake CONFIG REQUIRED)

if(UNIX)
    set(platform_tests unix)
    set(platform_sources
        src/process/detail/unix/path.cpp
        src/process/detail/unix/execute.cpp
        src/process/detail/unix/executor.cpp
        src/process/detail/unix/descriptor.cpp
        src/process/detail/unix/open.cpp
    )
else()
    message(SEND_ERROR "Platform is not supported")
endif()

bunsan_add_shared_library(${PROJECT_NAME}
    src/process/context.cpp
    src/process/error.cpp
    src/process/execute.cpp
    src/process/aliases.cpp
    src/process/path.cpp

    ${platform_sources}

    src/interprocess/sync/file_lock.cpp
    src/interprocess/sync/file_guard.cpp
)
bunsan_use_bunsan_package(${PROJECT_NAME} bunsan_common bunsan_common)
bunsan_use_boost(${PROJECT_NAME}
    thread
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})
bunsan_install_project()

bunsan_include_tests()
