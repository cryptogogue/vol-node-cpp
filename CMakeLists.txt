cmake_minimum_required ( VERSION 3.10.2 )
project ( volition )

set ( CMAKE_CXX_STANDARD 17 )

get_filename_component ( VOLITION_ROOT "./" ABSOLUTE )

add_subdirectory ( 3rd-party/padamose EXCLUDE_FROM_ALL )
add_subdirectory ( cmake/lua EXCLUDE_FROM_ALL )
add_subdirectory ( cmake/simple-cpp-router EXCLUDE_FROM_ALL )

set ( VOLITION_INCLUDES 
    src/
    3rd-party/lua-5.3.5/src/
    3rd-party/padamose/src/
    3rd-party/padamose/3rd-party/lognosis/src
    3rd-party/padamose/3rd-party/lognosis/3rd-party
    3rd-party/padamose/3rd-party/reproc/reproc/include/
    3rd-party/padamose/3rd-party/reproc/reproc++/include/
    3rd-party/simple-cpp-router/src/
    /usr/local/opt/openssl/include/
)

add_library ( volition-lib "" )

target_include_directories ( volition-lib PRIVATE ${VOLITION_INCLUDES} )

target_sources ( volition-lib
    PRIVATE
        src/volition/AbstractAPIRequestHandler.cpp
        src/volition/AbstractRequestHandler.cpp
        src/volition/AbstractTransaction.cpp
        src/volition/Block.cpp
        src/volition/Chain.cpp
        src/volition/ChainMetadata.cpp
        src/volition/CryptoKey.cpp
        src/volition/Format.cpp
        src/volition/Ledger.cpp
        src/volition/Miner.cpp
        src/volition/MinerInfo.cpp
        src/volition/Policy.cpp
        src/volition/RouteTable.cpp
        src/volition/Schema.cpp
        src/volition/SchemaLua.cpp
        src/volition/Signature.cpp
        src/volition/SquapFactory.cpp
        src/volition/SyncChainTask.cpp
        src/volition/TheContext.cpp
        src/volition/TheTransactionFactory.cpp
        src/volition/TheWebMiner.cpp
        src/volition/TransactionMakerSignature.cpp
        src/volition/WebMiner.cpp
        src/volition/simulation/Analysis.cpp
        src/volition/simulation/Cohort.cpp
        src/volition/simulation/SimMiner.cpp
        src/volition/simulation/Simulation.cpp
        src/volition/web-miner-api/HTTPRequestHandlerFactory.cpp
)

install ( TARGETS volition-lib EXPORT libvolition ARCHIVE DESTINATION lib)
install ( DIRECTORY src/volition/ DESTINATION include/volition FILES_MATCHING PATTERN "*.h" )

add_executable ( volition "" )

add_dependencies ( volition
    hiredis
    lognosis
    lua
    padamose
    reproc
    reproc++
    routing
    volition-lib
)

target_include_directories ( volition PRIVATE ${VOLITION_INCLUDES} )

target_sources ( volition
    PRIVATE
        src/volition/main-server.cpp
)

#include ( cmake/cmake-modules/FindOpenSSL.cmake )
find_package ( OpenSSL REQUIRED )

target_link_libraries ( volition
    ${OPENSSL_LIBRARIES}
    hiredis
    lua
    lognosis
    reproc
    reproc++
    routing
    padamose
    PocoCrypto
    PocoEncodings
    PocoFoundation
    PocoJSON
    PocoNet
    PocoNetSSL
    PocoRedis
    PocoUtil
    PocoXML
    PocoZip
    volition-lib
)

#install ( TARGETS volition DESTINATION bin )
