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

    static constexpr const char* PUBLISH_FUNC_NAME      = "publish";

    friend class Ledger;

    lua_State*  mLuaState;

    //----------------------------------------------------------------//
    static int      _createAsset        ( lua_State* L );

    //----------------------------------------------------------------//
    void            loadScript          ( string lua );
    void            miningReward        ( Ledger& ledger, string rewardName );
    void            publish             ( Ledger& ledger );
    void            runRule             ( Ledger& ledger, string ruleName, AssetIdentifier* assets, size_t nAssets );

public:

    //----------------------------------------------------------------//
                    Runtime             ();
};

} // namespace Volition
#endif
