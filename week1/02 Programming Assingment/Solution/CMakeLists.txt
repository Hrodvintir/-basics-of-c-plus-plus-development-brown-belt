project(hash)

add_executable(hash hash_set.cpp)

set_target_properties(
        hash PROPERTIES
        CXX_STANDART 17
        CXX_STANDART_REQUIRED ON
)
