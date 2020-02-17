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

    shared_ptr < Schema >   mSchema;

public:

    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mSchema != NULL );
    }
    
    //----------------------------------------------------------------//
    const Schema* operator -> () const {
        return this->mSchema.get ();
    }
    
    //----------------------------------------------------------------//
    const Schema& operator * () const {
        assert ( this->mSchema );
        return *this->mSchema;
    }

    //----------------------------------------------------------------//
    void reset ( const Ledger& ledger ) {
        this->mSchema = make_shared < Schema >();
        ledger.getSchema ( *this->mSchema );
    }

    //----------------------------------------------------------------//
    SchemaHandle ( const Ledger& ledger ) {
        this->reset ( ledger );
    }
};

} // namespace Volition
#endif
