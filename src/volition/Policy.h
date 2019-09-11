// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_POLICY_H
#define VOLITION_POLICY_H

#include <volition/common.h>
#include <volition/Entitlements.h>

namespace Volition {

//================================================================//
// AbstractPolicy
//================================================================//
class AbstractPolicy :
    public AbstractSerializable {
public:

    enum Type {
        ACCOUNT_POLICY,
        KEY_POLICY,
    };

private:

    string              mBase;
    Entitlements        mRestrictions;

    //----------------------------------------------------------------//
    virtual Type        AbstractPolicy_getType      () const = 0;

public:

    //----------------------------------------------------------------//
    void applyRestrictions ( Entitlements& entitlements ) const {
    
        this->mRestrictions.apply ( entitlements );
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
    AbstractPolicy () {
    }
    
    //----------------------------------------------------------------//
    ~AbstractPolicy () {
    }
    
    //----------------------------------------------------------------//
    string getBase () const {
    
        return this->mBase;
    }
    
    //----------------------------------------------------------------//
    const Entitlements& getRestrictions () const {
    
        return this->mRestrictions;
    }
    
    //----------------------------------------------------------------//
    Type getType () const {
    
        return this->AbstractPolicy_getType ();
    }
};

//================================================================//
// PolicyWithType
//================================================================//
template < AbstractPolicy::Type TYPE >
class PolicyWithType :
    public AbstractPolicy {
protected:

    //----------------------------------------------------------------//
    Type AbstractPolicy_getType () const override {
        return TYPE;
    }

public:
};

typedef PolicyWithType < AbstractPolicy::ACCOUNT_POLICY > AccountPolicy;
typedef PolicyWithType < AbstractPolicy::KEY_POLICY > KeyPolicy;

} // namespace Volition
#endif
