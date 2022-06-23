// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Ledger.h>
#include <volition/Policy.h>

namespace Volition {

//================================================================//
// Policy
//================================================================//

//----------------------------------------------------------------//
shared_ptr < Entitlements > Policy::applyRestrictions ( const Entitlements& entitlements ) const {

    return this->mRestrictions ? this->mRestrictions->apply ( entitlements ) : make_shared < Entitlements >( entitlements );
}

//----------------------------------------------------------------//
string Policy::getBase () const {

    return this->mBase;
}

//----------------------------------------------------------------//
const Entitlements* Policy::getRestrictions () const {

    return this->mRestrictions.get ();
}

//----------------------------------------------------------------//
bool Policy::isMatchOrSubsetOf ( const Entitlements& entitlements ) const {

    return this->mRestrictions ? this->mRestrictions->isMatchOrSubsetOf ( &entitlements ) : true;
}

//----------------------------------------------------------------//
bool Policy::isValid ( const AbstractLedger& ledger ) const {

    return ( this->mBase.length () > 0 ) ? ledger.hasEntitlements ( this->mBase ) : true;
}

//----------------------------------------------------------------//
Policy::Policy () {
}

//----------------------------------------------------------------//
Policy::~Policy () {
}

//----------------------------------------------------------------//
void Policy::setRestrictions ( shared_ptr < const Entitlements > restrictions ) {

    this->mRestrictions = restrictions;
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void Policy::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    
    serializer.serialize ( "base",          this->mBase );
    serializer.serialize ( "restrictions",  this->mRestrictions );
}

//----------------------------------------------------------------//
void Policy::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    
    serializer.serialize ( "base",          this->mBase );
    serializer.serialize ( "restrictions",  this->mRestrictions );
}

} // namespace Volition
