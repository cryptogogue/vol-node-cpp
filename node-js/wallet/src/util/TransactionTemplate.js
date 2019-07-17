/* eslint-disable no-whitespace-before-property */

import { Transaction, TRANSACTION_TYPE } from './Transaction';

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

//----------------------------------------------------------------//
function templateForType ( type ) {

    switch ( type ) {
        case TRANSACTION_TYPE.ACCOUNT_POLICY:   return accountPolicy;
        case TRANSACTION_TYPE.AFFIRM_KEY:       return affirmKey;
        case TRANSACTION_TYPE.KEY_POLICY:       return keyPolicy;
        case TRANSACTION_TYPE.OPEN_ACCOUNT:     return openSccount;
        case TRANSACTION_TYPE.REGISTER_MINER:   return registerMiner;
        case TRANSACTION_TYPE.SEND_VOL:         return sendVOL; 
    }
}

//================================================================//
// TransactionTemplate
//================================================================//
export class TransactionTemplate {

    //----------------------------------------------------------------//
    checkFieldValues ( fieldValues ) {

        for ( let fieldName in this.fields ) {
            let fieldValue = fieldValues [ fieldName ];
            if ( fieldValue === null ) {
                return false;
            }
        }
        return true;
    }

    //----------------------------------------------------------------//
    constructor ( type ) {

        const template = templateForType ( type );

        this.type           = type;
        this.friendlyName   = Transaction.friendlyNameForType ( type );
        this.fields         = template.fields;
        this.format         = template.format;
    }

    //----------------------------------------------------------------//
    formatBody ( fieldValues, format ) {
        
        format = format || this.format;

        let result = {};
        for ( let fieldName in format ) {

            const fieldSource = format [ fieldName ];
            let fieldValue;

            if (( typeof fieldSource ) === 'object' ) {
                fieldValue = this.formatBody ( fieldValues, fieldSource );
            }
            else {
                fieldValue = fieldValues [ fieldSource ];
            }
            result [ fieldName ] = fieldValue;
        }

        return result;
    }

    //----------------------------------------------------------------//
    makeTransaction ( fieldValues ) {

        return Transaction.transactionWithBody ( this.type, this.formatBody ( fieldValues ));
    }
}
