set(DEMO_USAGE_NAME ${PROJECT_NAME}_usage_demo)
set(DEMO_RANDOM_MOVES_NAME ${PROJECT_NAME}_random_demo)

add_executable(${DEMO_USAGE_NAME}
	${PROJECT_SOURCE_DIR}/demo/usage.cpp
)

add_executable(${DEMO_RANDOM_MOVES_NAME}
	${PROJECT_SOURCE_DIR}/demo/random_moves.cpp
)

include_directories(include)

target_link_libraries(${DEMO_RANDOM_MOVES_NAME} PRIVATE libdog)
target_link_libraries(${DEMO_USAGE_NAME} PRIVATE libdog)
