// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_THECONTEXT_H
#define VOLITION_THECONTEXT_H

#include <volition/common.h>
#include <volition/Singleton.h>

namespace Volition {

//================================================================//
// TheContext
//================================================================//
class TheContext :
    public Singleton < TheContext > {
private:

    unique_ptr < Poco::Crypto::ECKey >              mKey;
    unique_ptr < Poco::DigestEngine::Digest >       mDigest;

public:

    static const char* EC_CURVE;

    //----------------------------------------------------------------//
    string                                  getCurve                        () const;
    const Poco::DigestEngine::Digest&       getGenesisBlockDigest           () const;
    const Poco::Crypto::ECKey&              getGenesisBlockKey              () const;
                                            TheContext                      ();
    void                                    setGenesisBlockDigest           ( const Poco::DigestEngine::Digest& digest );
    void                                    setGenesisBlockKey              ( const Poco::Crypto::ECKey& key );
};

} // namespace Volition
#endif
