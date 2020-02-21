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

    typedef SerializableMap < string, SerializableSharedPtr < AbstractEntitlement, EntitlementFactory >> Children;

    Children    mChildren;

    //----------------------------------------------------------------//
    void                                    affirmPath              ( string path, shared_ptr < AbstractEntitlement > leaf );
    static bool                             isDelimiter             ( char c );
    void                                    print                   ( size_t indent, size_t step = 2 ) const;
    const AbstractEntitlement*              resolvePath             ( string path ) const;

    //----------------------------------------------------------------//
    bool                                    AbstractEntitlement_check               () const override;
    bool                                    AbstractEntitlement_check               ( double value ) const override;
    shared_ptr < AbstractEntitlement >      AbstractEntitlement_clone               () const override;
    Leniency                                AbstractEntitlement_compare             ( const AbstractEntitlement* abstractOther ) const override;
    Range                                   AbstractEntitlement_getRange            () const override;
    void                                    AbstractEntitlement_print               () const override;
    void                                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
    shared_ptr < PathEntitlement >          apply                   ( const PathEntitlement& other ) const;
    bool                                    check                   ( string path ) const;
    const AbstractEntitlement*              getChild                ( string name ) const;
                                            PathEntitlement         ();
                                            PathEntitlement         ( const PathEntitlement& other );
                                            ~PathEntitlement        ();
    void                                    print                   () const;
    shared_ptr < PathEntitlement >          prune                   ( const PathEntitlement& other ) const;
    void                                    setPath                 ( string path );

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
    template < typename TYPE >
    const TYPE* resolvePathAs ( string path ) const {

        const AbstractEntitlement* leaf = this->resolvePath ( path );
        if ( leaf ) {
            return dynamic_cast < const TYPE* >( leaf );
        }
        return NULL;
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    void setPath ( string path, const TYPE& leaf ) {
    
        this->affirmPath ( path, make_shared < TYPE >( leaf ));
    }
};

} // namespace Volition
#endif
