// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETID_H
#define VOLITION_ASSETID_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// AssetID
//================================================================//
namespace AssetID {

    //----------------------------------------------------------------//
    u64             decode          ( string assetID, bool* isValid = NULL );
    string          encode          ( u64 index );
};

} // namespace Volition
#endif
