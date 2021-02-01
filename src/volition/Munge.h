// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MUNGE_H
#define VOLITION_MUNGE_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// Munge
//================================================================//
namespace Munge {

    //----------------------------------------------------------------//
    u64             munge           ( u64 number );
    u64             spin            ( u64 number );
    u64             unmunge         ( u64 number );
    u64             unspin          ( u64 number );
};

} // namespace Volition
#endif
