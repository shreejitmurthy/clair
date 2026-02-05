cmake_minimum_required(VERSION 3.29)

add_executable(example
    main.cpp
)

target_link_libraries(example PRIVATE
    clair
)
