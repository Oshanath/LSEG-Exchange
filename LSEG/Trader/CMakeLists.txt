SET (TRADER_SOURCES
	"src/trader_app.cpp"
)
add_executable (TRADER  ${TRADER_SOURCES})
target_include_directories(TRADER PUBLIC "include")
target_link_libraries(TRADER PRIVATE sockpp)