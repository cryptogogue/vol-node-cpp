// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/FileSys.h>
#include <volition/Miner.h>
#include <volition/SimpleChainRecorder.h>

namespace Volition {

//================================================================//
// Miner
//================================================================//

//----------------------------------------------------------------//
SimpleChainRecorder::SimpleChainRecorder ( const Miner& miner, string path ) {
    
    Poco::Path basePath ( path );
    basePath.makeAbsolute ();
    
    this->mBasePath = basePath.toString ();
    
    mkdir ( this->mBasePath.c_str (), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    
    const Ledger& ledger = miner.getLedger ();

    shared_ptr < const Block > genesisBlock = ledger.getBlock ( 0 );
    assert ( genesisBlock );
    
    string hash = genesisBlock->getDigest ();
    this->mGenesisHash = hash;
    
    this->mChainFolderPath = Format::write ( "%s/%s/", this->mBasePath.c_str (), this->mGenesisHash.c_str ());
    mkdir ( this->mChainFolderPath.c_str (), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
}

//----------------------------------------------------------------//
SimpleChainRecorder::~SimpleChainRecorder () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void SimpleChainRecorder::AbstractChainRecorder_loadChain ( Miner& miner ) {
    
    const Ledger& ledger = miner.getLedger ();
    assert ( ledger.countBlocks () == 1 );
    
    for ( size_t i = 1; true; ++i ) {
    
        string blockPath = Format::write ( "%sblock_%d.json", this->mChainFolderPath.c_str (), i );
        if ( !FileSys::exists ( blockPath )) break;
        
         printf ( "LOADING: %s\n", blockPath.c_str ());
        
        shared_ptr < Block > block = make_shared < Block >();
        FromJSONSerializer::fromJSONFile ( *block, blockPath );
        AbstractChainRecorder::pushBlock ( miner, block );
    }
}

//----------------------------------------------------------------//
void SimpleChainRecorder::AbstractChainRecorder_loadConfig ( MinerConfig& minerConfig ) {

    string configPath = Format::write ( "%sconfig.json", this->mChainFolderPath.c_str ());
    if ( FileSys::exists ( configPath )) {
        FromJSONSerializer::fromJSONFile ( minerConfig, configPath );
    }
}

//----------------------------------------------------------------//
void SimpleChainRecorder::AbstractChainRecorder_reset () {

    size_t dirSize = FileSys::getDirSize ( this->mChainFolderPath );
    for ( size_t i = 1; i < dirSize; ++i ) {
    
        string blockPath = Format::write ( "%sblock_%d.json", this->mChainFolderPath.c_str (), i );
        if ( FileSys::exists ( blockPath )) {
            remove ( blockPath.c_str ());
        }
    }
}

//----------------------------------------------------------------//
void SimpleChainRecorder::AbstractChainRecorder_saveChain ( const Miner& miner ) {

    const Ledger& ledger = miner.getLedger ();

    size_t length = ledger.countBlocks ();
    for ( size_t i = 0; i < length; ++i ) {
    
        shared_ptr < const Block > block = ledger.getBlock ( i );
        assert ( block );
                
        string blockPath = Format::write ( "%sblock_%d.json", this->mChainFolderPath.c_str (), ( int )i );
        if ( FileSys::exists ( blockPath )) continue;
        ToJSONSerializer::toJSONFile ( *block, blockPath );
    }
}

//----------------------------------------------------------------//
void SimpleChainRecorder::AbstractChainRecorder_saveConfig ( const MinerConfig& minerConfig ) {

    string configPath = Format::write ( "%sconfig.json", this->mChainFolderPath.c_str ());
    ToJSONSerializer::toJSONFile ( minerConfig, configPath );
}

} // namespace Volition
