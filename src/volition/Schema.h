// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SCHEMA_H
#define VOLITION_SCHEMA_H

#include <volition/common.h>
#include <volition/Ledger.h>

namespace Volition {

//================================================================//
// Schema
//================================================================//
class Schema :
    public AbstractSerializable {
private:

    friend class Ledger;

    string          mName;
    lua_State*      mLuaState;

    //----------------------------------------------------------------//
    static int      _awardAsset         ( lua_State* L );

    //----------------------------------------------------------------//
    void            miningReward        ( Ledger& ledger, string rewardName );
    void            publish             ( Ledger& ledger );
    void            runRule             ( Ledger& ledger, string ruleName, AssetIdentifier* assets, size_t nAssets );

    //----------------------------------------------------------------//
    void            AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void            AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
                    Schema             ();
};

} // namespace Volition
#endif
