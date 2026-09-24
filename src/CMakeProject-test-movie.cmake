set(TEST_MOVIE_SRC
    "arch/MovieTexture/MovieTexture_Test.cpp"
    "arch/MovieTexture/MovieTexture_FFMpeg.cpp"
    "arch/MovieTexture/MovieTexture.cpp"
    "arch/MovieTexture/MovieTexture_Null.cpp"
    "arch/RageDriver.cpp"
    "global.cpp"
    "LuaManager.cpp"
    "LuaBinding.cpp"
    "LuaReference.cpp"
    "EnumHelper.cpp"
    "XmlFile.cpp"
    "DateTime.cpp"
    "${SM_GENERATED_SRC_DIR}/verstub.cpp"
    ${SMDATA_ARCH_THREADS_SRC}
    ${SMDATA_RAGE_UTILS_SRC}
    ${SMDATA_RAGE_FILE_SRC}
    "RageLog.cpp"
    "RageTimer.cpp"
    "RageThreads.cpp"
    "RageMath.cpp"
    "RageTypes.cpp"
    "RageException.cpp"
    "RageSurface.cpp"
    "RageSurfaceUtils.cpp"
    "RageSurfaceUtils_Zoom.cpp"
    "RageTexture.cpp"
    "RageTextureID.cpp"
)

add_executable("test_movie_decoding" ${TEST_MOVIE_SRC})

set_property(TARGET "test_movie_decoding" PROPERTY CXX_STANDARD 17)
set_property(TARGET "test_movie_decoding" PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET "test_movie_decoding" PROPERTY CXX_EXTENSIONS ON)
set_property(TARGET "test_movie_decoding" PROPERTY FOLDER "Tests")

target_include_directories("test_movie_decoding" PUBLIC ${SM_INCLUDE_DIRS})
target_link_libraries("test_movie_decoding" ${SMDATA_LINK_LIB})

if(MSVC)
  set_target_properties("test_movie_decoding"
                        PROPERTIES COMPILE_FLAGS "${SM_COMPILE_FLAGS}")
endif()

target_compile_definitions("test_movie_decoding" PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_compile_definitions("test_movie_decoding" PRIVATE $<$<CONFIG:Release>:RELEASE>)
target_compile_definitions("test_movie_decoding" PRIVATE $<$<CONFIG:MinSizeRel>:MINSIZEREL>)
target_compile_definitions("test_movie_decoding" PRIVATE $<$<CONFIG:RelWithDebInfo>:RELWITHDEBINFO>)

if(WIN32)
  target_compile_definitions("test_movie_decoding" PRIVATE WINDOWS)
  target_compile_definitions("test_movie_decoding" PRIVATE _WINDOWS)
  target_compile_definitions("test_movie_decoding" PRIVATE _CRT_SECURE_NO_WARNINGS)
  target_compile_definitions("test_movie_decoding" PRIVATE _WINSOCK_DEPRECATED_NO_WARNINGS)
  target_compile_definitions("test_movie_decoding" PRIVATE NOMINMAX)
  target_compile_definitions("test_movie_decoding" PRIVATE WIN32_LEAN_AND_MEAN)
  target_compile_definitions("test_movie_decoding" PRIVATE VC_EXTRALEAN)

  sm_add_link_flag("test_movie_decoding" "/LIBPATH:\"${SM_EXTERN_DIR}/ffmpeg-w32/${SM_WIN32_ARCH}\"")
  sm_add_link_flag("test_movie_decoding" "/LIBPATH:\"$(WindowsSdkDir)Lib\\um\\${SM_WIN32_ARCH}\"")
  sm_add_link_flag("test_movie_decoding" "/ERRORREPORT:SEND")
  sm_add_link_flag("test_movie_decoding" "/SAFESEH:NO")
  sm_add_link_flag("test_movie_decoding" "/NOLOGO")
  sm_add_link_flag("test_movie_decoding" "/NODEFAULTLIB:wininet.lib")
  sm_add_link_flag("test_movie_decoding" "/NODEFAULTLIB:msimg32.lib")
  sm_add_link_flag("test_movie_decoding" "/NODEFAULTLIB:libci.lib")
  set_target_properties("test_movie_decoding"
                        PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt.lib")
  set_target_properties("test_movie_decoding"
                        PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
  set_target_properties("test_movie_decoding"
                        PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:CONSOLE")
  set_target_properties("test_movie_decoding"
                        PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
elseif(APPLE)
  target_compile_definitions("test_movie_decoding" PRIVATE MACOSX)
  target_compile_definitions("test_movie_decoding" PRIVATE _XOPEN_SOURCE)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    target_compile_definitions("test_movie_decoding" PRIVATE CPU_X86_64)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86" OR CMAKE_SYSTEM_PROCESSOR MATCHES "i686")
    target_compile_definitions("test_movie_decoding" PRIVATE CPU_X86)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    target_compile_definitions("test_movie_decoding" PRIVATE CPU_AARCH64)
  endif()
else() # Linux
  target_compile_definitions("test_movie_decoding" PRIVATE UNIX)
  if("${CMAKE_SYSTEM}" MATCHES "Linux")
    target_compile_definitions("test_movie_decoding" PRIVATE LINUX)
  endif()
  if(${HAS_PTHREAD})
    target_compile_definitions("test_movie_decoding" PRIVATE HAVE_LIBPTHREAD)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
      target_compile_definitions("test_movie_decoding" PRIVATE CPU_X86_64)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86" OR CMAKE_SYSTEM_PROCESSOR MATCHES "i686")
      target_compile_definitions("test_movie_decoding" PRIVATE CPU_X86)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
      target_compile_definitions("test_movie_decoding" PRIVATE CPU_AARCH64)
    endif()
  endif()
endif()

set_target_properties("test_movie_decoding"
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                 "${SM_PROGRAM_DIR}"
                                 RUNTIME_OUTPUT_DIRECTORY_RELEASE
                                 "${SM_PROGRAM_DIR}"
                                 RUNTIME_OUTPUT_DIRECTORY_DEBUG
                                 "${SM_PROGRAM_DIR}"
                                 RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL
                                 "${SM_PROGRAM_DIR}"
                                 RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO
                                 "${SM_PROGRAM_DIR}")

set_target_properties("test_movie_decoding"
                      PROPERTIES OUTPUT_NAME "test_movie_decoding"
                                 RELEASE_OUTPUT_NAME "test_movie_decoding"
                                 DEBUG_OUTPUT_NAME "test_movie_decoding-debug"
                                 MINSIZEREL_OUTPUT_NAME "test_movie_decoding-min-size"
                                 RELWITHDEBINFO_OUTPUT_NAME "test_movie_decoding-release-symbols")
