// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINERLAUNCHTESTS_H
#define VOLITION_MINERLAUNCHTESTS_H

namespace Volition {

//================================================================//
// MinerLaunchTests
//================================================================//
class MinerLaunchTests {
public:
    
    //----------------------------------------------------------------//
    static bool             checkDeterministic          ( const CryptoKey& key, size_t cycles, size_t expectedResult );
    static void             checkEnvironment            ();
};

} // namespace Volition
#endif
