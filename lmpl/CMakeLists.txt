cmake_minimum_required(VERSION 3.8)

project(lmpl)

if (UNIX)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
endif ()

if (MSVC)
  set(
    COMPILER_DEFINITIONS

    _USE_MATH_DEFINES NOMINMAX
  )
  set(
    COMPILER_INTERFACE_OPTIONS

    /wd4250 /wd4008 /wd4996
  )
elseif(MINGW)
  set(
    COMPILER_DEFINITIONS

    _WIN32_WINNT=0x0602
  )
endif ()

if (WIN32)
  set(
    PLATFORM_SOURCES

    src/windows/windows_error.cpp
    src/windows/windows_message_to_string.cpp
    src/windows/misc.cpp
    src/windows/xinput.cpp
    src/windows/windows_display.cpp
    src/windows/create_display.cpp
    src/windows/windows_window.cpp
  )

  set(
    PLATFORM_LIBS

    kernel32.lib user32.lib gdi32.lib
    winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib
    oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
  )

  set(
    PLATFORM_DEFINITIONS

    UNICODE _UNICODE
    VK_USE_PLATFORM_WIN32_KHR
    _AMD64_=100 _M_X64=100 _WIN64=1 _WIN32=1
  )
elseif (UNIX)
  set(
    PLATFORM_SOURCES

    src/linux/create_display.cpp
    src/linux/xcb/xcb_display.cpp
    src/linux/wayland/wayland.cpp
    src/linux/xcb/xcb_window.cpp
  )

  set(
    XCB_LIBS

    xcb
    xcb-util
    xcb-randr
  )

  set(
    LINUX_DISPLAY_LIBS

    ${XCB_LIBS}
    wayland-client
  )

  set(
    PLATFORM_LIBS

    ${LINUX_DISPLAY_LIBS}
    ${GTKMM_LIBRARIES}
  )

  set(
    PLATFORM_LINK_DIRS

    ${GTKMM_LIBRARY_DIRS}
  )

  set(
    PLATFORM_DEFINITIONS

    VK_USE_PLATFORM_XCB_KHR
    VK_USE_PLATFORM_WAYLAND_KHR
  )
  set(
    PLATFORM_INCLUDES

    ${GTKMM_INCLUDE_DIRS}
  )
endif()

set(
  SOURCES

  ${PLATFORM_SOURCES}
)

set(
  INCLUDE_DIRS

  include
  ${fmt_INCLUDE_DIRS}
  ${PLATFORM_INCLUDES}
  ${scope_guard_INCLUDES}
  ${lmlib_INCLUDES}
)

set(
  LIBS

  ${PLATFORM_LIBS}

  ${fmt_LIBRARIES}
  ${TBB_LIBRARIES}
  lmlib
)

set(
  DEFINITIONS

  ${PLATFORM_DEFINITIONS}
  ${COMPILER_DEFINITIONS}
  VK_PROTOTYPES
  "-DFMT_NO_FMT_STRING_ALIAS=0"
)

set(
  INTERFACE_DEFINITIONS

  ${COMPILER_INTERFACE_DEFINITIONS}
)

set(
  OPTIONS

  ${COMPILER_OPTIONS}
)

set(
  INTERFACE_OPTIONS

  ${COMPILER_INTERFACE_OPTIONS}
)

add_library(
  lmpl
  SHARED

  ${SOURCES}
  src/keyboard.cpp
)

target_include_directories(
  lmpl

  PUBLIC
  ${INCLUDE_DIRS}

  PRIVATE
  ${lmlib_INCLUDES}
  ${scope_guard_INCLUDES}
)

target_compile_definitions(
  lmpl

  PUBLIC
  ${DEFINITIONS}

  INTERFACE
  ${INTERFACE_DEFINITIONS}

  PRIVATE
  SPDLOG_ACTIVE_LEVEL=${LM_ENGINE_LOGGING_LEVEL}
)

target_compile_options(
  lmpl

  PUBLIC
  ${OPTIONS}

  INTERFACE
  ${INTERFACE_OPTIONS}
)

target_link_libraries(
  lmpl
  PRIVATE
  ${LIBS}
  PUBLIC
)

target_link_directories(
  lmpl
  PRIVATE

  ${PLATFORM_LINK_DIRS}
)
target_include_directories(
  lmpl

  PUBLIC
  ${INCLUDE_DIRS}
  include
)
