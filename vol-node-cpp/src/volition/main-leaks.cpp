// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <padamose/padamose.h>
#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/FileSys.h>
#include <volition/Miner.h>
#include <volition/MinerAPIFactory.h>
#include <volition/RouteTable.h>
#include <volition/simulation/AbstractScenario.h>
#include <volition/simulation/SimTransaction.h>
#include <volition/simulation/SimulatorActivity.h>
#include <volition/UnsecureRandom.h>
#include <volition/version.h>

using namespace Volition;
using namespace Simulation;

//================================================================//
// main
//================================================================//

//----------------------------------------------------------------//
int main ( int argc, char** argv ) {

    Lognosis::setFilter ( PDM_FILTER_ROOT, Lognosis::OFF );
    Lognosis::init ( argc, argv );
    LOG_F ( INFO, "Hello from main-leaks.cpp!" );

    CryptoKeyPair key;

    while ( true ) {
        key.elliptic ();
    }
    return 0;
}
