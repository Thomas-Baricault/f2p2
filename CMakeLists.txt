cmake_minimum_required(VERSION 3.20)


if(NOT NAME)
    message(FATAL_ERROR "NAME is required")
endif()

if(NOT VERSION)
    message(FATAL_ERROR "VERSION is required")
endif()

if(NOT SRC)
    message(FATAL_ERROR "SRC is required")
endif()


project(${NAME}
    VERSION "${VERSION}"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(NOT DEFINED PACKAGES)
    set(PACKAGES "")
endif()

if(NOT DEFINED TARGETS)
    set(TARGETS "")
endif()


if(ENV)
    set(
        CMAKE_INSTALL_PREFIX
        "${ENV}"
        CACHE PATH "Install prefix"
        FORCE
    )

    list(APPEND CMAKE_PREFIX_PATH "${ENV}")
endif()


separate_arguments(PACKAGES)
separate_arguments(TARGETS)
separate_arguments(SRC)


list(TRANSFORM SRC REPLACE "^(.+)$" "src/\\1.cpp")

add_executable(${NAME}
    ${SRC}
)


target_include_directories(${NAME}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if (ENV)

    target_include_directories(${NAME}
        PRIVATE
            "${ENV}/include"
    )

endif()


target_compile_options(${NAME}
    PRIVATE
        -Wall
        -Werror
        -Wextra
)

target_compile_definitions(${NAME}
    PRIVATE
        VERSION="${VERSION}"
)


foreach(PACKAGE ${PACKAGES})
    find_package(${PACKAGE} REQUIRED)
endforeach()

target_link_libraries(${NAME}
    PRIVATE
        ${TARGETS}
)
