// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTLEDGERCOMPONENT_H
#define VOLITION_ABSTRACTLEDGERCOMPONENT_H

namespace Volition {

class AbstractLedger;

//================================================================//
// Ledger
//================================================================//
class AbstractLedgerComponent {
protected:

    //----------------------------------------------------------------//
    virtual AbstractLedger&         AbstractLedgerComponent_getLedger       () = 0;
    virtual const AbstractLedger&   AbstractLedgerComponent_getLedger       () const = 0;

public:

    //----------------------------------------------------------------//
    AbstractLedgerComponent () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractLedgerComponent () {
    }

    //----------------------------------------------------------------//
    AbstractLedger& getLedger () {
        return this->AbstractLedgerComponent_getLedger ();
    }

    //----------------------------------------------------------------//
    const AbstractLedger& getLedger () const {
        return this->AbstractLedgerComponent_getLedger ();
    }
};

} // namespace Volition
#endif
