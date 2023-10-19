# CMakeList.txt : CMake project for LSEG, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (TRADER "trader_app.cpp" "order.h" "rapidcsv.h")

add_executable (EXCHANGE "exchange_app.cpp" "order.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TRADER PROPERTY CXX_STANDARD 20)
  set_property(TARGET EXCHANGE PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
