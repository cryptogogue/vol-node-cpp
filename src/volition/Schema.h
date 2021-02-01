// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SCHEMA_H
#define VOLITION_SCHEMA_H

#include <volition/common.h>
#include <volition/AssetDefinition.h>
#include <volition/AssetMethod.h>
#include <volition/SchemaVersion.h>
#include <volition/SquapFactory.h>

namespace Volition {

//================================================================//
// Schema
//================================================================//
class Schema :
    public AbstractSerializable {
public:

    typedef SerializableSharedPtr < AbstractSquap, SquapFactory > Query;

    typedef SerializableMap < string, u64 >                 Deck;
    typedef SerializableMap < string, Deck >                Decks;
    typedef SerializableMap < string, AssetDefinition >     Definitions;
    typedef SerializableMap < string, SerializableOpaque >  Fonts;
    typedef SerializableMap < string, SerializableOpaque >  Icons;
    typedef SerializableMap < string, SerializableOpaque >  Layouts;
    typedef SerializableMap < string, AssetMethod >         Methods;
    typedef SerializableMap < string, string >              Upgrades;

private:

    friend class Ledger;
    friend class LuaContext;

    Decks                   mDecks;
    Definitions             mDefinitions;
    Fonts                   mFonts;
    Icons                   mIcons;
    Layouts                 mLayouts;
    Methods                 mMethods;
    Decks                   mSets;
    Upgrades                mUpgrades;
    SchemaVersion           mVersion;

    //----------------------------------------------------------------//
    template < typename TYPE >
    static bool hasKeyCollisions ( const TYPE& container0, const TYPE& container1 ) {
        
        typename TYPE::const_iterator itr = container1.cbegin ();
        for ( ; itr != container1.cend (); ++itr ) {
            if ( container0.find ( itr->first ) != container0.cend ()) return true;
        }
        return false;
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    static void merge ( TYPE& container0, const TYPE& container1 ) {
        
        typename TYPE::const_iterator itr = container1.cbegin ();
        for ( ; itr != container1.cend (); ++itr ) {
            container0 [ itr->first ] = itr->second;
        }
    }

    //----------------------------------------------------------------//
    void            AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer );
    void            AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const;

public:

    //----------------------------------------------------------------//
    bool                        canUpgrade                  ( string type, string upgrade ) const;
    bool                        compose                     ( const Schema& other );
    const Deck*                 getDeck                     ( string deckOrSetName ) const;
    const Definitions&          getDefinitions              () const;
    const AssetDefinition*      getDefinitionOrNull         ( string name ) const;
    const Methods&              getMethods                  () const;
    const AssetMethod*          getMethodOrNull             ( string name ) const;
    const Upgrades&             getUpgrades                 () const;
    const SchemaVersion&        getVersion                  () const;
    bool                        hasAssetType                ( string assetType ) const;
    bool                        hasCollisions               ( const Schema& other ) const;
};

} // namespace Volition
#endif
