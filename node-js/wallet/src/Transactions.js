/* eslint-disable no-whitespace-before-property */

import { action, computed, extendObservable, observable, observe } from 'mobx';

//----------------------------------------------------------------//
export const TRANSACTION_TYPE = {
    ACCOUNT_POLICY:     'ACCOUNT_POLICY',
    AFFIRM_KEY:         'AFFIRM_KEY',
    KEY_POLICY:         'KEY_POLICY',
    OPEN_ACCOUNT:       'OPEN_ACCOUNT',
    REGISTER_MINER:     'REGISTER_MINER',
    SEND_VOL:           'SEND_VOL',
};

//----------------------------------------------------------------//
function integerField ( hashOrder ) {
    return {
        hashOrder:      hashOrder,
        fieldType:      'INTEGER',
    };
}

//----------------------------------------------------------------//
function stringField ( hashOrder ) {
    return {
        hashOrder:      hashOrder,
        fieldType:      'STRING',
    };
}

//----------------------------------------------------------------//
function makerFormat () {
    return {
        accountName:        'makerAccountName',
        gratuity:           'gratuity',
        keyName:            'makerKeyName',
        nonce:              'makerNonce',
    };
}

//----------------------------------------------------------------//
const accountPolicy = {
    transactionType:    TRANSACTION_TYPE.ACCOUNT_POLICY,
    friendlyName:       'Account Policy',
    fields: {
        makerAccountName:           stringField ( 0 ),
        makerKeyName:               stringField ( 1 ),
        makerNonce:                 integerField ( 2 ),
        policyName:                 stringField ( 3 ),
        policy:                     stringField ( 4 ),
        gratuity:                   integerField ( 5 ),
    },
    format: {
        maker:                      makerFormat (),
        policy:                     'policy',
        policyName:                 'policyName',
    },
}

//----------------------------------------------------------------//
const affirmKey = {
    transactionType:    TRANSACTION_TYPE.AFFIRM_KEY,
    friendlyName:       'Affirm Key',
    fields: {
        makerAccountName:           stringField ( 0 ),
        makerKeyName:               stringField ( 1 ),
        makerNonce:                 integerField ( 2 ),
        keyName:                    stringField ( 3 ),
        key:                        stringField ( 4 ),
        policyName:                 stringField ( 5 ),
        gratuity:                   integerField ( 6 ),
    },
    format: {
        maker:                      makerFormat (),
        key:                        'key',
        keyName:                    'keyName',
        policyName:                 'policyName',
    },
}

//----------------------------------------------------------------//
const keyPolicy = {
    transactionType:    TRANSACTION_TYPE.KEY_POLICY,
    friendlyName:       'Key Policy',
    fields: {
        makerAccountName:           stringField ( 0 ),
        makerKeyName:               stringField ( 1 ),
        makerNonce:                 integerField ( 2 ),
        policyName:                 stringField ( 3 ),
        policy:                     stringField ( 4 ),
        gratuity:                   integerField ( 5 ),
    },
    format: {
        maker:                      makerFormat (),
        policy:                     'policy',
        policyName:                 'policyName',
    },
}

//----------------------------------------------------------------//
const openAccount = {
    transactionType:    TRANSACTION_TYPE.OPEN_ACCOUNT,
    friendlyName:       'Open Account',
    fields: {
        makerAccountName:           stringField ( 0 ),
        makerKeyName:               stringField ( 1 ),
        makerNonce:                 integerField ( 2 ),
        accountName:                stringField ( 3 ),
        keyName:                    stringField ( 4 ),
        key:                        stringField ( 5 ),
        amount:                     integerField ( 6 ),
        gratuity:                   integerField ( 7 ),
    },
    format: {
        maker:                      makerFormat (),
        accountName:                'accountName',
        amount:                     'amount',
        key:                        'key',
        keyName:                    'keyName',
    },
}

