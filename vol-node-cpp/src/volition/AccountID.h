// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ACCOUNTID_H
#define VOLITION_ACCOUNTID_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// AccountID
//================================================================//
class AccountID :
    public AbstractSerializable {
public:

    typedef u64 Index;
    enum {
        NULL_INDEX      = ( u64 )-1,
    };

    Index   mIndex;
    
    //----------------------------------------------------------------//
    operator Index () const {
        return this->mIndex;
    }
    
    //----------------------------------------------------------------//
    AccountID& operator = ( Index index ) {
        this->mIndex = index;
        return *this;
    }
    
    //----------------------------------------------------------------//
    AccountID& operator = ( const AccountID& accountID ) {
        this->mIndex = accountID.mIndex;
        return *this;
    }
    
    //----------------------------------------------------------------//
    AccountID () :
        mIndex ( NULL_INDEX ) {
    }
    
    //----------------------------------------------------------------//
    AccountID ( Index index ) :
        mIndex ( index ) {
    }
    
    //----------------------------------------------------------------//
    AccountID ( const AccountID& accountID ) :
        mIndex ( accountID.mIndex ) {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        this->mIndex = NULL_INDEX;
        
        const AbstractSerializerFrom* parent = serializer.getParent ();
        if ( parent ) {
            parent->serialize ( serializer.getName (), this->mIndex );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        AbstractSerializerTo* parent = serializer.getParent ();
        if ( parent ) {
            parent->serialize ( serializer.getName (), this->mIndex );
        }
    }
};

} // namespace Volition
#endif
