// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <Serialize.h>

namespace Volition {
namespace Serialize {

//================================================================//
// Serialize
//================================================================//

//----------------------------------------------------------------//
u64 getU64FromJSON ( const Poco::JSON::Object& object, string key, u64 opt ) {

    string strValue = object.optValue < string >( key, "" );
    if ( strValue.size () > 0 ) {
    
        assert ( sizeof ( unsigned long long int ) == sizeof ( u64 ));
    
        errno = 0;
        u64 value = strtoull ( strValue.c_str (), NULL, 16 );
        if ( errno == 0 ) {
            return value;
        }
    }
    return opt;
}

//----------------------------------------------------------------//
void getSerializableFromJSON ( const Poco::JSON::Object& object, string key, AbstractSerializable& serializable ) {

    const Poco::JSON::Object::Ptr json = object.getObject ( key );
    if ( json ) {
        serializable.fromJSON ( *json );
    }
}

//----------------------------------------------------------------//
void getSerializableFromJSON ( const Poco::JSON::Object& object, string key, Poco::DigestEngine::Digest& serializable ) {

    string digestString = object.optValue < string >( key, "" );
    serializable = digestString.size () ? Poco::DigestEngine::digestFromHex ( digestString ) : Poco::DigestEngine::Digest ();
}

//----------------------------------------------------------------//
void setU64ToJSON ( Poco::JSON::Object& object, string key, u64 value ) {

    char buffer [ 32 ];
    snprintf ( buffer, 32, "%" PRIx64 "", value );
    object.set ( key, string ( buffer ).c_str ());
}

//----------------------------------------------------------------//
void setSerializableToJSON ( Poco::JSON::Object& object, string key, const AbstractSerializable& serializable ) {
    object.set ( key, serializable.toJSON ());
}

//----------------------------------------------------------------//
void setSerializableToJSON ( Poco::JSON::Object& object, string key, const AbstractSerializable* serializable ) {
    if ( serializable ) {
        object.set ( key, serializable->toJSON ());
    }
}

//----------------------------------------------------------------//
void setSerializableToJSON ( Poco::JSON::Object& object, string key, const Poco::Crypto::ECKey* serializable ) {
    if ( serializable ) {
        stringstream keyString;
        serializable->save ( &keyString );
        object.set ( key, keyString.str ().c_str ());
    }
}

//----------------------------------------------------------------//
void setSerializableToJSON ( Poco::JSON::Object& object, string key, const Poco::DigestEngine::Digest& serializable ) {

    object.set ( key, Poco::DigestEngine::digestToHex ( serializable).c_str ());
}

//================================================================//
// Serialize <>
//================================================================//

//----------------------------------------------------------------//
template <>
unique_ptr < Poco::Crypto::ECKey > getSerializableFromJSON < Poco::Crypto::ECKey >( const Poco::JSON::Object& object, string key ) {

    string keyString = object.optValue < string >( key, "" );
    stringstream keyStream ( keyString );
    return keyString.size () ? make_unique < Poco::Crypto::ECKey >( &keyStream ) : NULL;
}

} // namespace Serialize
} // namespace Volition
