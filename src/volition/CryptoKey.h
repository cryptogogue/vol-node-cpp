// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CRYPTOKEY_H
#define VOLITION_CRYPTOKEY_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// CryptoKey
//================================================================//
class CryptoKey :
    public AbstractSerializable {
private:

    shared_ptr < Poco::Crypto::KeyPair >      mKeyPair;
    
    //----------------------------------------------------------------//
    void            AbstractSerializable_serialize       ( AbstractSerializer& serializer );

public:

    static const char*  DEFAULT_EC_GROUP_NAME;
    static const int    DEFAULT_EC_GROUP_NID = NID_secp256k1;

    //----------------------------------------------------------------//
                    CryptoKey               ();
                    ~CryptoKey              ();
    void            elliptic                ( int nid = DEFAULT_EC_GROUP_NID );
    void            elliptic                ( string groupName = DEFAULT_EC_GROUP_NAME );
    static string   getGroupNameFromNID     ( int nid );
    static int      getNIDFromGroupName     ( string groupName );
    static bool     hasCurve                ( int nid );
    static bool     hasCurve                ( string groupName );
    
    //----------------------------------------------------------------//
    operator const bool () const {
        return this->mKeyPair ? true : false;
    }
    
    //----------------------------------------------------------------//
    operator const Poco::Crypto::KeyPair* () const {
        return this->mKeyPair.get ();
    }
};

} // namespace Volition
#endif
