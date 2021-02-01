// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTLEDGERCOMPONENT_H
#define VOLITION_ABSTRACTLEDGERCOMPONENT_H

namespace Volition {

class Ledger;

//================================================================//
// Ledger
//================================================================//
class AbstractLedgerComponent {
protected:

    //----------------------------------------------------------------//
    virtual Ledger&         AbstractLedgerComponent_getLedger       () = 0;
    virtual const Ledger&   AbstractLedgerComponent_getLedger       () const = 0;

public:

    //----------------------------------------------------------------//
    AbstractLedgerComponent () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractLedgerComponent () {
    }

    //----------------------------------------------------------------//
    Ledger& getLedger () {
        return this->AbstractLedgerComponent_getLedger ();
    }

    //----------------------------------------------------------------//
    const Ledger& getLedger () const {
        return this->AbstractLedgerComponent_getLedger ();
    }
};

} // namespace Volition
#endif
