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
        return Poco::DigestEngine::digestToHex ( *this );
    }

public:
    
    using AbstractStringifiable::operator =;
    
    //----------------------------------------------------------------//
    Digest () {
    }
    
    //----------------------------------------------------------------//
    Digest ( const Poco::DigestEngine::Digest& digest ) :
        Poco::DigestEngine::Digest ( digest ) {
    }
    
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
};

} // namespace Volition
#endif