//----------------------------------------------------------------//
const registerMiner = {
    transactionType:    TRANSACTION_TYPE.REGISTER_MINER,
    friendlyName:       'Register Miner',
    fields: {
        makerAccountName:           stringField ( 0 ),
        makerKeyName:               stringField ( 1 ),
        makerNonce:                 integerField ( 2 ),
        url:                        stringField ( 3 ),
        gratuity:                   integerField ( 4 ),
    },
    format: {
        maker:                      makerFormat (),
        url:                        'url',
    },
}

//----------------------------------------------------------------//
const sendVOL = {
    transactionType:    TRANSACTION_TYPE.SEND_VOL,
    friendlyName:       'Send VOL',
    fields: {
        makerAccountName:           stringField ( 0 ),
        makerKeyName:               stringField ( 1 ),
        makerNonce:                 integerField ( 2 ),
        accountName:                stringField ( 3 ),
        amount:                     integerField ( 4 ),
        gratuity:                   integerField ( 5 ),
    },
    format: {
        maker:                      makerFormat (),
        accountName:                'accountName',
        amount:                     'amount',
    },
}

//================================================================//
// Transaction
//================================================================//
class Transaction {

    //----------------------------------------------------------------//
    constructor ( type, fieldValues ) {
        this.type = type;
        this.schema = schemaForType ( type );

        extendObservable ( this, {
            fieldValues:    fieldValues,
        });
    }

    //----------------------------------------------------------------//
    format ( format ) {

        format = format || this.schema.format;

        let result = {};
        for ( let fieldName in format ) {

            const fieldSource = format [ fieldName ];
            let fieldValue;

            if (( typeof fieldSource ) === 'object' ) {
                fieldValue = this.format ( fieldSource, this.fieldValues );
            }
            else {
                fieldValue = this.fieldValues [ fieldSource ];
            }
            result [ fieldName ] = fieldValue;
        }

        return result;
    }

    //----------------------------------------------------------------//
    getCost () {

        return this.schema.fields.gratuity && this.fieldValues.gratuity || 0;
    }
};

//================================================================//
// SendVol
//================================================================//
class SendVol extends Transaction {

    //----------------------------------------------------------------//
    getCost () {

        return super.getCost () + ( this.fieldValues.amount || 0 );
    }
};

//----------------------------------------------------------------//
export function makeTransaction ( type, fieldValues ) {

    switch ( type ) {
        case TRANSACTION_TYPE.ACCOUNT_POLICY:   return new Transaction ( type, fieldValues );
        case TRANSACTION_TYPE.AFFIRM_KEY:       return new Transaction ( type, fieldValues );
        case TRANSACTION_TYPE.KEY_POLICY:       return new Transaction ( type, fieldValues );
        case TRANSACTION_TYPE.OPEN_ACCOUNT:     return new Transaction ( type, fieldValues );
        case TRANSACTION_TYPE.REGISTER_MINER:   return new Transaction ( type, fieldValues );
        case TRANSACTION_TYPE.SEND_VOL:         return new SendVol ( type, fieldValues );
    }
}

//----------------------------------------------------------------//
export function schemaForType ( type ) {
    switch ( type ) {
        case TRANSACTION_TYPE.ACCOUNT_POLICY:   return accountPolicy;
        case TRANSACTION_TYPE.AFFIRM_KEY:       return affirmKey;
        case TRANSACTION_TYPE.KEY_POLICY:       return keyPolicy;
        case TRANSACTION_TYPE.OPEN_ACCOUNT:     return openAccount;
        case TRANSACTION_TYPE.REGISTER_MINER:   return registerMiner;
        case TRANSACTION_TYPE.SEND_VOL:         return sendVOL;
    }
}

//----------------------------------------------------------------//
export const transactionTypes = [
    TRANSACTION_TYPE.SEND_VOL,
    TRANSACTION_TYPE.ACCOUNT_POLICY,
    TRANSACTION_TYPE.KEY_POLICY,
    TRANSACTION_TYPE.OPEN_ACCOUNT,
    TRANSACTION_TYPE.REGISTER_MINER,
    TRANSACTION_TYPE.SEND_VOL,
];
