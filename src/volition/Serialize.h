// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZE_H
#define VOLITION_SERIALIZE_H

#include <volition/common.h>
#include <volition/AbstractSerializable.h>

namespace Volition {
namespace Serialize {

//================================================================//
// Serialize
//================================================================//

//----------------------------------------------------------------//
u64         getU64FromJSON              ( const Poco::JSON::Object& object, string key, u64 opt );
void        getSerializableFromJSON     ( const Poco::JSON::Object& object, string key, AbstractSerializable& serializable );
void        getSerializableFromJSON     ( const Poco::JSON::Object& object, string key, Poco::DigestEngine::Digest& serializable );
void        setU64ToJSON                ( Poco::JSON::Object& object, string key, u64 value );
void        setSerializableToJSON       ( Poco::JSON::Object& object, string key, const AbstractSerializable& serializable );
void        setSerializableToJSON       ( Poco::JSON::Object& object, string key, const AbstractSerializable* serializable );
void        setSerializableToJSON       ( Poco::JSON::Object& object, string key, const Poco::Crypto::ECKey* serializable );
void        setSerializableToJSON       ( Poco::JSON::Object& object, string key, const Poco::DigestEngine::Digest& serializable );

//----------------------------------------------------------------//
template < typename TYPE >
unique_ptr < TYPE > getSerializableFromJSON ( const Poco::JSON::Object& object, string key ) {

    const Poco::JSON::Object::Ptr json = object.getObject ( key );
    if ( json ) {
        unique_ptr < TYPE > serializable = make_unique < TYPE >();
        serializable->fromJSON ( *json );
        return serializable;
    }
    return NULL;
}

//----------------------------------------------------------------//
template <> unique_ptr < Poco::Crypto::ECKey > getSerializableFromJSON < Poco::Crypto::ECKey >( const Poco::JSON::Object& object, string key );

} // namespace Singleton
} // namespace Volition
#endif
