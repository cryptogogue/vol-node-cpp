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
public:

    typedef SerializableMap < string, size_t >              Deck;
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
    Upgrades                mUpgrades;

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
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

        serializer.serialize ( "decks",             this->mDecks );
        serializer.serialize ( "definitions",       this->mDefinitions );
        serializer.serialize ( "fonts",             this->mFonts );
        serializer.serialize ( "icons",             this->mIcons );
        serializer.serialize ( "layouts",           this->mLayouts );
        serializer.serialize ( "methods",           this->mMethods );
        serializer.serialize ( "upgrades",          this->mUpgrades );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

        serializer.serialize ( "decks",             this->mDecks );
        serializer.serialize ( "definitions",       this->mDefinitions );
        serializer.serialize ( "fonts",             this->mFonts );
        serializer.serialize ( "icons",             this->mIcons );
        serializer.serialize ( "layouts",           this->mLayouts );
        serializer.serialize ( "methods",           this->mMethods );
        serializer.serialize ( "upgrades",          this->mUpgrades );
    }

public:

    //----------------------------------------------------------------//
    bool canUpgrade ( string type, string upgrade ) const {
    
        Upgrades::const_iterator typeIt = this->mUpgrades.find ( type );
        for ( ; typeIt != this->mUpgrades.cend (); typeIt = this->mUpgrades.find ( typeIt->second )) {
            if ( typeIt->second == upgrade ) return true;
        }
        return false;
    }

    //----------------------------------------------------------------//
    bool compose ( const Schema& other ) {
        
        Schema::merge ( this->mDecks, other.mDecks );
        Schema::merge ( this->mDefinitions, other.mDefinitions );
        Schema::merge ( this->mFonts, other.mFonts );
        Schema::merge ( this->mIcons, other.mIcons );
        Schema::merge ( this->mLayouts, other.mLayouts );
        Schema::merge ( this->mMethods, other.mMethods );
        Schema::merge ( this->mUpgrades, other.mUpgrades );
    
        return true;
    }

    //----------------------------------------------------------------//
    const Deck* getDeck ( string deckName ) const {
       Decks::const_iterator deckIt = this->mDecks.find ( deckName );
       return deckIt != this->mDecks.cend () ? &deckIt->second : NULL;
    }

    //----------------------------------------------------------------//
    const Definitions& getDefinitions () const {
        return this->mDefinitions;
    }

    //----------------------------------------------------------------//
    const AssetDefinition* getDefinitionOrNull ( string name ) const {
        Definitions::const_iterator definitionIt = this->mDefinitions.find ( name );
        return ( definitionIt != this->mDefinitions.cend ()) ? &definitionIt->second : NULL;
    }

    //----------------------------------------------------------------//
    const Methods& getMethods () const {
        return this->mMethods;
    }
    
    //----------------------------------------------------------------//
    const AssetMethod* getMethodOrNull ( string name ) const {
        Methods::const_iterator methodIt = this->mMethods.find ( name );
        return ( methodIt != this->mMethods.cend ()) ? &methodIt->second : NULL;
    }
    
    //----------------------------------------------------------------//
    const Upgrades& getUpgrades () const {
        return this->mUpgrades;
    }

    //----------------------------------------------------------------//
    bool hasCollisions ( const Schema& other ) {
    
        if ( Schema::hasKeyCollisions ( this->mDecks, other.mDecks )) return true;
        if ( Schema::hasKeyCollisions ( this->mDefinitions, other.mDefinitions )) return true;
        if ( Schema::hasKeyCollisions ( this->mFonts, other.mFonts )) return true;
        if ( Schema::hasKeyCollisions ( this->mIcons, other.mIcons )) return true;
        if ( Schema::hasKeyCollisions ( this->mLayouts, other.mLayouts )) return true;
        if ( Schema::hasKeyCollisions ( this->mMethods, other.mMethods )) return true;
        if ( Schema::hasKeyCollisions ( this->mUpgrades, other.mUpgrades )) return true;

        return false;
    }
};

} // namespace Volition
#endif
