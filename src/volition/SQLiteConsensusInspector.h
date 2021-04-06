// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SQLITECONSENSUSINSPECTOR_H
#define VOLITION_SQLITECONSENSUSINSPECTOR_H

#include <volition/common.h>
#include <volition/AbstractConsensusInspector.h>

namespace Volition {

//================================================================//
// SQLiteConsensusInspector
//================================================================//
class SQLiteConsensusInspector :
    public AbstractConsensusInspector {
protected:

    string              mBlockTreeFilename;

    //----------------------------------------------------------------//
    shared_ptr < const Block >      AbstractConsensusInspector_getBlock     ( string hash ) const override;

public:

    //----------------------------------------------------------------//
                SQLiteConsensusInspector        ( string blockTreeFilename );
                ~SQLiteConsensusInspector       ();
};

} // namespace Volition
#endif
