// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/FileSys.h>

namespace Volition {

//================================================================//
// FileSys
//================================================================//
namespace FileSys {

//----------------------------------------------------------------//
bool exists ( string path ) {
  struct stat buffer;
  return ( stat ( path.c_str(), &buffer ) == 0 );
}

} // namespace Format
} // namespace Volition
