//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "AbstractHashable.h"
#include "TheMiner.h"

namespace Volition {

//================================================================//
// TheMiner
//================================================================//

//----------------------------------------------------------------//
TheMiner& TheMiner::get () {
    static Poco::SingletonHolder < TheMiner > single;
    return *single.get ();
}

//----------------------------------------------------------------//
void TheMiner::load ( string keyfile, string password ) {

    this->mKey = make_unique < Poco::Crypto::ECKey >( "", keyfile, password );
}

//----------------------------------------------------------------//
Poco::DigestEngine::Digest TheMiner::sign ( const AbstractHashable& hashable ) const {

    assert ( this->mKey );

    Poco::Crypto::ECDSADigestEngine signature ( *this->mKey, "SHA256" );
    Poco::DigestOutputStream signatureStream ( signature );
    hashable.Hash ( signatureStream );
    signatureStream.close ();
    
    return signature.signature ();
}

//----------------------------------------------------------------//
TheMiner::TheMiner () {
}

//----------------------------------------------------------------//
TheMiner::~TheMiner () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition

