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

    typedef SerializableMap < string, AssetDefinition >     Definitions;
    typedef SerializableMap < string, SerializableOpaque >  Fonts;
    typedef SerializableMap < string, SerializableOpaque >  Icons;
    typedef SerializableMap < string, SerializableOpaque >  Layouts;
    typedef SerializableMap < string, AssetMethod >         Methods;
    typedef SerializableMap < string, string >              Upgrades;

private:

    friend class Ledger;
    friend class LuaContext;

    Definitions             mDefinitions;
    Fonts                   mFonts;
    Icons                   mIcons;
    Layouts                 mLayouts;
    Methods                 mMethods;
    Upgrades                mUpgrades;

    //----------------------------------------------------------------//
    template < typename TYPE >
    static bool hasCollisions ( const TYPE& container0, const TYPE& container1 ) {
        
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

        serializer.serialize ( "definitions",       this->mDefinitions );
        serializer.serialize ( "fonts",             this->mFonts );
        serializer.serialize ( "icons",             this->mIcons );
        serializer.serialize ( "layouts",           this->mLayouts );
        serializer.serialize ( "methods",           this->mMethods );
        serializer.serialize ( "upgrades",          this->mUpgrades );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

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
    
        if ( Schema::hasCollisions ( this->mDefinitions, other.mDefinitions )) return false;
        if ( Schema::hasCollisions ( this->mFonts, other.mFonts )) return false;
        if ( Schema::hasCollisions ( this->mIcons, other.mIcons )) return false;
        if ( Schema::hasCollisions ( this->mLayouts, other.mLayouts )) return false;
        if ( Schema::hasCollisions ( this->mMethods, other.mMethods )) return false;
        if ( Schema::hasCollisions ( this->mUpgrades, other.mUpgrades )) return false;
    
        Schema::merge ( this->mDefinitions, other.mDefinitions );
        Schema::merge ( this->mFonts, other.mFonts );
        Schema::merge ( this->mIcons, other.mIcons );
        Schema::merge ( this->mLayouts, other.mLayouts );
        Schema::merge ( this->mMethods, other.mMethods );
        Schema::merge ( this->mUpgrades, other.mUpgrades );
    
        return true;
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
};

} // namespace Volition
#endif
