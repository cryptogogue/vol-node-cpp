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
        gratuity:           'gratuity',
        keyName:            'makerKeyName',
    };
}

//----------------------------------------------------------------//
const accountPolicy = {
    fields: {
        makerKeyName:               stringField ( 0 ),
        policyName:                 stringField ( 1 ),
        policy:                     stringField ( 2 ),
        gratuity:                   integerField ( 3 ),
    },
    format: {
        maker:                      makerFormat (),
        policy:                     'policy',
        policyName:                 'policyName',
    },
}

//----------------------------------------------------------------//
const affirmKey = {
    fields: {
        makerKeyName:               stringField ( 0 ),
        keyName:                    stringField ( 1 ),
        key:                        stringField ( 2 ),
        policyName:                 stringField ( 3 ),
        gratuity:                   integerField ( 4 ),
    },
    format: {
        maker:                      makerFormat (),
        key:                        'key',
        keyName:                    'keyName',
        policyName:                 'policyName',
    },
}

//----------------------------------------------------------------//
const betaGetAssets = {
    fields: {
        makerKeyName:               stringField ( 0 ),
        numAssets:                  integerField ( 1 ),
        gratuity:                   integerField ( 2 ),
    },
    format: {
        maker:                      makerFormat (),
        numAssets:                  'numAssets',
    },
}

//----------------------------------------------------------------//
const keyPolicy = {
    fields: {
        makerKeyName:               stringField ( 0 ),
        policyName:                 stringField ( 1 ),
        policy:                     stringField ( 2 ),
        gratuity:                   integerField ( 3 ),
    },
    format: {
        maker:                      makerFormat (),
        policy:                     'policy',
        policyName:                 'policyName',
    },
}

//----------------------------------------------------------------//
const openAccount = {
    fields: {
        makerKeyName:               stringField ( 0 ),
        accountName:                stringField ( 1 ),
        keyName:                    stringField ( 2 ),
        key:                        stringField ( 3 ),
        amount:                     integerField ( 4 ),
        gratuity:                   integerField ( 5 ),
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
    fields: {
        makerKeyName:               stringField ( 0 ),
        url:                        stringField ( 1 ),
        gratuity:                   integerField ( 2 ),
    },
    format: {
        maker:                      makerFormat (),
        url:                        'url',
    },
}

//----------------------------------------------------------------//
const runScript = {
    fields: {
        makerKeyName:               stringField ( 0 ),
        gratuity:                   integerField ( 2 ),
    },
    format: {
        maker:                      makerFormat (),
    },
}

//----------------------------------------------------------------//
const sendVOL = {
    fields: {
        makerKeyName:               stringField ( 0 ),
        accountName:                stringField ( 1 ),
        amount:                     integerField ( 2 ),
        gratuity:                   integerField ( 3 ),
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
        case TRANSACTION_TYPE.BETA_GET_ASSETS:  return betaGetAssets;
        case TRANSACTION_TYPE.KEY_POLICY:       return keyPolicy;
        case TRANSACTION_TYPE.OPEN_ACCOUNT:     return openSccount;
        case TRANSACTION_TYPE.REGISTER_MINER:   return registerMiner;
        case TRANSACTION_TYPE.RUN_SCRIPT:       return runScript;
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
        this.type = type;
        this.fields = template.fields;
        this.format = template.format;
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
