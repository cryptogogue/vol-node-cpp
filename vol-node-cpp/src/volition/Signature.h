// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIGNATURE_H
#define VOLITION_SIGNATURE_H

#include <volition/common.h>
#include <volition/Digest.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// Signature
//================================================================//
class Signature :
    public AbstractSerializable {
private:

    Digest      mDigest;
    Digest      mSignature;
    string      mHashAlgorithm;

protected:

    //----------------------------------------------------------------//
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mSignature.size () > 0 );
    }

    //----------------------------------------------------------------//
    bool operator == ( const Signature& rhs ) const {
        return ( Poco::DigestEngine::constantTimeEquals ( this->mSignature, rhs.mSignature ));
    }
    
    //----------------------------------------------------------------//
    bool operator != ( const Signature& rhs ) const {
        return !( *this == rhs );
    }

    //----------------------------------------------------------------//
    const Digest&       getDigest           () const;
    string              getHashAlgorithm    () const;
    const Digest&       getSignature        () const;
                        Signature           ();
                        Signature           ( Digest digest, Digest signature, string hashAlgorithm );
                        ~Signature          ();
};

} // namespace Volition
#endif
