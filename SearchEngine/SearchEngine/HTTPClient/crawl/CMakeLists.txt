cmake_minimum_required(VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)
set (PostgreSQL_ADDITIONAL_VERSIONS 16)

project(crawl)

set(SOURCES_FILE crawl.cpp Crawler.h Crawler.cpp Indexer.h Indexer.cpp IniParser.h IniParser.cpp)
add_executable(${PROJECT_NAME} ${SOURCES_FILE})

#add files
file(GLOB MY_FILES "*.txt" "*.ini") 
file(COPY ${MY_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY config.ini
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
               
#libpqxx
add_subdirectory("C:/Program Files/libpqxx-7.8.1" libpqxx-build)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

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
				pqxx
				${Boost_LIBRARIES}
)

cmake_policy(SET CMP0074 NEW)

