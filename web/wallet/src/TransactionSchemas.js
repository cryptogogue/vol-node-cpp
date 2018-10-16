/* eslint-disable no-whitespace-before-property */

//----------------------------------------------------------------//
const accountPolicy = {
    transactionType:    'ACCOUNT_POLICY',
    friendlyName:       'Account Policy',
    fields: {
        'makerAccountName': {
            hashOrder:      0,
            fieldType:      'STRING',
        },
        'makerKeyName': {
            hashOrder:      1,
            fieldType:      'STRING',
        },
        'makerNonce': {
            hashOrder:      2,
            fieldType:      'INTEGER',
        },
        'policyName': {
            hashOrder:      3,
            fieldType:      'STRING',
        },
        'policy': {
            hashOrder:      4,
            fieldType:      'STRING',
        },
        'gratuity': {
            hashOrder:      5,
            fieldType:      'INTEGER',
        },
    },
}

//----------------------------------------------------------------//
const affirmKey = {
    transactionType:    'AFFIRM_KEY',
    friendlyName:       'Affirm Key',
    fields: {
        'makerAccountName': {
            hashOrder:      0,
            fieldType:      'STRING',
        },
        'makerKeyName': {
            hashOrder:      1,
            fieldType:      'STRING',
        },
        'makerNonce': {
            hashOrder:      2,
            fieldType:      'INTEGER',
        },
        'keyName': {
            hashOrder:      3,
            fieldType:      'STRING',
        },
        'key': {
            hashOrder:      4,
            fieldType:      'STRING',
        },
        'keyPolicy': {
            hashOrder:      5,
            fieldType:      'STRING',
        },
        'gratuity': {
            hashOrder:      6,
            fieldType:      'INTEGER',
        },
    },
}

//----------------------------------------------------------------//
const keyPolicy = {
    transactionType:    'KEY_POLICY',
    friendlyName:       'Key Policy',
    fields: {
        'makerAccountName': {
            hashOrder:      0,
            fieldType:      'STRING',
        },
        'makerKeyName': {
            hashOrder:      1,
            fieldType:      'STRING',
        },
        'makerNonce': {
            hashOrder:      2,
            fieldType:      'INTEGER',
        },
        'policyName': {
            hashOrder:      3,
            fieldType:      'STRING',
        },
        'policy': {
            hashOrder:      4,
            fieldType:      'STRING',
        },
        'gratuity': {
            hashOrder:      5,
            fieldType:      'INTEGER',
        },
    },
}

//----------------------------------------------------------------//
const openAccount = {
    transactionType:    'OPEN_ACCOUNT',
    friendlyName:       'Open Account',
    fields: {
        'makerAccountName': {
            hashOrder:      0,
            fieldType:      'STRING',
        },
        'makerKeyName': {
            hashOrder:      1,
            fieldType:      'STRING',
        },
        'makerNonce': {
            hashOrder:      2,
            fieldType:      'INTEGER',
        },
        'accountName': {
            hashOrder:      3,
            fieldType:      'STRING',
        },
        'keyName': {
            hashOrder:      4,
            fieldType:      'STRING',
        },
        'key': {
            hashOrder:      5,
            fieldType:      'STRING',
        },
        'amount': {
            hashOrder:      6,
            fieldType:      'INTEGER',
        },
        'gratuity': {
            hashOrder:      7,
            fieldType:      'INTEGER',
        },
    },
}

//----------------------------------------------------------------//
const registerMiner = {
    transactionType:    'REGISTER_MINER',
    friendlyName:       'Register Miner',
    fields: {
        'makerAccountName': {
            hashOrder:      0,
            fieldType:      'STRING',
        },
        'makerKeyName': {
            hashOrder:      1,
            fieldType:      'STRING',
        },
        'makerNonce': {
            hashOrder:      2,
            fieldType:      'INTEGER',
        },
        'url': {
            hashOrder:      3,
            fieldType:      'STRING',
        },
        'gratuity': {
            hashOrder:      4,
            fieldType:      'INTEGER',
        },
    },
}

//----------------------------------------------------------------//
const sendVOL = {
    transactionType:    'SEND_VOL',
    friendlyName:       'Send VOL',
    fields: {
        'makerAccountName': {
            hashOrder:      0,
            fieldType:      'STRING',
        },
        'makerKeyName': {
            hashOrder:      1,
            fieldType:      'STRING',
        },
        'makerNonce': {
            hashOrder:      2,
            fieldType:      'INTEGER',
        },
        'recipientAccountName': {
            hashOrder:      3,
            fieldType:      'STRING',
        },
        'amount': {
            hashOrder:      4,
            fieldType:      'INTEGER',
        },
        'gratuity': {
            hashOrder:      5,
            fieldType:      'INTEGER',
        },
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
