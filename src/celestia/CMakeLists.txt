set(CELESTIA_SOURCES
  catalogloader.h
  celestiacore.cpp
  celestiacore.h
  celestiastate.cpp
  celestiastate.h
  configfile.cpp
  configfile.h
  destination.cpp
  destination.h
  eclipsefinder.cpp
  eclipsefinder.h
  favorites.cpp
  favorites.h
  helper.cpp
  helper.h
  hud.cpp
  hud.h
  loaddso.cpp
  loaddso.h
  loadsso.cpp
  loadsso.h
  loadstars.cpp
  loadstars.h
  moviecapture.h
  scriptmenu.cpp
  scriptmenu.h
  textinput.cpp
  textinput.h
  textprintposition.cpp
  textprintposition.h
  timeinfo.h
  url.cpp
  url.h
  view.cpp
  view.h
  viewmanager.cpp
  viewmanager.h
  windowmetrics.cpp
  windowmetrics.h
)

if(ENABLE_FFMPEG)
  list(APPEND CELESTIA_SOURCES
    ffmpegcapture.cpp
    ffmpegcapture.h
  )
endif()

if(ENABLE_MINIAUDIO)
  list(APPEND CELESTIA_SOURCES
    audiosession.cpp
    audiosession.h
    miniaudiosession.h
  )

  if(APPLE)
    set(MINIAUDIO_SOURCES
      miniaudiosession.mm
    )
  else()
    set(MINIAUDIO_SOURCES
      miniaudiosession.cpp
    )
  endif()

  list(APPEND CELESTIA_SOURCES
    ${MINIAUDIO_SOURCES}
  )
endif()

set(CELESTIA_CORE_LIBS $<TARGET_OBJECTS:cel3ds>
                       $<TARGET_OBJECTS:celastro>
                       $<TARGET_OBJECTS:celengine>
                       $<TARGET_OBJECTS:celephem>
                       $<TARGET_OBJECTS:celimage>
                       $<TARGET_OBJECTS:celmath>
                       $<TARGET_OBJECTS:celmodel>
                       $<TARGET_OBJECTS:celrender>
                       $<TARGET_OBJECTS:celttf>
                       $<TARGET_OBJECTS:celutil>
)
if(TARGET celcompat)
  set(CELESTIA_CORE_LIBS ${CELESTIA_CORE_LIBS} $<TARGET_OBJECTS:celcompat>)
endif()

set(SCRIPT_LIBS $<TARGET_OBJECTS:celcommonscript> $<TARGET_OBJECTS:cellegacyscript>)
if(ENABLE_CELX)
  set(SCRIPT_LIBS ${SCRIPT_LIBS} $<TARGET_OBJECTS:celluascript>)
endif()

string(REPLACE "." ";" PROJECT_VERSION_LIST ${PROJECT_VERSION})
list(GET PROJECT_VERSION_LIST 0 PROJECT_VERSION_MAJOR)
list(GET PROJECT_VERSION_LIST 1 PROJECT_VERSION_MINOR)
set(CELSO "celestia-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

add_library(celestia SHARED ${CELESTIA_SOURCES}
                            ${SCRIPT_LIBS}
                            ${CELESTIA_CORE_LIBS}
)

set_target_properties(celestia PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  WINDOWS_EXPORT_ALL_SYMBOLS TRUE
)

if(ENABLE_SPICE)
  target_link_libraries(celestia CSPICE::CSPICE)
endif()

if(APPLE)
  target_link_libraries(celestia "-framework Foundation")
endif()

if(ENABLE_FFMPEG)
  target_link_libraries(celestia ${FFMPEG_LIBRARIES})
endif()

if(HAVE_MESHOPTIMIZER)
  target_link_libraries(celestia meshoptimizer::meshoptimizer)
endif()

if(ENABLE_LTO)
  set_target_properties(celestia PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
endif()

if(WIN32)
  install(
    TARGETS celestia
    RUNTIME_DEPENDENCIES
      PRE_EXCLUDE_REGEXES "^api-ms" "^ext-ms-"
      POST_EXCLUDE_REGEXES ".*system32/.*\\.dll$"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT core
  )
else()
  install(
    TARGETS celestia
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    NAMELINK_SKIP
    COMPONENT core
  )
endif()

add_subdirectory(qt5)
add_subdirectory(qt6)
add_subdirectory(sdl)
add_subdirectory(win32)
