// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Schema.h>

namespace Volition {

//================================================================//
// Schema
//================================================================//

//----------------------------------------------------------------//
bool Schema::canUpgrade ( string type, string upgrade ) const {

    Upgrades::const_iterator typeIt = this->mUpgrades.find ( type );
    for ( ; typeIt != this->mUpgrades.cend (); typeIt = this->mUpgrades.find ( typeIt->second )) {
        if ( typeIt->second == upgrade ) return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool Schema::compose ( const Schema& other ) {
    
    this->mDecks = other.mDecks;
    
    Schema::merge ( this->mDefinitions, other.mDefinitions );
    Schema::merge ( this->mFonts, other.mFonts );
    Schema::merge ( this->mIcons, other.mIcons );
    Schema::merge ( this->mLayouts, other.mLayouts );
    Schema::merge ( this->mMethods, other.mMethods );
    Schema::merge ( this->mSets, other.mSets );
    Schema::merge ( this->mUpgrades, other.mUpgrades );

    this->mVersion.compose ( other.mVersion );

    return true;
}

//----------------------------------------------------------------//
const Schema::Deck* Schema::getDeck ( string deckOrSetName ) const {

   Decks::const_iterator deckIt = this->mSets.find ( deckOrSetName );
   if ( deckIt != this->mSets.end ()) {
        return &deckIt->second;
   }
   deckIt = this->mDecks.find ( deckOrSetName );
   return deckIt != this->mDecks.cend () ? &deckIt->second : NULL;
}

//----------------------------------------------------------------//
const Schema::Definitions& Schema::getDefinitions () const {
    return this->mDefinitions;
}

//----------------------------------------------------------------//
const AssetDefinition* Schema::getDefinitionOrNull ( string name ) const {
    Definitions::const_iterator definitionIt = this->mDefinitions.find ( name );
    return ( definitionIt != this->mDefinitions.cend ()) ? &definitionIt->second : NULL;
}

//----------------------------------------------------------------//
const Schema::Methods& Schema::getMethods () const {
    return this->mMethods;
}

//----------------------------------------------------------------//
const AssetMethod* Schema::getMethodOrNull ( string name ) const {
    Methods::const_iterator methodIt = this->mMethods.find ( name );
    return ( methodIt != this->mMethods.cend ()) ? &methodIt->second : NULL;
}

//----------------------------------------------------------------//
const Schema::Upgrades& Schema::getUpgrades () const {
    return this->mUpgrades;
}

//----------------------------------------------------------------//
const SchemaVersion& Schema::getVersion () const {

    return this->mVersion;
}

//----------------------------------------------------------------//
bool Schema::hasAssetType ( string assetType ) const {

    return ( this->mDefinitions.find ( assetType ) != this->mDefinitions.cend ());
}

//----------------------------------------------------------------//
bool Schema::hasCollisions ( const Schema& other ) const {
    
    if ( Schema::hasKeyCollisions ( this->mDefinitions, other.mDefinitions )) return true;
    if ( Schema::hasKeyCollisions ( this->mFonts, other.mFonts )) return true;
    if ( Schema::hasKeyCollisions ( this->mIcons, other.mIcons )) return true;
    if ( Schema::hasKeyCollisions ( this->mLayouts, other.mLayouts )) return true;
    if ( Schema::hasKeyCollisions ( this->mMethods, other.mMethods )) return true;
    if ( Schema::hasKeyCollisions ( this->mSets, other.mSets )) return true;
    if ( Schema::hasKeyCollisions ( this->mUpgrades, other.mUpgrades )) return true;

    return false;
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void Schema::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    serializer.serialize ( "decks",             this->mDecks );
    serializer.serialize ( "definitions",       this->mDefinitions );
    serializer.serialize ( "fonts",             this->mFonts );
    serializer.serialize ( "icons",             this->mIcons );
    serializer.serialize ( "layouts",           this->mLayouts );
    serializer.serialize ( "methods",           this->mMethods );
    serializer.serialize ( "sets",              this->mSets );
    serializer.serialize ( "upgrades",          this->mUpgrades );
    serializer.serialize ( "version",           this->mVersion );
}

//----------------------------------------------------------------//
void Schema::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "decks",             this->mDecks );
    serializer.serialize ( "definitions",       this->mDefinitions );
    serializer.serialize ( "fonts",             this->mFonts );
    serializer.serialize ( "icons",             this->mIcons );
    serializer.serialize ( "layouts",           this->mLayouts );
    serializer.serialize ( "methods",           this->mMethods );
    serializer.serialize ( "sets",              this->mSets );
    serializer.serialize ( "upgrades",          this->mUpgrades );
    serializer.serialize ( "version",           this->mVersion );
}

} // namespace Volition
