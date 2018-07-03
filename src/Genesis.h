// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_GENESIS_H
#define VOLITION_GENESIS_H

#include "common.h"

namespace Volition {
namespace Genesis {

extern const char* PUBLIC_KEY_STRING;
extern const char* DIGEST_STRING;

//----------------------------------------------------------------//
Poco::Crypto::ECKey     getGenesisKey       ();

} // namespace Genesis
} // namespace Volition
#endif
