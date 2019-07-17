/* eslint-disable no-whitespace-before-property */

import { action, computed, extendObservable, observable, observe } from 'mobx';

//----------------------------------------------------------------//
export const TRANSACTION_TYPE = {
    ACCOUNT_POLICY:     'ACCOUNT_POLICY',
    AFFIRM_KEY:         'AFFIRM_KEY',
    KEY_POLICY:         'KEY_POLICY',
    OPEN_ACCOUNT:       'OPEN_ACCOUNT',
    REGISTER_MINER:     'REGISTER_MINER',
    RUN_SCRIPT:         'RUN_SCRIPT',
    SEND_VOL:           'SEND_VOL',
};

//================================================================//
// Transaction
//================================================================//
export class Transaction {

    //----------------------------------------------------------------//
    constructor ( type, body ) {
        
        this.type = type;
        body.type = type;

        extendObservable ( this, {
            body:    body,
        });
    }

    //----------------------------------------------------------------//
    static friendlyNameForType ( type ) {

        switch ( type ) {
            case TRANSACTION_TYPE.ACCOUNT_POLICY:   return 'Account Policy';
            case TRANSACTION_TYPE.AFFIRM_KEY:       return 'Affirm Key';
            case TRANSACTION_TYPE.KEY_POLICY:       return 'Key Policy';
            case TRANSACTION_TYPE.OPEN_ACCOUNT:     return 'Open Account';
            case TRANSACTION_TYPE.REGISTER_MINER:   return 'Register Miner';
            case TRANSACTION_TYPE.RUN_SCRIPT:       return 'Run Script';
            case TRANSACTION_TYPE.SEND_VOL:         return 'Send VOL';
        }
    }

    //----------------------------------------------------------------//
    getCost () {

        return this.body.maker.gratuity || 0;
    }

    //----------------------------------------------------------------//
    @action
    setBody ( body ) {

        this.body = body;
    }

    //----------------------------------------------------------------//
    static transactionWithBody ( type, body ) {

        switch ( type ) {
            case TRANSACTION_TYPE.ACCOUNT_POLICY:   return new Transaction ( type, body );
            case TRANSACTION_TYPE.AFFIRM_KEY:       return new Transaction ( type, body );
            case TRANSACTION_TYPE.KEY_POLICY:       return new Transaction ( type, body );
            case TRANSACTION_TYPE.OPEN_ACCOUNT:     return new Transaction ( type, body );
            case TRANSACTION_TYPE.REGISTER_MINER:   return new Transaction ( type, body );
            case TRANSACTION_TYPE.RUN_SCRIPT:       return new Transaction ( type, body );
            case TRANSACTION_TYPE.SEND_VOL:         return new SendVol ( type, body );
        }
    }
};

//================================================================//
// SendVol
//================================================================//
class SendVol extends Transaction {

    //----------------------------------------------------------------//
    getCost () {

        return super.getCost () + ( this.body.amount || 0 );
    }
};
