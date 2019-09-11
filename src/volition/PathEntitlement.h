// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_PATHENTITLEMENT_H
#define VOLITION_PATHENTITLEMENT_H

#include <volition/common.h>
#include <volition/AbstractEntitlement.h>
#include <volition/EntitlementFactory.h>

namespace Volition {

//================================================================//
// PathEntitlement
//================================================================//
class PathEntitlement :
    public AbstractEntitlement {
protected:

    friend class Entitlements;

    typedef SerializableMap < string, SerializableSharedPtr < AbstractEntitlement, EntitlementFactory >> Children;

    Children    mChildren;

    //----------------------------------------------------------------//
    bool AbstractEntitlement_check () const override {
    
        return true; // path exists, so return true
    }

    //----------------------------------------------------------------//
    bool AbstractEntitlement_isMatchOrSubsetOf ( const AbstractEntitlement* abstractOther ) const override {
    
        const PathEntitlement* other = dynamic_cast < const PathEntitlement* >( abstractOther );
        if ( !other ) return false;
        
        Children::const_iterator childIt = this->mChildren.cbegin ();
        for ( ; childIt != this->mChildren.end (); ++childIt ) {
            if ( childIt->second ) {
                const AbstractEntitlement* otherChild = other->getChild ( childIt->first );
                if ( !otherChild ) return false; // other *must* contain a matching child, or is *not* a superset.
                if ( !childIt->second->isMatchOrSubsetOf ( otherChild )) return false; // make sure the matchign child is a superset.
            }
        }
        return true;
    }

    //----------------------------------------------------------------//
    const AbstractEntitlement* AbstractEntitlement_getChild ( string name ) const override {
    
        Children::const_iterator childIt = this->mChildren.find ( name );
        return ( childIt != this->mChildren.cend ()) ? childIt->second.get () : NULL;
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        // since wer'e using a factory, may wind up with NULL children.
        Children children;
        serializer.serialize ( "children", children );
        
        // filter out NULL children.
        Children::const_iterator childIt = children.cbegin ();
        for ( ; childIt != children.end (); ++childIt ) {
            if ( childIt->second ) {
                this->mChildren [ childIt->first ] = childIt->second;
            }
        }
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "type",          ENTITLEMENT_TYPE_PATH );
        serializer.serialize ( "children",      this->mChildren );
    }

public:

    //----------------------------------------------------------------//
    PathEntitlement () {
    }
    
    //----------------------------------------------------------------//
    ~PathEntitlement () {
    }
};

} // namespace Volition
#endif
