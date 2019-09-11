// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BOOLEANENTITLEMENT_H
#define VOLITION_BOOLEANENTITLEMENT_H

#include <volition/common.h>
#include <volition/AbstractEntitlement.h>

namespace Volition {

//================================================================//
// BooleanEntitlement
//================================================================//
class BooleanEntitlement :
    public AbstractEntitlement {
protected:

    bool    mValue;

    //----------------------------------------------------------------//
    bool AbstractEntitlement_check () const override {
    
        return this->mValue;
    }

    //----------------------------------------------------------------//
    bool AbstractEntitlement_isMatchOrSubsetOf ( const AbstractEntitlement* abstractOther ) const override {
        
        return (( abstractOther && abstractOther->check ()) || ( this->mValue == false ));
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "value",     this->mValue );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "type",      ENTITLEMENT_TYPE_BOOLEAN );
        serializer.serialize ( "value",     this->mValue );
    }

public:

    //----------------------------------------------------------------//
    BooleanEntitlement () :
        mValue ( false ) {
    }
    
    //----------------------------------------------------------------//
    BooleanEntitlement ( bool value ) :
        mValue ( value ) {
    }
    
    //----------------------------------------------------------------//
    ~BooleanEntitlement () {
    }
};

} // namespace Volition
#endif
