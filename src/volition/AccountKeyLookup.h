// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ACCOUNTKEYLOOKUP_H
#define VOLITION_ACCOUNTKEYLOOKUP_H

#include <volition/common.h>
#include <volition/AccountID.h>
#include <volition/CryptoKey.h>
#include <volition/Policy.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// AccountKeyLookup
//================================================================//
class AccountKeyLookup :
    public AbstractSerializable {
public:
    
    AccountID       mAccountIndex;
    string          mKeyName;

    //----------------------------------------------------------------//
    AccountKeyLookup () {
    }
    
    //----------------------------------------------------------------//
    AccountKeyLookup ( AccountID accountIndex, string keyName ) :
        mAccountIndex ( accountIndex ),
        mKeyName ( keyName ) {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "accountIndex",      this->mAccountIndex );
        serializer.serialize ( "keyName",           this->mKeyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "accountIndex",      this->mAccountIndex );
        serializer.serialize ( "keyName",           this->mKeyName );
    }
};

} // namespace Volition
#endif
