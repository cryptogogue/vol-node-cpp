// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREETAG_H
#define VOLITION_BLOCKTREETAG_H

#include <volition/common.h>
#include <volition/Accessors.h>
#include <volition/BlockTreeCursor.h>

namespace Volition {

class AbstractBlockTree;

//================================================================//
// BlockTreeTag
//================================================================//
class BlockTreeTag {
private:

    friend class AbstractBlockTree;

    AbstractBlockTree*          mTree;
    string                      mName;
    
    //----------------------------------------------------------------//
    BlockTreeTag& operator = ( BlockTreeCursor ) {
        assert ( false );
        return *this;
    }
    
    //----------------------------------------------------------------//
    BlockTreeTag& operator = ( const BlockTreeTag& ) {
        assert ( false );
        return *this;
    }

public:

    GET_SET ( string,               Name,       mName )
    GET_SET ( AbstractBlockTree*,   Tree,       mTree )

    //----------------------------------------------------------------//
    BlockTreeCursor             operator *                      () const;
                                BlockTreeTag                    ();
                                BlockTreeTag                    ( string name );
                                BlockTreeTag                    ( BlockTreeTag& other );
                                ~BlockTreeTag                   ();
    bool                        check                           ( const AbstractBlockTree* tree ) const;
    bool                        equals                          ( const BlockTreeTag& rhs ) const;
    BlockTreeCursor             getCursor                       () const;
    bool                        hasCursor                       () const;
    bool                        hasName                         () const;
    void                        reset                           ();
};

} // namespace Volition
#endif
