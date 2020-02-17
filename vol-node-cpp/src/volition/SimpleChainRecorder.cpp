// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
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
    
    const Chain* chain = miner.getBestBranch ();
    if ( !chain ) return;

    shared_ptr < Block > genesisBlock = chain->getBlock ( 0 );
    assert ( genesisBlock );
    
    this->mGenesisHash = genesisBlock->getHash ();
    
    this->mChainFolderPath = Format::write ( "%s/%s/", this->mBasePath.c_str (), this->mGenesisHash.c_str ());
    mkdir ( this->mChainFolderPath.c_str (), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    
    this->mBlocksFolderPath = Format::write ( "%s%s/", this->mChainFolderPath.c_str (), BLOCK_FOLDERNAME );
    mkdir ( this->mBlocksFolderPath.c_str (), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    
    this->mIndexFilePath = Format::write ( "%s%s", this->mChainFolderPath.c_str (), INDEX_FILENAME );
    
    struct stat buf;
    if ( stat ( this->mIndexFilePath.c_str (), &buf) != 0) {
        SerializableVector < string > hashes;
        hashes.push_back ( this->mGenesisHash );
        ToJSONSerializer::toJSONFile ( hashes, this->mIndexFilePath );
    }
}

//----------------------------------------------------------------//
SimpleChainRecorder::~SimpleChainRecorder () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void SimpleChainRecorder::AbstractChainRecorder_loadChain ( Miner& miner ) const {
    
    const Chain* chain = miner.getBestBranch ();
    if ( !chain ) return;
    assert ( chain->countBlocks () == 1 );
    
    SerializableVector < string > hashes;
    FromJSONSerializer::fromJSONFile ( hashes, this->mIndexFilePath );
    
    size_t length = hashes.size ();
    for ( size_t i = 0; i < length; ++i ) {
    
        string hash = hashes [ i ];
        
        if ( i == 0 ) {
            assert ( hash == this->mGenesisHash );
            continue;
        }
        
        string blockPath = Format::write ( "%s%s.json", this->mBlocksFolderPath.c_str (), hash.c_str ());
        
        Block block;
        FromJSONSerializer::fromJSONFile ( block, blockPath );
        
        Miner::SubmissionResponse response = miner.submitBlock ( block );
        string accountName = chain->getAccountName ( 2 );
        shared_ptr < Account > account = chain->getAccount ( accountName );
        assert ( response == Miner::SubmissionResponse::ACCEPTED );
    }
}

//----------------------------------------------------------------//
void SimpleChainRecorder::AbstractChainRecorder_saveChain ( const Miner& miner ) {

    const Chain* chain = miner.getBestBranch ();
    if ( !chain ) return;

    SerializableVector < string > prevHashes;
    FromJSONSerializer::fromJSONFile ( prevHashes, this->mIndexFilePath );

    SerializableVector < string > hashes;
    bool didChange = false;

    size_t length = chain->countBlocks ();
    for ( size_t i = 0; i < length; ++i ) {
        shared_ptr < Block > block = chain->getBlock ( i );
        assert ( block );
        
        string hash = block->getHash ();
        hashes.push_back ( hash );
        
        if ( i == 0 ) {
            assert ( hash == this->mGenesisHash );
            continue;
        }
        
        if (( i < prevHashes.size () ) && ( hash == prevHashes [ i ])) continue;
        
        didChange = true;
        
        string blockPath = Format::write ( "%s%s.json", this->mBlocksFolderPath.c_str (), hash.c_str ());
        ToJSONSerializer::toJSONFile ( *block, blockPath );
    }
    
    if ( didChange ) {
        ToJSONSerializer::toJSONFile ( hashes, this->mIndexFilePath );
    }
}

} // namespace Volition
