set(DEMO_NAME ${PROJECT_NAME}_demo)

add_executable(${DEMO_NAME}
	${PROJECT_SOURCE_DIR}/demo/main.cpp
)

include_directories(include)

target_link_libraries(${DEMO_NAME} PRIVATE libdog)
