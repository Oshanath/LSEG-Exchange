get_filename_component(PARENT_DIR ../ ABSOLUTE)
include_directories(${PARENT_DIR}/include)

SET (EXCHANGE_SOURCES
	"src/exchange_app.cpp"
)
add_executable (EXCHANGE  ${EXCHANGE_SOURCES})
target_include_directories(EXCHANGE PUBLIC "include")
target_link_libraries(EXCHANGE PRIVATE sockpp)