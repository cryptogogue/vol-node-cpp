// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_RUNTIME_H
#define VOLITION_RUNTIME_H

#include <volition/common.h>
#include <volition/Ledger.h>

namespace Volition {

//================================================================//
// Runtime
//================================================================//
class Runtime {
private:

    friend class Ledger;

    lua_State*  mLuaState;

    //----------------------------------------------------------------//
    static int      _awardAsset         ( lua_State* L );

    //----------------------------------------------------------------//
    void            loadScript          ( Ledger& ledger, string schemaName, string lua );
    void            miningReward        ( string rewardName );
    void            publish             ();
    void            runRule             ( string ruleName, AssetIdentifier* assets, size_t nAssets );

public:

    //----------------------------------------------------------------//
                    Runtime             ();
};

} // namespace Volition
#endif
