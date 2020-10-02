// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SCHEMA_HANDLE_H
#define VOLITION_SCHEMA_HANDLE_H

#include <volition/common.h>
#include <volition/Schema.h>

namespace Volition {

class Ledger;

//================================================================//
// SchemaHandle
//================================================================//
class SchemaHandle {
private:

    const Schema*   mSchema;

public:

    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mSchema != NULL );
    }
    
    //----------------------------------------------------------------//
    const Schema* operator -> () const {
        return this->getSchema ();
    }
    
    //----------------------------------------------------------------//
    const Schema& operator * () const {
        assert ( this->mSchema );
        return *this->mSchema;
    }

    //----------------------------------------------------------------//
    const Schema*       getSchema               () const;
    void                reset                   ( Ledger& ledger );
                        SchemaHandle            ( Ledger& ledger );
};

} // namespace Volition
#endif
