// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_IDENTITYPROVIDER_H
#define VOLITION_IDENTITYPROVIDER_H

#include <volition/common.h>
#include <volition/IndexID.h>
#include <volition/KeyAndPolicy.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// IdentityProvider
//================================================================//
class IdentityProvider :
    public AbstractSerializable {
public:
    
    string          mProviderKeyName;
    string          mEd25519PublicHex;
    Policy          mKeyPolicy;
    Policy          mAccountPolicy;
    u64             mGrant;
    
    //----------------------------------------------------------------//
    operator bool () {
        
        return ( this->mEd25519PublicHex.size () > 0 );
    }
    
    //----------------------------------------------------------------//
    IdentityProvider () :
        mGrant ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "providerKeyName",   this->mProviderKeyName );
        serializer.serialize ( "ed25519PublicHex",  this->mEd25519PublicHex );
        serializer.serialize ( "keyPolicy",         this->mKeyPolicy );
        serializer.serialize ( "accountPolicy",     this->mAccountPolicy );
        serializer.serialize ( "grant",             this->mGrant );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "providerKeyName",   this->mProviderKeyName );
        serializer.serialize ( "ed25519PublicHex",  this->mEd25519PublicHex );
        serializer.serialize ( "keyPolicy",         this->mKeyPolicy );
        serializer.serialize ( "accountPolicy",     this->mAccountPolicy );
        serializer.serialize ( "grant",             this->mGrant );
    }
};

} // namespace Volition
#endif
