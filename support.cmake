# Copyright jdknight

# targeting c11
set(CMAKE_C_STANDARD 11)

# force c support (ensures MSVC has been processed)
enable_language(C)

# enable warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")

# force unicode
add_definitions(-DUNICODE -D_UNICODE)
