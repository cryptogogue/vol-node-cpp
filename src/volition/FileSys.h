// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_FILESYS_H
#define VOLITION_FILESYS_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// FileSys
//================================================================//
namespace FileSys {

    //----------------------------------------------------------------//
    void            createDirectories   ( string path );
    bool            exists              ( string path );
    void            getFilenames        ( string path, set < string > files );
    size_t          getDirSize          ( string path );
};

} // namespace Volition
#endif
