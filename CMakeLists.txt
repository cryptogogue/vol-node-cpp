cmake_minimum_required ( VERSION 3.10.2 )
project ( volition )

set ( THREADS_PREFER_PTHREAD_FLAG ON )
find_package ( Threads REQUIRED )

set ( CMAKE_CXX_STANDARD 17 )
#set ( CMAKE_CXX_STANDARD_REQUIRED ON )
#set ( CMAKE_BUILD_TYPE "Debug" )

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wall -Wextra" )
set ( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic" )

# TODO: doesn't work in docker container
#add_custom_target (
#    make-version-header
#    COMMAND ops/make-version-header.sh
#)

get_filename_component ( VOLITION_ROOT "./" ABSOLUTE )

add_subdirectory ( 3rd-party/padamose EXCLUDE_FROM_ALL )
add_subdirectory ( cmake/lua EXCLUDE_FROM_ALL )
add_subdirectory ( cmake/simple-cpp-router EXCLUDE_FROM_ALL )

set ( VOLITION_INCLUDES
    src/.config/
    src/
    3rd-party/lua-5.3.5/src/
    3rd-party/padamose/src/
    3rd-party/padamose/3rd-party/lognosis/src
    3rd-party/padamose/3rd-party/lognosis/3rd-party
#    3rd-party/padamose/3rd-party/reproc/reproc/include/
#    3rd-party/padamose/3rd-party/reproc/reproc++/include/
    3rd-party/padamose/3rd-party/sqlite-3.34.0/
    3rd-party/padamose/3rd-party/rocksdb/include/
    3rd-party/simple-cpp-router/src/
    /usr/local/opt/openssl/include/
)

add_library ( volition-lib "" )

#add_dependencies ( volition-lib make-version-header )

target_include_directories ( volition-lib PRIVATE ${VOLITION_INCLUDES} )

target_sources ( volition-lib
    PRIVATE
        src/volition/AbstractAPIRequestHandler.cpp
        src/volition/AbstractBlockTree.cpp
        src/volition/AbstractControlCommandBody.cpp
        src/volition/AbstractCryptoKey.cpp
        src/volition/AbstractEntitlement.cpp
        src/volition/AbstractRequestHandler.cpp
        src/volition/AbstractTransactionBody.cpp
        src/volition/AssetID.cpp
        src/volition/AssetMethod.cpp
        src/volition/Block.cpp
        src/volition/BlockHeader.cpp
        src/volition/BlockHeaderFields.cpp
        src/volition/BlockSearchPool.cpp
        src/volition/BlockTreeCursor.cpp
        src/volition/BlockTreeSampler.cpp
        src/volition/BlockTreeTag.cpp
        src/volition/ControlCommand.cpp
        src/volition/CryptoKeyInfo.cpp
        src/volition/CryptoKeyPair.cpp
        src/volition/CryptoPublicKey.cpp
        src/volition/EntitlementFactory.cpp
        src/volition/FileSys.cpp
        src/volition/Format.cpp
        src/volition/HasBlockHeaderFields.cpp
        src/volition/HTTPMiningMessenger.cpp
        src/volition/InMemoryBlockTree.cpp
        src/volition/InMemoryBlockTreeNode.cpp
        src/volition/InventoryLogEntry.cpp
        src/volition/Ledger_Account.cpp
        src/volition/Ledger_Dump.cpp
        src/volition/Ledger_Inventory.cpp
        src/volition/Ledger_Miner.cpp
        src/volition/Ledger.cpp
        src/volition/LuaContext.cpp
        src/volition/Miner.cpp
        src/volition/MinerActivity.cpp
        src/volition/MinerAPIFactory.cpp
        src/volition/MinerInfo.cpp
        src/volition/MinerLaunchTests.cpp
        src/volition/Munge.cpp
        src/volition/PathEntitlement.cpp
        src/volition/RemoteMiner.cpp
        src/volition/Schema.cpp
        src/volition/Signature.cpp
        src/volition/SQLiteBlockTree.cpp
        src/volition/SquapFactory.cpp
        src/volition/TheControlCommandBodyFactory.cpp
        src/volition/TheTransactionBodyFactory.cpp
        src/volition/Transaction.cpp
        src/volition/TransactionContext.cpp
        src/volition/TransactionMaker.cpp
        src/volition/TransactionQueue.cpp
        src/volition/TransactionResult.cpp
        src/volition/simulation/Analysis.cpp
        src/volition/simulation/SimMiner.cpp
        src/volition/simulation/SimMiningNetwork.cpp
        src/volition/simulation/Simulator.cpp
        src/volition/simulation/SimulatorActivity.cpp
        src/volition/transactions/LoadLedger.cpp
)

#install ( TARGETS volition-lib EXPORT libvolition ARCHIVE DESTINATION lib)
#install ( DIRECTORY src/volition/ DESTINATION include/volition FILES_MATCHING PATTERN "*.h" )

add_executable ( volition "" )

add_dependencies ( volition
    hiredis
    lognosis
    lua
    padamose
#    reproc
#    reproc++
    routing
    sqlite
    volition-lib
)

target_include_directories ( volition PRIVATE ${VOLITION_INCLUDES} )

target_sources ( volition
    PRIVATE
        src/volition/main-server.cpp
)

find_package ( OpenSSL REQUIRED )

target_link_libraries ( volition
    volition-lib
    lua
    padamose
    hiredis
    lognosis
    Threads::Threads
#    reproc
#    reproc++
    routing
    sqlite
    rocksdb
    PocoCrypto
    PocoEncodings
    PocoNet
    PocoNetSSL
    PocoUtil
    PocoJSON
    PocoXML
    PocoZip
    PocoFoundation
    ${OPENSSL_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

#install ( TARGETS hiredis )
install ( TARGETS volition DESTINATION bin )
