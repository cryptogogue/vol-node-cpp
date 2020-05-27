// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_DIGEST_H
#define VOLITION_DIGEST_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// Digest
//================================================================//
class Digest :
    public Poco::DigestEngine::Digest,
    public AbstractStringifiable {
private:

    //----------------------------------------------------------------//
    void AbstractStringifiable_fromString ( string str ) {
        *this = Poco::DigestEngine::digestFromHex ( str );
    }
    
    //----------------------------------------------------------------//
    string AbstractStringifiable_toString () const {
        return this->toHex ();
    }

public:
    
    static constexpr const char* HASH_ALGORITHM_MD5         = "MD5";
    static constexpr const char* HASH_ALGORITHM_SHA256      = "SHA256";
    static constexpr const char* DEFAULT_HASH_ALGORITHM     = HASH_ALGORITHM_SHA256;
    
    using AbstractStringifiable::operator =;
    
    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->size () > 0 );
    }

    //----------------------------------------------------------------//
    Digest& operator = ( const Poco::DigestEngine::Digest& digest ) {
        *this = Digest ( digest );
        return *this;
    }
    
    //----------------------------------------------------------------//
    bool operator == ( const Poco::DigestEngine::Digest& other ) const {
        return Poco::DigestEngine::constantTimeEquals ( *this, other );
    }
    
    //----------------------------------------------------------------//
    bool operator != ( const Poco::DigestEngine::Digest& other ) const {
        return !( *this == other );
    }
    
    //----------------------------------------------------------------//
    Digest () {
    }
    
    //----------------------------------------------------------------//
    Digest ( string input, string hashAlgorithm = DEFAULT_HASH_ALGORITHM ) {
    
        Poco::Crypto::DigestEngine digestEngine ( hashAlgorithm );
        Poco::DigestOutputStream digestStream ( digestEngine );
        digestStream << input;
        digestStream.close ();
        
        *this = digestEngine.digest ();
    }
    
    //----------------------------------------------------------------//
    Digest ( const AbstractSerializable& serializable, string hashAlgorithm = DEFAULT_HASH_ALGORITHM ) {
    
        Poco::Crypto::DigestEngine digestEngine ( hashAlgorithm );
        Poco::DigestOutputStream digestStream ( digestEngine );
        ToJSONSerializer::toDigest ( serializable, digestStream );
        digestStream.close ();
        
        *this = digestEngine.digest ();
    }
    
    //----------------------------------------------------------------//
    Digest ( const Poco::DigestEngine::Digest& digest ) :
        Poco::DigestEngine::Digest ( digest ) {
    }
    
    //----------------------------------------------------------------//
    string toHex () const {
        return Poco::DigestEngine::digestToHex ( *this );
    }
};

} // namespace Volition
#endif
