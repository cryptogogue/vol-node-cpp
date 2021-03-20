// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractConsensusInspector.h>
#include <volition/Block.h>

namespace Volition {

//================================================================//
// AbstractConsensusInspector
//================================================================//

//----------------------------------------------------------------//
AbstractConsensusInspector::AbstractConsensusInspector () {
}

//----------------------------------------------------------------//
AbstractConsensusInspector::~AbstractConsensusInspector () {
}

//----------------------------------------------------------------//
shared_ptr < const Block > AbstractConsensusInspector::getBlock ( string hash ) const {

    return this->AbstractConsensusInspector_getBlock ( hash );
}

} // namespace Volition
