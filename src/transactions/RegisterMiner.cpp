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
RegisterMiner::~RegisterMiner () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void RegisterMiner::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {

    digestStream << this->mURL;
}

//----------------------------------------------------------------//
void RegisterMiner::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
    AbstractTransaction::AbstractSerializable_fromJSON ( object );
    
    string keyString        = object.optValue < string >( "publicKey", "" );
    
    stringstream keyStream ( keyString );
    
    this->mMinerID          = object.optValue < string >( "minerID", "" );
    this->mURL              = object.optValue < string >( "url", "" );
    this->mPublicKey        = keyString.size () ? make_unique < Poco::Crypto::ECKey >( &keyStream ) : NULL;
}

//----------------------------------------------------------------//
void RegisterMiner::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
    AbstractTransaction::AbstractSerializable_toJSON ( object );
    
    stringstream publicKeyString;
    this->mPublicKey->save ( &publicKeyString );
    
    object.set ( "minerID",         this->mMinerID.c_str ());
    object.set ( "url",             this->mURL.c_str ());
    object.set ( "publicKey",       publicKeyString.str ().c_str ());
}

//----------------------------------------------------------------//
void RegisterMiner::AbstractTransaction_apply ( State& state ) const {

    // TODO: make sure we've verified

    state.registerMiner ( MinerInfo ( this->mMinerID, this->mURL, *this->mPublicKey ));
}

} // namespace Transaction
} // namespace Volition
