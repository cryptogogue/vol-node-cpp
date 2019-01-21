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
const SchemaMethod* Schema::getMethod ( string name ) const {

    SerializableMap < string, SchemaMethod >::const_iterator methodIt = this->mMethods.find ( name );
    if ( methodIt != this->mMethods.end ()) {
        return &methodIt->second;
    }
    return NULL;
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

    assert ( false ); // unsupported
}

} // namespace Volition
