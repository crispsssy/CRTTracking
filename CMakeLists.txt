cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(CRTTracking)
set(CMAKE_CXX_STANDARD 14)

find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
link_directories(${ROOT_LIBRARY_DIR})

#Make dictionaries
ROOT_GENERATE_DICTIONARY(${CMAKE_CURRENT_SOURCE_DIR}/src/G__EventDisplay ${CMAKE_CURRENT_SOURCE_DIR}/src/EventDisplay.hxx LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/src/EventDisplay_LinkDef.h)
ROOT_GENERATE_DICTIONARY(${CMAKE_CURRENT_SOURCE_DIR}/src/G__STL_Dictionary ${CMAKE_CURRENT_SOURCE_DIR}/src/stl_dictionary.hxx LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/src/stl_dictionary_LinkDef.h)

set(SOURCES     ${CMAKE_CURRENT_SOURCE_DIR}/src/CDCHit.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/CDCGeom.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/CDCWireInfo.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/EventLoop.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/Preprocess.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/EventDisplay.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/CDCLineCandidate.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/HoughHandler.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/RuntimeParameter.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/TrackFitHandler.cxx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TrackFitMinimizerBase.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/TrackFitMinimizer.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/CalibInfo.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/G__EventDisplay.cxx
		${CMAKE_CURRENT_SOURCE_DIR}/src/G__STL_Dictionary.cxx)
add_library(Tracking SHARED ${SOURCES})
target_include_directories(Tracking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(Tracking ${ROOT_LIBRARIES} ROOT::Gui)

add_executable(CRTTracking ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cxx)
target_link_libraries(CRTTracking PUBLIC Tracking)

target_compile_definitions(Tracking PRIVATE XT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/xt")

install(TARGETS CRTTracking
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
