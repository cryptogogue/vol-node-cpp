// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

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
    
    this->mMinerID      = object.optValue < string >( "minerID", "" );
    this->mURL          = object.optValue < string >( "url", "" );
}

//----------------------------------------------------------------//
void RegisterMiner::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
    AbstractTransaction::AbstractSerializable_toJSON ( object );
    
    object.set ( "minerID",     this->mMinerID.c_str ());
    object.set ( "url",         this->mURL.c_str ());
}

//----------------------------------------------------------------//
void RegisterMiner::AbstractTransaction_apply ( State& state ) const {

    state.registerMiner ( this->mMinerID, this->mURL );
}

} // namespace Transaction
} // namespace Volition
