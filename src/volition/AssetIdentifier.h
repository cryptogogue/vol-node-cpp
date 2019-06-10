// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETIDENTIFIER_H
#define VOLITION_ASSETIDENTIFIER_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// AssetIdentifier
//================================================================//
class AssetIdentifier {
public:

    string      mType;
    size_t      mIndex;
    
    //----------------------------------------------------------------//
    AssetIdentifier ( string type, size_t index ) :
        mType ( type ),
        mIndex ( index ) {
    }
};

} // namespace Volition
#endif
