// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// Schema
//================================================================//

//----------------------------------------------------------------//
void Schema::composeTemplate ( string name, SchemaAssetTemplate& assetTemplate ) const {

    if ( name.size () == 0 ) return;

    AssetTemplates::const_iterator templateIt = this->mAssetTemplates.find ( name );
    if ( templateIt != this->mAssetTemplates.cend ()) {
        const SchemaAssetTemplate& sourceTemplate = templateIt->second;
        
        this->composeTemplate ( sourceTemplate.mExtends, assetTemplate );
        assetTemplate.mExtends = sourceTemplate.mExtends;
        
        SchemaAssetTemplate::Fields::const_iterator fieldIt = sourceTemplate.mFields.cbegin ();
        for ( ; fieldIt != sourceTemplate.mFields.cend (); ++fieldIt ) {
            assetTemplate.mFields [ fieldIt->first ] = fieldIt->second;
        }
    }
}

//----------------------------------------------------------------//
const SchemaMethod* Schema::getMethod ( string name ) const {

    SerializableMap < string, SchemaMethod >::const_iterator methodIt = this->mMethods.find ( name );
    if ( methodIt != this->mMethods.end ()) {
        return &methodIt->second;
    }
    return NULL;
}

//----------------------------------------------------------------//
SchemaAssetTemplate Schema::getTemplate ( string name ) const {

    SchemaAssetTemplate assetTemplate;
    this->composeTemplate ( name, assetTemplate );
    return assetTemplate;
}

//----------------------------------------------------------------//
Schema::Schema () {
}

//----------------------------------------------------------------//
bool Schema::verifyMethod ( string methodName, u64 weight, u64 maturity ) const {

    const SchemaMethod* method = this->getMethod ( methodName );
    return ( method && ( method->mWeight == weight ) && ( method->mMaturity == maturity ));
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Schema::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    serializer.serialize ( "name",              this->mName );
    serializer.serialize ( "assetTemplates",    this->mAssetTemplates );
    serializer.serialize ( "assetDefinitions",  this->mAssetDefinitions );
    serializer.serialize ( "methods",           this->mMethods );
    serializer.serialize ( "lua",               this->mLua );
}

//----------------------------------------------------------------//
void Schema::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "name",              this->mName );
    serializer.serialize ( "assetTemplates",    this->mAssetTemplates );
    serializer.serialize ( "assetDefinitions",  this->mAssetDefinitions );
    serializer.serialize ( "methods",           this->mMethods );
    serializer.serialize ( "lua",               this->mLua );
}

} // namespace Volition
