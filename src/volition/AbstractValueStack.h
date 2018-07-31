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

    friend class VersionedStore;
    friend class VersionedStoreEpoch;
    template < typename TYPE > friend class ValueStack;

    size_t              mTypeID;

    //----------------------------------------------------------------//
    const void*                             getRaw                                  ( size_t version ) const;
    bool                                    isEmpty                                 () const;
    unique_ptr < AbstractValueStack >       makeEmptyCopy                           () const;
    void                                    pop                                     ();
    void                                    pushBackRaw                             ( const void* value, size_t version );
//    void                                    pushFrontRaw                            ( const void* value );

    //----------------------------------------------------------------//
    virtual const void*                             AbstractValueStack_getRaw           ( size_t version ) const = 0;
    virtual bool                                    AbstractValueStack_isEmpty          () const = 0;
    virtual unique_ptr < AbstractValueStack >       AbstractValueStack_makeEmptyCopy    () const = 0;
    virtual void                                    AbstractValueStack_pop              () = 0;
    virtual void                                    AbstractValueStack_pushBackRaw      ( const void* value, size_t version ) = 0;
//    virtual void                                    AbstractValueStack_pushFrontRaw     ( const void* value ) = 0;

public:

    //----------------------------------------------------------------//
                        AbstractValueStack                  ();
    virtual             ~AbstractValueStack                 ();
};

} // namespace Volition
#endif
