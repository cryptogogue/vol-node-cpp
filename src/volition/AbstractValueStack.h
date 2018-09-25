// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTVALUESTACK_H
#define VOLITION_ABSTRACTVALUESTACK_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// AbstractValueStack
//================================================================//
/** \brief Sparse stack of values indexed by version. Intended to
    support fast lookup of a value for the most recent version.
*/
class AbstractValueStack {
protected:

    friend class AbstractVersionedValueIterator;
    friend class VersionedBranch;
    friend class VersionedStoreSnapshot;
    
    template < typename TYPE > friend class ValueStack;
    template < typename TYPE > friend class VersionedValueIterator;

    //----------------------------------------------------------------//
    void                                            copyValueFrom                               ( const AbstractValueStack& from, size_t version );
    void                                            erase                                       ( size_t version );
    void                                            join                                        ( AbstractValueStack& to ) const;
    unique_ptr < AbstractValueStack >               makeEmptyCopy                               () const;
    size_t                                          size                                        () const;

    //----------------------------------------------------------------//
    virtual void                                    AbstractValueStack_copyValueFrom            ( const AbstractValueStack& from, size_t version ) = 0;
    virtual void                                    AbstractValueStack_erase                    ( size_t version ) = 0;
    virtual void                                    AbstractValueStack_join                     ( AbstractValueStack& to ) const = 0;
    virtual unique_ptr < AbstractValueStack >       AbstractValueStack_makeEmptyCopy            () const = 0;
    virtual size_t                                  AbstractValueStack_size                     () const = 0;

public:

    //----------------------------------------------------------------//
                        AbstractValueStack                  ();
    virtual             ~AbstractValueStack                 ();
};

} // namespace Volition
#endif
