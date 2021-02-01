// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SCHEMA_HANDLE_H
#define VOLITION_SCHEMA_HANDLE_H

#include <volition/common.h>
#include <volition/Schema.h>

namespace Volition {

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
    const Schema* getSchema () const {
        return this->mSchema;
    }

    //----------------------------------------------------------------//
    void reset ( Ledger& ledger ) {
        this->mSchema = &ledger.getSchema ();
    }

    //----------------------------------------------------------------//
    SchemaHandle ( Ledger& ledger ) {
        this->reset ( ledger );
    }
};

} // namespace Volition
#endif
