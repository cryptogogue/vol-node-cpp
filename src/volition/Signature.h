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
    void                AbstractSerializable_serialize      ( AbstractSerializer& serializer ) override;

public:

    static constexpr const char* DEFAULT_HASH_ALGORITHM = "SHA256";

    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mSignature.size () > 0 );
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
