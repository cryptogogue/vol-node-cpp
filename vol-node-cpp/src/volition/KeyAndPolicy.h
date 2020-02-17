// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_KEYANDPOLICY_H
#define VOLITION_KEYANDPOLICY_H

#include <volition/common.h>
#include <volition/CryptoKey.h>
#include <volition/Policy.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// KeyAndPolicy
//================================================================//
class KeyAndPolicy :
    public AbstractSerializable {
public:

    friend class Ledger;
    friend class Account;
    
    CryptoKey                           mKey;
    Policy                              mPolicy;
    SerializableSharedPtr < Policy >    mBequest;

    //----------------------------------------------------------------//
    operator const Policy& () const {
        return this->mPolicy;
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "key",               this->mKey );
        serializer.serialize ( "policy",            this->mPolicy );
        serializer.serialize ( "bequest",           this->mBequest );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "key",               this->mKey );
        serializer.serialize ( "policy",            this->mPolicy );
        serializer.serialize ( "bequest",           this->mBequest );
    }

    //----------------------------------------------------------------//
    const Policy* getBequest () const {
        return this->mBequest.get ();
    }

    //----------------------------------------------------------------//
    const Policy& getPolicy () {
        return this->mPolicy;
    }

    //----------------------------------------------------------------//
    KeyAndPolicy () {
    }
    
    //----------------------------------------------------------------//
    KeyAndPolicy ( const CryptoKey& key, const Policy& policy ) :
        mKey ( key ),
        mPolicy ( policy ) {
    }
};

} // namespace Volition
#endif
