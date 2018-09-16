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
    friend class VersionedStore;
    friend class VersionedStoreEpoch;
    template < typename TYPE > friend class ValueStack;
    template < typename TYPE > friend class VersionedValueIterator;

    size_t              mTypeID;

    //----------------------------------------------------------------//
    const void*                                     getRaw                                      ( size_t version ) const;
    const void*                                     getRawForIndex                              ( size_t index ) const;
    size_t                                          getVersionForIndex                          ( size_t index ) const;
    bool                                            isEmpty                                     () const;
    unique_ptr < AbstractValueStack >               makeEmptyCopy                               () const;
    void                                            pop                                         ();
    void                                            pushBackRaw                                 ( const void* value, size_t version );
    size_t                                          size                                        () const;

    //----------------------------------------------------------------//
    virtual const void*                             AbstractValueStack_getRaw                   ( size_t version ) const = 0;
    virtual const void*                             AbstractValueStack_getRawForIndex           ( size_t index ) const = 0;
    virtual size_t                                  AbstractValueStack_getVersionForIndex       ( size_t index ) const = 0;
    virtual bool                                    AbstractValueStack_isEmpty                  () const = 0;
    virtual unique_ptr < AbstractValueStack >       AbstractValueStack_makeEmptyCopy            () const = 0;
    virtual void                                    AbstractValueStack_pop                      () = 0;
    virtual void                                    AbstractValueStack_pushBackRaw              ( const void* value, size_t version ) = 0;
    virtual size_t                                  AbstractValueStack_size                     () const = 0;

public:

    //----------------------------------------------------------------//
                        AbstractValueStack                  ();
    virtual             ~AbstractValueStack                 ();
};

} // namespace Volition
#endif
