// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/common.h>
#include <volition/BooleanEntitlement.h>
#include <volition/PathEntitlement.h>

namespace Volition {

//================================================================//
// PathEntitlement
//================================================================//
    
//----------------------------------------------------------------//
void PathEntitlement::affirmPath ( string path, shared_ptr < AbstractEntitlement > leaf ) {
    
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
shared_ptr < PathEntitlement > PathEntitlement::apply ( const PathEntitlement& other ) const {

    shared_ptr < PathEntitlement > result = make_shared < PathEntitlement >();

    Children::const_iterator childIt = this->mChildren.cbegin ();
    for ( ; childIt != this->mChildren.end (); ++childIt ) {
        
        const AbstractEntitlement* child = childIt->second.get ();
        const AbstractEntitlement* otherChild = other.getChild ( childIt->first );
        
        const PathEntitlement* pathChild = dynamic_cast < const PathEntitlement* >( child );
        const PathEntitlement* otherPathChild = dynamic_cast < const PathEntitlement* >( otherChild );
        
        if ( pathChild && otherPathChild ) {
            shared_ptr < AbstractEntitlement > mergedChild = pathChild->apply ( *otherPathChild );
            result->mChildren [ childIt->first ] = mergedChild;
        }
        else {
            result->mChildren [ childIt->first ] = child->clone ();
        }
    }
    
    Children::const_iterator otherChildIt = other.mChildren.cbegin ();
    for ( ; otherChildIt != other.mChildren.end (); ++otherChildIt ) {
     
        const AbstractEntitlement* otherChild = otherChildIt->second.get ();
        const AbstractEntitlement* child = result->getChild ( otherChildIt->first );
        if ( !child ) {
            result->mChildren [ otherChildIt->first ] = otherChild->clone ();
        }
    }
    return result;
}

//----------------------------------------------------------------//
bool PathEntitlement::check ( string path ) const {

   const AbstractEntitlement* leaf = this->resolvePath ( path );
   return leaf ? leaf->check () : false;
}

//----------------------------------------------------------------//
const AbstractEntitlement* PathEntitlement::getChild ( string name ) const {

    Children::const_iterator childIt = this->mChildren.find ( name );
    return ( childIt != this->mChildren.cend ()) ? childIt->second.get () : NULL;
}

//----------------------------------------------------------------//
bool PathEntitlement::isDelimiter ( char c ) {

    return ( c == '.' );
}

//----------------------------------------------------------------//
PathEntitlement::PathEntitlement () {
}

//----------------------------------------------------------------//
PathEntitlement::PathEntitlement ( const PathEntitlement& other ) {

    Children::const_iterator childIt = other.mChildren.cbegin ();
    for ( ; childIt != other.mChildren.cend (); ++childIt ) {
        this->mChildren [ childIt->first ] = childIt->second->clone ();
    }
}

//----------------------------------------------------------------//
PathEntitlement::~PathEntitlement () {
}

//----------------------------------------------------------------//
void PathEntitlement::print () const {
    AbstractEntitlement::print ();
}

//----------------------------------------------------------------//
void PathEntitlement::print ( size_t indent, size_t step ) const {
    
    Children::const_iterator childIt = this->mChildren.cbegin ();
    for ( ; childIt != this->mChildren.end (); ++childIt ) {
    
        for ( size_t i = 0; i < indent; ++i ) {
            printf ( " " );
        }
    
        printf ( "%s:", childIt->first.c_str ());
    
        AbstractEntitlement* child = childIt->second.get ();
        PathEntitlement* pathChild = dynamic_cast < PathEntitlement* >( child );
        
        if ( pathChild ) {
            printf ( "\n" );
            pathChild->print ( indent + step );
        }
        else {
            printf ( " " );
            child->print ();
            printf ( "\n" );
        }
    }
}

//----------------------------------------------------------------//
shared_ptr < PathEntitlement > PathEntitlement::prune ( const PathEntitlement& other ) const {

    shared_ptr < PathEntitlement > result = make_shared < PathEntitlement >();

    Children::const_iterator childIt = this->mChildren.cbegin ();
    for ( ; childIt != this->mChildren.end (); ++childIt ) {
    
        AbstractEntitlement* child = childIt->second.get ();
        PathEntitlement* pathChild = dynamic_cast < PathEntitlement* >( child );
        
        const AbstractEntitlement* otherChild = other.getChild ( childIt->first );
        const PathEntitlement* otherPathChild = dynamic_cast < const PathEntitlement* >( otherChild );
        
        shared_ptr < PathEntitlement > prunedChild;
        
        if ( pathChild && otherPathChild ) {
            prunedChild = pathChild->prune ( *otherPathChild );
            child = prunedChild.get ();
        }
        
        if ( AbstractEntitlement::compareRanges ( child, otherChild ) != IS_EQUALLY_RESTRICTIVE ) {
            result->mChildren [ childIt->first ] = child->clone ();
        }
    }
    return result;
}

//----------------------------------------------------------------//
const AbstractEntitlement* PathEntitlement::resolvePath ( string path ) const {
    
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
void PathEntitlement::setPath ( string path ) {

    this->setPath ( path, BooleanEntitlement ( true ));
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
bool PathEntitlement::AbstractEntitlement_check () const {

    return true; // path exists, so return true
}

//----------------------------------------------------------------//
bool PathEntitlement::AbstractEntitlement_check ( double value ) const {
    UNUSED ( value );

    return true; // always true
}

//----------------------------------------------------------------//
shared_ptr < AbstractEntitlement > PathEntitlement::AbstractEntitlement_clone () const {

    return make_shared < PathEntitlement >( *this );
}

//----------------------------------------------------------------//
AbstractEntitlement::Leniency PathEntitlement::AbstractEntitlement_compare ( const AbstractEntitlement* abstractOther ) const {

    const PathEntitlement* otherPath = dynamic_cast < const PathEntitlement* >( abstractOther );
    
    if ( otherPath ) {
        size_t equalCount = 0;
    
        Children::const_iterator childIt = this->mChildren.cbegin ();
        for ( ; childIt != this->mChildren.end (); ++childIt ) {
        
            const AbstractEntitlement* child = childIt->second.get ();
            const AbstractEntitlement* otherChild = otherPath->getChild ( childIt->first );
            
            Leniency compare = child->compare ( otherChild );
            
            if ( compare == IS_LESS_RESTRICTIVE ) return IS_LESS_RESTRICTIVE;
            
            if ( compare == IS_EQUALLY_RESTRICTIVE ) {
                equalCount++;
            }
        }
        
        size_t nChildren = this->mChildren.size ();
        if (( equalCount == nChildren ) && ( nChildren == otherPath->mChildren.size ())) return IS_EQUALLY_RESTRICTIVE;
        return IS_MORE_RESTRICTIVE;
    }
    return AbstractEntitlement::compareRanges ( this, abstractOther );
}

//----------------------------------------------------------------//
AbstractEntitlement::Range PathEntitlement::AbstractEntitlement_getRange () const {

    return ALWAYS_TRUE;
}

//----------------------------------------------------------------//
void PathEntitlement::AbstractEntitlement_print () const {
    this->print ( 0 );
}

//----------------------------------------------------------------//
void PathEntitlement::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    
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
void PathEntitlement::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    
    serializer.serialize ( "type",          ENTITLEMENT_TYPE_PATH );
    serializer.serialize ( "children",      this->mChildren );
}

} // namespace Volition
