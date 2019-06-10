// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SCHEMA_H
#define VOLITION_SCHEMA_H

#include <volition/common.h>
#include <volition/AssetDefinition.h>
#include <volition/AssetMethod.h>
#include <volition/Ledger.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// Schema
//================================================================//
class Schema :
    public AbstractSerializable {
private:

    friend class Ledger;
    friend class SchemaLua;

    typedef SerializableMap < string, AssetDefinition >     Definitions;
    typedef SerializableMap < string, AssetMethod >         Methods;

    string                  mName;

    Definitions             mDefinitions;
    Methods                 mMethods;

    string                  mLua;

    //----------------------------------------------------------------//
    const AssetMethod*      getMethod               ( string name ) const;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
                            Schema                  ();
    bool                    verifyMethod            ( string methodName, u64 weight, u64 maturity ) const;
};

} // namespace Volition
#endif
