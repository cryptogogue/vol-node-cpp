// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_GENESIS_H
#define VOLITION_GENESIS_H

#include <common.h>
#include <Singleton.h>

namespace Volition {

//================================================================//
// Genesis
//================================================================//
class Genesis :
    public Singleton < Genesis > {
private:

    unique_ptr < Poco::Crypto::ECKey >              mKey;
    unique_ptr < Poco::DigestEngine::Digest >       mDigest;

public:

    static const char* EC_CURVE;

    //----------------------------------------------------------------//
    string                                  getCurve            () const;
    const Poco::DigestEngine::Digest&       getDigest           () const;
    const Poco::Crypto::ECKey&              getKey              () const;
                                            Genesis             ();
    void                                    setDigest           ( const Poco::DigestEngine::Digest& digest );
    void                                    setKey              ( const Poco::Crypto::ECKey& key );
};

} // namespace Volition
#endif
