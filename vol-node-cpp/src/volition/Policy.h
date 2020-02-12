// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_POLICY_H
#define VOLITION_POLICY_H

#include <volition/common.h>
#include <volition/Entitlements.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// Policy
//================================================================//
class Policy :
    public AbstractSerializable {
private:

    string                                      mBase;
    SerializableSharedPtr < Entitlements >      mRestrictions;

public:

    //----------------------------------------------------------------//
    shared_ptr < Entitlements > applyRestrictions ( const Entitlements& entitlements ) const {
    
        return this->mRestrictions ? this->mRestrictions->apply ( entitlements ) : make_shared < Entitlements >( entitlements );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "base",          this->mBase );
        serializer.serialize ( "restrictions",  this->mRestrictions );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "base",          this->mBase );
        serializer.serialize ( "restrictions",  this->mRestrictions );
    }
    
    //----------------------------------------------------------------//
    string getBase () const {
    
        return this->mBase;
    }
    
    //----------------------------------------------------------------//
    const Entitlements* getRestrictions () const {
    
        return this->mRestrictions.get ();
    }
    
    //----------------------------------------------------------------//
    bool isMatchOrSubsetOf ( const Entitlements& entitlements ) const {
    
        return this->mRestrictions ? this->mRestrictions->isMatchOrSubsetOf ( &entitlements ) : true;
    }
    
    //----------------------------------------------------------------//
    Policy () {
    }
    
    //----------------------------------------------------------------//
    ~Policy () {
    }
};

} // namespace Volition
#endif
