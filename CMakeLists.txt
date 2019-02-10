cmake_minimum_required(VERSION 3.10)
project(
    WSDL2 
    VERSION 0.1
    DESCRIPTION "C++ SDL2 wrapper"
    LANGUAGES CXX
)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)
# if ("${CMAKE_SYSTEM}" MATCHES "Linux")
#     list(INSERT CMAKE_MODULE_PATH 0 /usr/local/lib/cmake/)
# endif ()


############################
# find dependencies
find_package(SDL2 REQUIRED)
find_package(MM
    PATHS
        $<$<CMAKE_SYSTEM:Linux>:/usr/local/lib/cmake/>
)

############################
# libwsdl2
add_library(wsdl2 STATIC
    event.cpp
    video.cpp
    wrapsdl2.cpp
)

add_library(WSDL2::wsdl2 ALIAS wsdl2)

# set up headers
target_include_directories(wsdl2
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_DIR}
)

target_compile_options(wsdl2
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:
            -pedantic -Wall -Wextra -Wcast-qual -Wcast-align -Wpointer-arith
            -Winit-self -Wshadow -Wswitch-enum -Wredundant-decls -Wfloat-equal
            -Wundef -Wvla -Wconversion -Wstrict-aliasing
        >
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# configuration of SDL2 under windows
if (MSVC)
	# setting up SDL for windows
    set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")

    # Support both 32 and 64 bit builds
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2main.lib")
    else ()
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2main.lib")
    endif ()

    string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

endif()

target_compile_definitions(wsdl2
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

# set compiler features
# For a complete list see:
# https://cmake.org/cmake/help/latest/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html
target_compile_features(wsdl2
    PRIVATE
        cxx_std_17
)

# set up required libraries
target_link_libraries(wsdl2
    PUBLIC
        SDL2
)

if (MM_LIB)
    message("Building with libmm support")
    target_compile_definitions(wsdl2
        PRIVATE
            WSDL2_USE_MM
    )

    target_link_libraries(wsdl2
        PUBLIC
            MM
    )
else ()
    message("Building without libmm support")
endif ()

############################
# installation
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/WSDL2)

install(TARGETS wsdl2
    EXPORT wsdl2-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set_target_properties(wsdl2 PROPERTIES EXPORT_NAME WSDL2)

# install public headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# export targets
install(EXPORT wsdl2-targets
    FILE
        WSDL2Targets.cmake
    NAMESPACE
        WSDL2::
    DESTINATION
        ${INSTALL_CONFIGDIR}
)

# create ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/WSDL2ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/WSDL2Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/WSDL2Config.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# install config, configversion
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/WSDL2Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/WSDL2ConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

############################
# export from the build tree
export(EXPORT wsdl2-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/WSDL2Targets.cmake
    NAMESPACE WSDL2::
)

export(PACKAGE WSDL2)
