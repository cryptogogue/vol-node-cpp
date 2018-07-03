//
//  main.cpp
// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "Genesis.h"

namespace Volition {
namespace Genesis {

const char* PUBLIC_KEY_STRING = "-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEzfUD/qtmfa4H2NJ+vriC8SNm0bUe8dMx\nBlOig2FzzOZE1iVu3u1Tgvvr4MsYmY2KYxOt+zHWcT5DlSKmrmaQqw==\n-----END PUBLIC KEY-----\n";
const char* DIGEST_STRING = "caf28a0415ff5fc3edf6a87ff5f0f11fe8b4bbb1bc6343a7b715887325647167";

//----------------------------------------------------------------//
Poco::Crypto::ECKey getGenesisKey () {

    stringstream genesisKeyStream ( Genesis::PUBLIC_KEY_STRING );
    return Poco::Crypto::ECKey ( &genesisKeyStream );
}

} // namespace Genesis
} // namespace Volition
