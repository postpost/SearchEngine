cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)

project (HTTPServer)
add_executable(${PROJECT_NAME} main.cpp "Server.h" "Server.cpp")

#BOOST LIBS
find_package(Boost 1.82.00 REQUIRED)

#libpqxx
#add_subdirectory("C:/Program Files/libpqxx-7.8.1" libpqxx-build)

target_include_directories(${PROJECT_NAME}
			PUBLIC
				$<INSTALL_INTERFACE:include>
				$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
				${Boost_INCLUDE_DIRS}
				./HTTPClient/crawl
				libpqxx-build
			PRIVATE
)

target_link_libraries(${PROJECT_NAME}
			PUBLIC
				pqxx
				crawlerLib
				${Boost_LIBRARIES}
				#${PROJECT_NAME} HTTPClient\\crawl
)

