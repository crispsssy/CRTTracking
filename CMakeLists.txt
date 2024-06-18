cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(CRTTracking)
set(CMAKE_CXX_STANDARD 14)

find_package(ROOT REQUIRED)

add_library(Tracking ${CMAKE_CURRENT_SOURCE_DIR}/src/CDCHit.cxx
		     ${CMAKE_CURRENT_SOURCE_DIR}/src/CDCGeom.cxx
		     ${CMAKE_CURRENT_SOURCE_DIR}/src/EventLoop.cxx
		     ${CMAKE_CURRENT_SOURCE_DIR}/src/Preprocess.cxx
		     ${CMAKE_CURRENT_SOURCE_DIR}/src/EventDisplay.cxx
		     ${CMAKE_CURRENT_SOURCE_DIR}/src/CDCLineCandidate.cxx
		     ${CMAKE_CURRENT_SOURCE_DIR}/src/HoughHandler.cxx
		     ${CMAKE_CURRENT_SOURCE_DIR}/src/RuntimeParameter.cxx
		     ${CMAKE_CURRENT_SOURCE_DIR}/src/TrackFitHandler.cxx)
target_include_directories(Tracking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
         		   		   ${ROOT_INCLUDE_DIRS})
target_link_libraries(Tracking ${ROOT_LIBRARIES} ROOT::Spectrum)

add_executable(CRTTracking ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cxx)
target_link_libraries(CRTTracking PUBLIC Tracking)

install(TARGETS CRTTracking
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
