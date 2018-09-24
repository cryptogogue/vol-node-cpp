// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTVALUESTACK_H
#define VOLITION_ABSTRACTVALUESTACK_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// AbstractValueStack
//================================================================//
class AbstractValueStack {
protected:

    friend class AbstractVersionedValueIterator;
    friend class VersionedStoreSnapshot;
    friend class VersionedStoreBranch;
    
    template < typename TYPE > friend class ValueStack;
    template < typename TYPE > friend class VersionedValueIterator;

    size_t              mTypeID;

    //----------------------------------------------------------------//
    void                                            copyFrom                                    ( const AbstractValueStack& from );
    void                                            erase                                       ( size_t version );
    const void*                                     getRaw                                      ( size_t version ) const;
    unique_ptr < AbstractValueStack >               makeEmptyCopy                               () const;
    void                                            setRaw                                      ( size_t version, const void* value );
    size_t                                          size                                        () const;

    //----------------------------------------------------------------//
    virtual void                                    AbstractValueStack_copyFrom                 ( const AbstractValueStack& from ) = 0;
    virtual void                                    AbstractValueStack_erase                    ( size_t version ) = 0;
    virtual const void*                             AbstractValueStack_getRaw                   ( size_t version ) const = 0;
    virtual unique_ptr < AbstractValueStack >       AbstractValueStack_makeEmptyCopy            () const = 0;
    virtual void                                    AbstractValueStack_setRaw                   ( size_t version, const void* value ) = 0;
    virtual size_t                                  AbstractValueStack_size                     () const = 0;

public:

    //----------------------------------------------------------------//
                        AbstractValueStack                  ();
    virtual             ~AbstractValueStack                 ();
};

} // namespace Volition
#endif
