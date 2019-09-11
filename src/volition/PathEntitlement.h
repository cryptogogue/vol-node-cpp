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
    
    //----------------------------------------------------------------//
    const AbstractEntitlement* resolvePath ( string path ) const {
        
        size_t length = path.size ();
        if ( length ) {
        
            size_t delim = 0;
            for ( ; ( delim < length ) && !PathEntitlement::isDelimiter ( path [ delim ]); ++delim ) {}
            
            if ( delim > 0 ) {
            
                string name = path.substr ( 0, delim );
                bool isBranch = ( delim < length );
                const AbstractEntitlement* child = this->getChild ( name );
            
                if ( isBranch ) {
                    const PathEntitlement* branch = dynamic_cast < const PathEntitlement* >( child );
                    return branch ? branch->resolvePath ( path.substr ( delim + 1, string::npos )) : NULL;
                }
                else {
                    return child;
                }
            }
        }
        return this;
    }

    //----------------------------------------------------------------//
    static bool isDelimiter ( char c ) {
    
        return ( c == '.' );
    }

public:

    //----------------------------------------------------------------//
    void affirmPath ( string path, shared_ptr < AbstractEntitlement > leaf = NULL ) {
        
        size_t length = path.size ();
        if ( length ) {
        
            size_t delim = 0;
            for ( ; ( delim < length ) && !PathEntitlement::isDelimiter ( path [ delim ]); ++delim ) {}
            
            if ( delim > 0 ) {
            
                string name = path.substr ( 0, delim );
                bool isBranch = ( delim < length );
                
                if ( isBranch || ( leaf == NULL )) {
                    
                    Children::const_iterator childIt = this->mChildren.find ( name );
                    PathEntitlement* child = ( childIt != this->mChildren.cend ()) ? dynamic_cast < PathEntitlement* >( childIt->second.get ()) : NULL;
                    
                    if ( !child ) {
                        shared_ptr < PathEntitlement > newChild = make_shared < PathEntitlement >();
                        child = newChild.get ();
                        this->mChildren [ name ] = SerializableSharedPtr < AbstractEntitlement, EntitlementFactory >( newChild );
                    }
                    
                    if ( isBranch ) {
                        child->affirmPath ( path.substr ( delim + 1, string::npos ), leaf );
                    }
                }
                else {
                
                    this->mChildren [ name ] = leaf;
                }
            }
        }
    }

    //----------------------------------------------------------------//
    bool check ( string path ) const {

       const AbstractEntitlement* leaf = this->resolvePath ( path );
       return leaf ? leaf->check () : false;
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    bool check ( string path, TYPE value ) const {
    
        const AbstractEntitlement* leaf = this->resolvePath ( path );
        if ( leaf ) {
            return leaf->check ( value );
        }
        return false;
    }

    //----------------------------------------------------------------//
    const AbstractEntitlement* getChild ( string name ) const {
    
        Children::const_iterator childIt = this->mChildren.find ( name );
        return ( childIt != this->mChildren.cend ()) ? childIt->second.get () : NULL;
    }

    //----------------------------------------------------------------//
    PathEntitlement () {
    }
    
    //----------------------------------------------------------------//
    ~PathEntitlement () {
    }
};

} // namespace Volition
#endif
