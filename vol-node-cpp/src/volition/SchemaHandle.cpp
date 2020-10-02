// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Ledger.h>
#include <volition/SchemaHandle.h>

namespace Volition {

//================================================================//
// SchemaHandle
//================================================================//

//----------------------------------------------------------------//
const Schema* SchemaHandle::getSchema () const {
    return this->mSchema;
}

//----------------------------------------------------------------//
void SchemaHandle::reset ( Ledger& ledger ) {
    this->mSchema = &ledger.getSchema ();
}

//----------------------------------------------------------------//
SchemaHandle::SchemaHandle ( Ledger& ledger ) {
    this->reset ( ledger );
}

} // namespace Volition
