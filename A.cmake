cmake_policy              (SET CMP0042 NEW)
cmake_policy              (SET CMP0068 NEW)

if(DEFINED ENV{SRC})
    set(CMAKE_INSTALL_PREFIX $ENV{SRC}/silver-import)
elseif(CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    message(STATUS "CMAKE_INSTALL_PREFIX set to system location, using our own build location to avoid conflicts")
    set(CMAKE_INSTALL_PREFIX  ${CMAKE_BINARY_DIR}/silver-import)
else()
    message(STATUS "using user-defined CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY   ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE   ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE   ONLY)
set(CMAKE_PREFIX_PATH                   ${CMAKE_INSTALL_PREFIX})
set(CMAKE_LIBRARY_PATH                  ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_SKIP_BUILD_RPATH              FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH      TRUE)
set(CMAKE_INSTALL_RPATH                 ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH   TRUE)
set(CMAKE_FIND_USE_SYSTEM_PATH          FALSE)

set                       (DEBUG_PROJECTS "" CACHE STRING "comma-separated list of projects to debug in silver-import")
if(DEBUG_PROJECTS)
    file                  (WRITE "${CMAKE_BINARY_DIR}/debug.txt" "${DEBUG_PROJECTS}")
endif()

# rebuild is an all, however you may rebuild with make REBUILD=llvm
add_custom_target         (rebuild COMMAND ${CMAKE_COMMAND} -E env REBUILD=all ${CMAKE_COMMAND} -E env bash ${CMAKE_SOURCE_DIR}/../A/import.sh ${CMAKE_INSTALL_PREFIX} --deps ${CMAKE_SOURCE_DIR}/deps --debug ${CMAKE_BINARY_DIR}/debug.txt WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_custom_target         (script COMMAND ${CMAKE_COMMAND} -E env bash ${CMAKE_SOURCE_DIR}/../A/import.sh ${CMAKE_INSTALL_PREFIX} --deps ${CMAKE_SOURCE_DIR}/deps --debug ${CMAKE_BINARY_DIR}/debug.txt WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# copy all res to build folder
file(GLOB_RECURSE RES_FILES "${CMAKE_SOURCE_DIR}/src/res/*")
foreach(RES_FILE ${RES_FILES})
    file(COPY ${RES_FILE} DESTINATION ${CMAKE_BINARY_DIR})
endforeach()
include_directories       (./src ${CMAKE_INSTALL_PREFIX}/include)
link_directories          (${CMAKE_INSTALL_PREFIX}/lib)
file                      (GLOB src "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
add_compile_options       (-fno-exceptions -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)
add_compile_options       (
    -Wno-write-strings
    -Wno-compare-distinct-pointer-types
    -Wno-deprecated-declarations
    -Wno-incompatible-pointer-types
    -Wfatal-errors
    -fPIC)

#add_compile_options      (-fsanitize=address -fno-omit-frame-pointer -g)
#add_link_options         (-fsanitize=address)

add_compile_options       (-I${CMAKE_INSTALL_PREFIX}/include)
set                       (CMAKE_C_STANDARD             11)
set                       (CMAKE_CXX_STANDARD           17)
set                       (CMAKE_CXX_STANDARD_REQUIRED  ON)
set                       (CMAKE_C_COMPILER             clang-18)
set                       (CMAKE_CXX_COMPILER           clang-18++)

# we make apps or libs
if(app)
    add_executable        (${PROJECT_NAME}              ${src})
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)
else()
    add_library           (${PROJECT_NAME} SHARED       ${src})
    install               (TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
    install               (FILES src/${PROJECT_NAME} DESTINATION include)
endif()

add_dependencies          (${PROJECT_NAME}              script)
set                       (L ${CMAKE_INSTALL_PREFIX}/lib)

target_include_directories(${PROJECT_NAME} PRIVATE . ./src)
