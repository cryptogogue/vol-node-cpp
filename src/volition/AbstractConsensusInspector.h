// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTCONSENSUSINSPECTOR_H
#define VOLITION_ABSTRACTCONSENSUSINSPECTOR_H

#include <volition/common.h>

namespace Volition {

class Block;
class Ledger;

//================================================================//
// AbstractConsensusInspector
//================================================================//
class AbstractConsensusInspector {
protected:

    //----------------------------------------------------------------//
    virtual shared_ptr < const Block >      AbstractConsensusInspector_getBlock         ( string hash ) const = 0;

public:

    //----------------------------------------------------------------//
                                    AbstractConsensusInspector          ();
    virtual                         ~AbstractConsensusInspector         ();
    shared_ptr < const Block >      getBlock                            ( string hash ) const;
};

} // namespace Volition
#endif
