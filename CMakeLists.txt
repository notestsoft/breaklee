cmake_minimum_required(VERSION 3.17.5)

project(breaklee VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG=1")

option(CONFIG_BUILD_TARGET_AUCTION_SVR "Build Auction Server" OFF)
option(CONFIG_BUILD_TARGET_BREAKLEE "Build Breaklee" ON)
option(CONFIG_BUILD_TARGET_CHAT_SVR "Build Chat Server" OFF)
option(CONFIG_BUILD_TARGET_LOGIN_SVR "Build Login Server" ON)
option(CONFIG_BUILD_TARGET_MASTER_DBAGENT "Build Master DB Agent" ON)
option(CONFIG_BUILD_TARGET_MASTER_SVR "Build Master Server" ON)
option(CONFIG_BUILD_TARGET_PARTY_SVR "Build Party Server" ON)
option(CONFIG_BUILD_TARGET_WORLD_SVR "Build World Server" ON)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

find_package(libuv REQUIRED)
find_package(lua REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(ODBC REQUIRED)

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/)

file(COPY ${PROJECT_SOURCE_DIR}/ServerData/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/Data/)
file(COPY ${PROJECT_SOURCE_DIR}/Scripts/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/Scripts/)

set(CORE_LIB_DIR ${PROJECT_SOURCE_DIR}/CoreLib)
set(NET_LIB_DIR ${PROJECT_SOURCE_DIR}/NetLib)
set(RUNTIME_DATA_LIB_DIR ${PROJECT_SOURCE_DIR}/RuntimeDataLib)
set(RUNTIME_LIB_DIR ${PROJECT_SOURCE_DIR}/RuntimeLib)

file(GLOB CORE_LIB_HEADERS ${CORE_LIB_DIR}/*.h)
file(GLOB CORE_LIB_SOURCES ${CORE_LIB_DIR}/*.c)

file(GLOB NET_LIB_HEADERS ${NET_LIB_DIR}/*.h)
file(GLOB NET_LIB_SOURCES ${NET_LIB_DIR}/*.c)

file(GLOB RUNTIME_DATA_LIB_HEADERS ${RUNTIME_DATA_LIB_DIR}/*.h)
file(GLOB RUNTIME_DATA_LIB_SOURCES ${RUNTIME_DATA_LIB_DIR}/*.c)

file(GLOB RUNTIME_LIB_HEADERS ${RUNTIME_LIB_DIR}/*.h)
file(GLOB RUNTIME_LIB_SOURCES ${RUNTIME_LIB_DIR}/*.c)

set(SHARED_HEADERS_DIR ${PROJECT_SOURCE_DIR}/Shared)
file(GLOB SHARED_HEADERS ${SHARED_HEADERS_DIR}/*.h)

add_library(CoreLib STATIC ${CORE_LIB_HEADERS} ${CORE_LIB_SOURCES})
target_link_libraries(CoreLib PRIVATE ${OpenSSL_LIBRARIES} ${zlib_LIBRARIES} ${libuv_LIBRARIES})
target_include_directories(CoreLib PUBLIC ${OpenSSL_INCLUDE_DIR} ${zlib_INCLUDE_DIR} ${libuv_INCLUDE_DIR})
target_link_libraries(CoreLib PRIVATE ${ODBC_LIBRARIES})
target_include_directories(CoreLib PUBLIC ${ODBC_INCLUDE_DIRS})

add_library(NetLib STATIC ${NET_LIB_HEADERS} ${NET_LIB_SOURCES})
target_link_libraries(NetLib PRIVATE CoreLib lua::lua)
target_include_directories(NetLib PUBLIC ${PROJECT_SOURCE_DIR} ${lua_INCLUDE_DIR})

add_library(RuntimeDataLib STATIC ${RUNTIME_DATA_LIB_HEADERS} ${RUNTIME_DATA_LIB_SOURCES})
target_include_directories(RuntimeDataLib PUBLIC ${PROJECT_SOURCE_DIR})

add_library(RuntimeLib STATIC ${RUNTIME_LIB_HEADERS} ${RUNTIME_LIB_SOURCES})
target_link_libraries(RuntimeLib PRIVATE CoreLib RuntimeDataLib ${LUA_LIBRARIES})
target_include_directories(RuntimeLib PUBLIC ${PROJECT_SOURCE_DIR} ${lua_INCLUDE_DIR})

if(CONFIG_BUILD_TARGET_AUCTION_SVR)
    set(AUCTION_SVR_DIR ${PROJECT_SOURCE_DIR}/AuctionSvr)
    
    file(GLOB AUCTION_SVR_HEADERS ${AUCTION_SVR_DIR}/*.h)
    file(GLOB AUCTION_SVR_SOURCES ${AUCTION_SVR_DIR}/*.c)

    add_executable(AuctionSvr ${AUCTION_SVR_HEADERS} ${AUCTION_SVR_SOURCES} ${SHARED_HEADERS})
    target_include_directories(AuctionSvr PUBLIC ${PROJECT_SOURCE_DIR} ${SHARED_HEADERS_DIR})
    target_link_libraries(AuctionSvr PRIVATE NetLib CoreLib)
    
    if(WIN32)
        target_link_libraries(AuctionSvr PRIVATE ${AUCTION_SVR_DIR}/main.res)
    endif()
endif()

if(CONFIG_BUILD_TARGET_BREAKLEE)
    set(BREAKLEE_DIR ${PROJECT_SOURCE_DIR}/Breaklee)

    file(GLOB BREAKLEE_HEADERS ${BREAKLEE_DIR}/*.h)
    file(GLOB BREAKLEE_SOURCES ${BREAKLEE_DIR}/*.c)
    
    add_executable(breaklee ${BREAKLEE_HEADERS} ${BREAKLEE_SOURCES} ${SHARED_HEADERS})
    target_include_directories(breaklee PUBLIC ${PROJECT_SOURCE_DIR} ${SHARED_HEADERS_DIR})
    target_link_libraries(breaklee PRIVATE NetLib CoreLib)
endif()

if(CONFIG_BUILD_TARGET_CHAT_SVR)
    set(CHAT_SVR_DIR ${PROJECT_SOURCE_DIR}/ChatSvr)
    
    file(GLOB CHAT_SVR_HEADERS ${CHAT_SVR_DIR}/*.h)
    file(GLOB CHAT_SVR_SOURCES ${CHAT_SVR_DIR}/*.c)

    add_executable(ChatSvr ${CHAT_SVR_HEADERS} ${CHAT_SVR_SOURCES} ${SHARED_HEADERS})
    target_include_directories(ChatSvr PUBLIC ${PROJECT_SOURCE_DIR} ${SHARED_HEADERS_DIR})
    target_link_libraries(ChatSvr PRIVATE NetLib CoreLib)
    
    if(WIN32)
        target_link_libraries(ChatSvr PRIVATE ${CHAT_SVR_DIR}/main.res)
    endif()
endif()

if(CONFIG_BUILD_TARGET_LOGIN_SVR)
    set(LOGIN_SVR_DIR ${PROJECT_SOURCE_DIR}/LoginSvr)
    
    file(GLOB LOGIN_SVR_HEADERS ${LOGIN_SVR_DIR}/*.h)
    file(GLOB LOGIN_SVR_SOURCES ${LOGIN_SVR_DIR}/*.c)

    add_executable(LoginSvr ${LOGIN_SVR_HEADERS} ${LOGIN_SVR_SOURCES} ${SHARED_HEADERS})
    target_include_directories(LoginSvr PUBLIC ${PROJECT_SOURCE_DIR} ${OPENSSL_INCLUDE_DIR} ${SHARED_HEADERS_DIR} ${LUA_INCLUDE_DIR})
    target_link_libraries(LoginSvr PRIVATE NetLib CoreLib)

    if(WIN32)
        target_link_libraries(LoginSvr PRIVATE ${LOGIN_SVR_DIR}/main.res)
    endif()
endif()

if(CONFIG_BUILD_TARGET_MASTER_DBAGENT)
    set(MASTER_DBAGENT_DIR ${PROJECT_SOURCE_DIR}/MasterDBAgent)
    
    file(GLOB MASTER_DBAGENT_HEADERS ${MASTER_DBAGENT_DIR}/*.h)
    file(GLOB MASTER_DBAGENT_SOURCES ${MASTER_DBAGENT_DIR}/*.c)

    add_executable(MasterDBAgent ${MASTER_DBAGENT_HEADERS} ${MASTER_DBAGENT_SOURCES} ${SHARED_HEADERS})
    target_include_directories(MasterDBAgent PUBLIC ${PROJECT_SOURCE_DIR} ${SHARED_HEADERS_DIR})
    target_link_libraries(MasterDBAgent PRIVATE NetLib CoreLib RuntimeLib RuntimeDataLib)
    
    if(WIN32)
        target_link_libraries(MasterDBAgent PRIVATE ${MASTER_DBAGENT_DIR}/main.res)
    endif()
endif()

if(CONFIG_BUILD_TARGET_MASTER_SVR)
    set(MASTER_SVR_DIR ${PROJECT_SOURCE_DIR}/MasterSvr)
    
    file(GLOB MASTER_SVR_HEADERS ${MASTER_SVR_DIR}/*.h)
    file(GLOB MASTER_SVR_SOURCES ${MASTER_SVR_DIR}/*.c)

    add_executable(MasterSvr ${MASTER_SVR_HEADERS} ${MASTER_SVR_SOURCES} ${SHARED_HEADERS})
    target_include_directories(MasterSvr PUBLIC ${PROJECT_SOURCE_DIR} ${SHARED_HEADERS_DIR})
    target_link_libraries(MasterSvr PRIVATE NetLib CoreLib RuntimeLib RuntimeDataLib)
    
    if(WIN32)
        target_link_libraries(MasterSvr PRIVATE ${MASTER_SVR_DIR}/main.res)
    endif()
endif()

if(CONFIG_BUILD_TARGET_PARTY_SVR)
    set(PARTY_SVR_DIR ${PROJECT_SOURCE_DIR}/PartySvr)
    
    file(GLOB PARTY_SVR_HEADERS ${PARTY_SVR_DIR}/*.h)
    file(GLOB PARTY_SVR_SOURCES ${PARTY_SVR_DIR}/*.c)

    add_executable(PartySvr ${PARTY_SVR_HEADERS} ${PARTY_SVR_SOURCES} ${SHARED_HEADERS})
    target_include_directories(PartySvr PUBLIC ${PROJECT_SOURCE_DIR} ${SHARED_HEADERS_DIR})
    target_link_libraries(PartySvr PRIVATE NetLib CoreLib RuntimeLib RuntimeDataLib)
    
    if(WIN32)
        target_link_libraries(PartySvr PRIVATE ${PARTY_SVR_DIR}/main.res)
    endif()
endif()

if(CONFIG_BUILD_TARGET_WORLD_SVR)
    set(WORLD_SVR_DIR ${PROJECT_SOURCE_DIR}/WorldSvr)
    
    file(GLOB WORLD_SVR_HEADERS ${WORLD_SVR_DIR}/*.h)
    file(GLOB WORLD_SVR_SOURCES ${WORLD_SVR_DIR}/*.c)
        
    add_executable(WorldSvr ${WORLD_SVR_HEADERS} ${WORLD_SVR_SOURCES} ${SHARED_HEADERS})
    target_include_directories(WorldSvr PUBLIC ${PROJECT_SOURCE_DIR} ${SHARED_HEADERS_DIR})
    target_link_libraries(WorldSvr PRIVATE NetLib CoreLib RuntimeLib RuntimeDataLib)

    if(WIN32)
        target_link_libraries(WorldSvr PRIVATE ${WORLD_SVR_DIR}/main.res)
    endif()
endif()

if(NOT WIN32)
    target_link_libraries(CoreLib PRIVATE m dl)
    target_link_libraries(NetLib PRIVATE m)
    target_link_libraries(RuntimeDataLib PRIVATE m)
    target_link_libraries(RuntimeLib PRIVATE m)

    if(CONFIG_BUILD_TARGET_LOGIN_SVR)
        target_link_libraries(LoginSvr PRIVATE m)
    endif()
endif()
