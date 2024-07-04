# CMakeList.txt : CMake project for SearchEngine, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (SearchEngine "SearchEngine.cpp" "SearchEngine.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SearchEngine PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
cmake_minimum_required(VERSION 3.8)
project (SearchEngine)
add_subdirectory(HTTPClient)
add_subdirectory(HTTPServer)

#add files
#file(GLOB MY_FILES "*.txt") file(COPY ${MY_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#BOOST LIBS
find_package(Boost 1.82.00 REQUIRED)

target_include_directories(${PROJECT_NAME}
			PUBLIC
				$<INSTALL_INTERFACE:include>
				$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
				${Boost_INCLUDE_DIRS}
			PRIVATE
)

target_link_libraries(${PROJECT_NAME}
			PUBLIC
				${Boost_LIBRARIES}
)
