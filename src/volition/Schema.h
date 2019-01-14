// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SCHEMA_H
#define VOLITION_SCHEMA_H

#include <volition/common.h>
#include <volition/Ledger.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// SchemaClass
//================================================================//
class SchemaClass :
     public AbstractSerializable {
public:

    string                              mDisplayName;       // friendly name for the class.
    SerializableSet < string >          mKeyWords;          // array of keywords.
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "displayName",       this->mDisplayName );
        serializer.serialize ( "keywords",          this->mKeyWords );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        assert ( false ); // unsupported
    }
};

//================================================================//
// SchemaRule
//================================================================//
class SchemaRule :
     public AbstractSerializable {
public:

    string                                                                          mDescription;       // friendly description for the rule.
    SerializableVector < SerializableSharedPtr < AbstractSquap, SquapFactory >>     mQualifiers;        // list of qualifier trees.
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "description",       this->mDescription );
        serializer.serialize ( "qualifiers",        this->mQualifiers );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        assert ( false ); // unsupported
    }
};

//================================================================//
// Schema
//================================================================//
class Schema :
    public AbstractSerializable {
private:

    friend class Ledger;

    string          mName;
    lua_State*      mLuaState;

    SerializableMap < string, SchemaClass >     mClasses;
    SerializableMap < string, SchemaRule >      mRules;

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
