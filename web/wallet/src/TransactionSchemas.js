/* eslint-disable no-whitespace-before-property */

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
const accountPolicy = {
    transactionType:    'ACCOUNT_POLICY',
    friendlyName:       'Account Policy',
    fields: {
        'makerAccountName':         stringField ( 0 ),
        'makerKeyName':             stringField ( 1 ),
        'makerNonce':               integerField ( 2 ),
        'policyName':               stringField ( 3 ),
        'policy':                   stringField ( 4 ),
        'gratuity':                 integerField ( 5 ),
    },
}

//----------------------------------------------------------------//
const affirmKey = {
    transactionType:    'AFFIRM_KEY',
    friendlyName:       'Affirm Key',
    fields: {
        'makerAccountName':         stringField ( 0 ),
        'makerKeyName':             stringField ( 1 ),
        'makerNonce':               integerField ( 2 ),
        'keyName':                  stringField ( 3 ),
        'key':                      stringField ( 4 ),
        'keyPolicy':                stringField ( 5 ),
        'gratuity':                 integerField ( 6 ),
    },
}

//----------------------------------------------------------------//
const keyPolicy = {
    transactionType:    'KEY_POLICY',
    friendlyName:       'Key Policy',
    fields: {
        'makerAccountName':         stringField ( 0 ),
        'makerKeyName':             stringField ( 1 ),
        'makerNonce':               integerField ( 2 ),
        'policyName':               stringField ( 3 ),
        'policy':                   stringField ( 4 ),
        'gratuity':                 integerField ( 5 ),
    },
}

//----------------------------------------------------------------//
const openAccount = {
    transactionType:    'OPEN_ACCOUNT',
    friendlyName:       'Open Account',
    fields: {
        'makerAccountName':         stringField ( 0 ),
        'makerKeyName':             stringField ( 1 ),
        'makerNonce':               integerField ( 2 ),
        'accountName':              stringField ( 3 ),
        'keyName':                  stringField ( 4 ),
        'key':                      stringField ( 5 ),
        'amount':                   integerField ( 6 ),
        'gratuity':                 integerField ( 7 ),
    },
}

//----------------------------------------------------------------//
const registerMiner = {
    transactionType:    'REGISTER_MINER',
    friendlyName:       'Register Miner',
    fields: {
        'makerAccountName':         stringField ( 0 ),
        'makerKeyName':             stringField ( 1 ),
        'makerNonce':               integerField ( 2 ),
        'url':                      stringField ( 3 ),
        'gratuity':                 integerField ( 4 ),
    },
}

//----------------------------------------------------------------//
const sendVOL = {
    transactionType:    'SEND_VOL',
    friendlyName:       'Send VOL',
    fields: {
        'makerAccountName':         stringField ( 0 ),
        'makerKeyName':             stringField ( 1 ),
        'makerNonce':               integerField ( 2 ),
        'recipientAccountName':     stringField ( 3 ),
        'amount':                   integerField ( 4 ),
        'gratuity':                 integerField ( 5 ),
    },
}

// TODO: this is a placeholder; transaction schemas should come from the server.
export const transactionSchemas = () => {
    return [
        sendVOL,
        accountPolicy,
        affirmKey,
        keyPolicy,
        openAccount,
        registerMiner,
    ];
}
