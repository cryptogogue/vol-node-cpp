// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CONTRACTWITHDIGEST_H
#define VOLITION_CONTRACTWITHDIGEST_H

#include <volition/common.h>
#include <volition/IndexID.h>
#include <volition/KeyAndPolicy.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// ContractWithDigest
//================================================================//
class ContractWithDigest :
    public AbstractSerializable {
public:
    
    string              mText;
    Digest              mDigest;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "text",              this->mText );
        serializer.serialize ( "digest",            this->mDigest );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "text",              this->mText );
        serializer.serialize ( "digest",            this->mDigest );
    }

    //----------------------------------------------------------------//
    ContractWithDigest () {
    }

    //----------------------------------------------------------------//
    ContractWithDigest ( string text ) :
        mText ( text ),
        mDigest ( text ) {
    }
};

} // namespace Volition
#endif
