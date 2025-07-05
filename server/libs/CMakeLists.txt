
add_library(Server STATIC
    Server.cpp
)

add_library(Sessions STATIC
    Sessions.cpp
)

target_include_directories(Server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Sessions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Server
    PUBLIC
    Boost::system
    Boost::thread
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_link_libraries(Sessions
    PUBLIC
    Boost::system
    Boost::thread
    OpenSSL::SSL
    OpenSSL::Crypto
)

if(WIN32)
    target_link_libraries(Server PUBLIC ws2_32 wsock32)
    target_link_libraries(Sessions PUBLIC ws2_32 wsock32)
endif()

if(MSVC)
    target_compile_options(Server PRIVATE /W4)
    target_compile_options(Sessions PRIVATE /W4)
    target_compile_definitions(Server PRIVATE _WIN32_WINNT=0x0601)
    target_compile_definitions(Sessions PRIVATE _WIN32_WINNT=0x0601)
else()
    target_compile_options(Server PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(Sessions PRIVATE -Wall -Wextra -Wpedantic)
endif()

