# CMakeList.txt : CMake project for LSEG, include source and define
# project specific logic here.
#

find_package(sockpp REQUIRED)

# Add source to this project's executable.
SET (TRADER_SOURCES
	"Trader/src/trader_app.cpp"
)
add_executable (TRADER  ${TRADER_SOURCES})
target_include_directories(TRADER PUBLIC "include" "Trader/include")
target_link_libraries(TRADER PRIVATE sockpp)

SET (EXCHANGE_SOURCES
	"Exchange/src/exchange_app.cpp"
)
add_executable (EXCHANGE  ${EXCHANGE_SOURCES})
target_include_directories(EXCHANGE PUBLIC "include" "Exchange/include")
target_link_libraries(EXCHANGE PRIVATE sockpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TRADER PROPERTY CXX_STANDARD 20)
  set_property(TARGET EXCHANGE PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
