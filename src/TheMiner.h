#ifndef VLMINERSINGLETON_H
#define VLMINERSINGLETON_H

#include "common.h"

namespace Volition {

class AbstractHashable;

//================================================================//
// TheMiner
//================================================================//
class TheMiner {
private:

    unique_ptr < Poco::Crypto::ECKey >  mKey;

public:

    //----------------------------------------------------------------//
    static TheMiner&                get             ();
    void                            load            ( string keyfile, string password = "" );
    Poco::DigestEngine::Digest      sign            ( const AbstractHashable& hashable ) const;
                                    TheMiner        ();
                                    ~TheMiner       ();
};

} // namespace Volition
#endif
