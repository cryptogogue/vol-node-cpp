// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "MinerInfo.h"
#include "RegisterMiner.h"

namespace Volition {
namespace Transaction {

//================================================================//
// RegisterMiner
//================================================================//

//----------------------------------------------------------------//
RegisterMiner::RegisterMiner () {
}

//----------------------------------------------------------------//
RegisterMiner::RegisterMiner ( string minerID, string url, string keyName ) :
    mMinerID ( minerID ),
    mURL ( url ),
    mKeyName ( keyName ) {
}

//----------------------------------------------------------------//
RegisterMiner::~RegisterMiner () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void RegisterMiner::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {

    digestStream << this->mMinerID;
    digestStream << this->mURL;
    digestStream << this->mKeyName;
}

//----------------------------------------------------------------//
void RegisterMiner::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
    AbstractTransaction::AbstractSerializable_fromJSON ( object );
    
    this->mMinerID                  = object.optValue < string >( "minerID", "" );
    this->mURL                      = object.optValue < string >( "url", "" );
    this->mKeyName                  = object.optValue < string >( "keyName", "" );
}

//----------------------------------------------------------------//
void RegisterMiner::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
    AbstractTransaction::AbstractSerializable_toJSON ( object );
    
    object.set ( "minerID",         this->mMinerID.c_str ());
    object.set ( "url",             this->mURL.c_str ());
    object.set ( "keyName",         this->mKeyName.c_str ());
}

//----------------------------------------------------------------//
void RegisterMiner::AbstractTransaction_apply ( State& state ) const {

    // TODO: make sure we've verified

    //state.registerMiner ( MinerInfo ( this->mMinerID, this->mURL, *this->mPublicKey ));
}

} // namespace Transaction
} // namespace Volition
