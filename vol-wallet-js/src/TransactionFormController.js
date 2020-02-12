/* eslint-disable no-whitespace-before-property */

import { Transaction, TRANSACTION_TYPE }    from './Transaction';
import * as inputType                       from './TransactionFormInputTypes';
import { assert, excel, hooks, randomBytes, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import _                                    from 'lodash';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                         from 'mobx-react';

//----------------------------------------------------------------//
const MAKER_FORMAT = {
    gratuity:           'gratuity',
    accountName:        'makerAccountName',
    keyName:            'makerKeyName',
    nonce:              'makerNonce',
}

//================================================================//
// TransactionFormController
//================================================================//
export class TransactionFormController {

    @observable     cost = 0;

    //----------------------------------------------------------------//
    @computed get
    balance () {

        return this.appState.balance - this.cost;
    }

    //----------------------------------------------------------------//
    constructor () {
    }

    //----------------------------------------------------------------//
    composeBody ( fieldValues ) {

        return this.formatBody ( fieldValues, this.format );
    }

    //----------------------------------------------------------------//
    finalize () {
    }

    //----------------------------------------------------------------//
    formatBody ( fieldValues, format ) {
        
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
    @action
    handleChange ( field, type, value ) {
        
        let typedValue = null;

        if ( value && ( value.length > 0 )) {
            typedValue = ( type === 'number' ) ? Number ( value ) : String ( value );
        }
        this.fieldValues [ field.name ] = typedValue;
        this.transaction = this.makeTransaction ();

        this.cost = this.transaction.getCost ();

        this.validate ();
    }

    //----------------------------------------------------------------//
    initialize ( appState, type, fields, format, fieldValues ) {

        fields = fields || [];
        fields.push (
            inputType.integerField      ( 'gratuity',       'Gratuity', 0 ),
        );

        this.appState   = appState;
        this.type       = type;
        this.fields     = fields;
        this.format     = format;

        // populate fields with null
        fieldValues = fieldValues || {};
        for ( let field of this.fields ) {
            fieldValues [ field.name ] = null;
        }

        fieldValues.makerAccountName    = appState.accountID;
        fieldValues.makerKeyName        = appState.getDefaultAccountKeyName (),
        fieldValues.makerNonce          = -1;

        extendObservable ( this, {
            fieldValues:    fieldValues,
            fieldErrors:    {},
            isComplete:     false,
        });

        this.transaction = this.makeTransaction ();
        this.validate ();
    }

    //----------------------------------------------------------------//
    @computed
    get isCompleteAndErrorFree () {

        return this.isComplete && ( Object.keys ( this.fieldErrors ).length === 0 );
    }

    //----------------------------------------------------------------//
    @computed get
    key () {

        return this.appState.getKey ( this.fieldValues.makerKeyName );
    }

    //----------------------------------------------------------------//
    @computed get
    keyName () {

        return this.fieldValues.makerKeyName;
    }

    //----------------------------------------------------------------//
    @action
    makeTransaction () {

        const fieldValues = _.cloneDeep ( this.fieldValues );
        for ( let field of this.fields ) {
            let value = fieldValues [ field.name ];
            fieldValues [ field.name ] = ( value === null ) ? field.defaultValue : value;
        }
        return Transaction.transactionWithBody ( this.type, this.composeBody ( fieldValues ));
    }

    //----------------------------------------------------------------//
    @action
    setKeyName ( keyName ) {
        
        return this.fieldValues.makerKeyName = keyName;
    }

    //----------------------------------------------------------------//
    @action
    validate () {

        this.fieldErrors = {};
        this.isComplete = false;

        const cost = this.transaction.getCost ();
        if ( this.appState.balance < cost ) return false;

        for ( let field of this.fields ) {

            if ( field.isRequired === false ) continue;
            
            const fieldValue = this.fieldValues [ field.name ];
            if ( fieldValue === null ) {
                return;
            }
        }
        this.isComplete = true;
    }
}

//================================================================//
// TransactionFormController_AccountPolicy
//================================================================//
export class TransactionFormController_AccountPolicy extends TransactionFormController {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        const type = TRANSACTION_TYPE.ACCOUNT_POLICY;

        const fields = [
            inputType.stringField       ( 'policyName',     'Policy Name' ),
            inputType.textField         ( 'policy',         'Policy', 8 ),
        ];

        const format = {
            maker:              MAKER_FORMAT,
            policy:             'policy',
            policyName:         'policyName',
        };

        this.initialize ( appState, type, fields, format );
    }
}

//================================================================//
// TransactionFormController_AffirmKey
//================================================================//
export class TransactionFormController_AffirmKey extends TransactionFormController {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        const type = TRANSACTION_TYPE.AFFIRM_KEY;

        const fields = [
            inputType.stringField       ( 'keyName',        'Key Name' ),
            inputType.stringField       ( 'key',            'Key' ),
            inputType.stringField       ( 'policyName',     'Policy' ),
        ];

        const format = {
            maker:              MAKER_FORMAT,
            key:                'key',
            keyName:            'keyName',
            policyName:         'policyName',
        };

        this.initialize ( appState, type, fields, format );
    }
}

//================================================================//
// TransactionFormController_BetaGetAssets
//================================================================//
export class TransactionFormController_BetaGetAssets extends TransactionFormController {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        const type = TRANSACTION_TYPE.BETA_GET_ASSETS;

        const fields = [
            inputType.integerField      ( 'numAssets',      'Copies', 1 ),
        ];

        const format = {
            maker:              MAKER_FORMAT,
            numAssets:          'numAssets',
        };

        this.initialize ( appState, type, fields, format );
    }
}

//================================================================//
// TransactionFormController_KeyPolicy
//================================================================//
export class TransactionFormController_KeyPolicy extends TransactionFormController {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        const type = TRANSACTION_TYPE.KEY_POLICY;

        const fields = [
            inputType.stringField       ( 'policyName',     'Policy Name' ),
            inputType.textField         ( 'policy',         'Policy', 8 ),
        ];

        const format = {
            maker:              MAKER_FORMAT,
            policy:             'policy',
            policyName:         'policyName',
        };

        this.initialize ( appState, type, fields, format );
    }
}

//================================================================//
// TransactionFormController_OpenAccount
//================================================================//
export class TransactionFormController_OpenAccount extends TransactionFormController {

    //----------------------------------------------------------------//
    composeBody ( fieldValues ) {

        const request       = this.decodeRequest ();

        let body = {};
        body.maker          = this.formatBody ( fieldValues, MAKER_FORMAT );
        body.suffix         = fieldValues.suffix || '';
        body.key            = request && request.key || false;
        body.grant          = fieldValues.grant || 0;

        return body;
    }

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        const type = TRANSACTION_TYPE.OPEN_ACCOUNT;

        const fields = [
            inputType.cryptoField       ( 'request',        'New Account Request', 6 ),
            inputType.integerField      ( 'grant',          'Grant', 0 ),
        ];

        const format = {
            maker:              MAKER_FORMAT,
            accountName:        'accountName',
            amount:             'amount',
            key:                'key',
            keyName:            'keyName',
        };

        this.initialize ( appState, type, fields, format );
        this.initSuffix ();
    }

    //----------------------------------------------------------------//
    decodeRequest () {

        console.log ( 'DECODE REQUEST', this.fieldValues.request );

        let encoded = this.fieldValues.request;
        if ( encoded && encoded.length ) {
            try {

                encoded = encoded.replace ( /(\r\n|\n|\r )/gm, '' );
                console.log ( 'ENCODED:', encoded );

                const requestJSON = Buffer.from ( encoded, 'base64' ).toString ( 'utf8' );
                const request = JSON.parse ( requestJSON );

                if ( !request ) return false;
                if ( !request.key ) return false;
                if ( request.networkID !== this.appState.network.identity ) return false;

                console.log ( 'DECODED REQUEST:', request );

                // TODO: check key format and validity!

                return request;
            }
            catch ( error ) {
                console.log ( error );
            }
        }
        return false;
    }

    //----------------------------------------------------------------//
    @action
    initSuffix () {

        // TODO: replace with something deterministic        
        const suffixPart = () => {
            return randomBytes ( 2 ).toString ( 'hex' ).substring ( 0, 3 );
        }
        this.fieldValues.suffix = `${ suffixPart ()}.${ suffixPart ()}.${ suffixPart()}`.toUpperCase ();
        console.log ( 'SUFFIX:', this.fieldValues.suffix );
    }

    //----------------------------------------------------------------//
    @action
    validate () {
        super.validate ();

        const fieldValues = this.fieldValues;
        const fieldErrors = this.fieldErrors;

        const encoded = fieldValues.request || '';

        if ( encoded.length > 0 ) {
            const request = this.decodeRequest ();
            if ( !request ) {
                fieldErrors.request = 'Problem decoding request.';
            }
        }
    }
}

//================================================================//
// TransactionFormController_PublishSchema
//================================================================//
export class TransactionFormController_PublishSchema extends TransactionFormController {

    //----------------------------------------------------------------//
    composeBody ( fieldValues ) {

        let body = {};
        body.maker          = this.formatBody ( fieldValues, MAKER_FORMAT );
        body.schema         = JSON.parse ( this.fieldValues.schema );

        return body;
    }

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        const type = TRANSACTION_TYPE.PUBLISH_SCHEMA;

        const fields = [
            inputType.textField         ( 'schema',         'Schema', 8 ),
        ];

        const format = {
            maker:              MAKER_FORMAT,
            schema:             'schema',
        };

        this.initialize ( appState, type, fields, format );
    }

    //----------------------------------------------------------------//
    @action
    validate () {
        super.validate ();

        const fieldValues = this.fieldValues;
        const fieldErrors = this.fieldErrors;

        try {
            this.schema = JSON.parse ( this.fieldValues.schema );
        }
        catch ( error ) {
            fieldErrors.schema = 'Error parsing JSON.';
        }
    }
}

//================================================================//
// TransactionFormController_RegisterMiner
//================================================================//
export class TransactionFormController_RegisterMiner extends TransactionFormController {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        const type = TRANSACTION_TYPE.REGISTER_MINER;

        const fields = [
            inputType.stringField       ( 'url',            'Miner URL' ),
        ];

        const format = {
            maker:              MAKER_FORMAT,
            url:                'url',
        };

        this.initialize ( appState, type, fields, format );
    }
}

//================================================================//
// TransactionFormController_RenameAccount
//================================================================//
export class TransactionFormController_RenameAccount extends TransactionFormController {

    //----------------------------------------------------------------//
    composeBody ( fieldValues ) {

        const makerAccountName = fieldValues.makerAccountName;
        const secretName = fieldValues.secretName || '';

        let body = {};
        body.maker          = this.formatBody ( fieldValues, MAKER_FORMAT );
        body.revealedName   = fieldValues.revealedName || '';

        if ( secretName.length ) {
            body.nameHash       = sha256 ( secretName );
            body.nameSecret     = sha256 ( `${ makerAccountName }:${ secretName }` );
        }

        return body;
    }

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        const type = TRANSACTION_TYPE.RENAME_ACCOUNT;

        const fields = [
            inputType.stringField       ( 'revealedName',   'New Name' ),
        ];

        // const fields = [
        //     inputType.stringField       ( 'revealedName',   'Revealed Name' ),
        //     inputType.stringField       ( 'secretName',     'Secret Name' ),
        // ];

        this.initialize ( appState, type, fields );
    }

    //----------------------------------------------------------------//
    @action
    validate () {
        
        const fieldValues = this.fieldValues;

        const revealedName = fieldValues.revealedName;
        const secretName = fieldValues.secretName;

        this.isComplete = (
            ( revealedName && ( revealedName.length > 0 )) ||
            ( secretName && ( secretName.length > 0 ))
        );

        this.fieldErrors = {};

        const fieldErrors = this.fieldErrors;

        if ( fieldValues.makerAccountName === revealedName ) {
            fieldErrors.revealedName = 'Revealed name should be different from current account name.';
        }

        if ( secretName && ( secretName.length > 0 )) {
            if ( fieldValues.makerAccountName === secretName ) {
                fieldErrors.secretName = 'Secret name should be different from current account name.';
            }
            else if ( secretName === revealedName ) {
                fieldErrors.secretName = 'Secret name should be different from revealed name.';
            }
        }
    }
};

//================================================================//
// TransactionFormController_SendVol
//================================================================//
export class TransactionFormController_SendVol extends TransactionFormController {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        const type = TRANSACTION_TYPE.SEND_VOL;

        const fields = [
            inputType.stringField       ( 'recipient',      'Recipient' ),
            inputType.integerField      ( 'amount',         'Amount' ),
        ];

        const format = {
            maker:              MAKER_FORMAT,
            accountName:        'recipient',
            amount:             'amount',
        };

        this.initialize ( appState, type, fields, format );
    }

    //----------------------------------------------------------------//
    @action
    validate () {
        super.validate ();

        const fieldValues = this.fieldValues;
        const fieldErrors = this.fieldErrors;

        if ( fieldValues.makerAccountName === fieldValues.recipient ) {
            fieldErrors.recipient = 'Maker cannot also be recipient.';
        }

        if ( fieldValues.amount === 0 ) {
            fieldErrors.amount = 'Pick a non-zero amount.';
        }
    }
};

//================================================================//
// TransactionFormController_UpgradeAssets
//================================================================//
export class TransactionFormController_UpgradeAssets extends TransactionFormController {

    //----------------------------------------------------------------//
    composeBody ( fieldValues ) {

        const body = this.formatBody ( fieldValues, this.format );
        body.upgrades = _.clone ( this.fieldValues.upgrades );
        return body;
    }

    //----------------------------------------------------------------//
    constructor ( appState, upgradeMap ) {
        super ();

        const type = TRANSACTION_TYPE.UPGRADE_ASSETS;

        const format = {
            maker:              MAKER_FORMAT,
        };

        const fieldValues = {
            upgrades:           upgradeMap,
        }

        this.initialize ( appState, type, fields, format, fieldValues );
    }
}

//================================================================//
// factory
//================================================================//
export function makeControllerForTransactionType ( appState, transactionType ) {

    switch ( transactionType ) {
        case TRANSACTION_TYPE.ACCOUNT_POLICY:   return new TransactionFormController_AccountPolicy ( appState );
        case TRANSACTION_TYPE.AFFIRM_KEY:       return new TransactionFormController_AffirmKey ( appState );
        case TRANSACTION_TYPE.BETA_GET_ASSETS:  return new TransactionFormController_BetaGetAssets ( appState );
        case TRANSACTION_TYPE.KEY_POLICY:       return new TransactionFormController_KeyPolicy ( appState );
        case TRANSACTION_TYPE.OPEN_ACCOUNT:     return new TransactionFormController_OpenAccount ( appState );
        case TRANSACTION_TYPE.PUBLISH_SCHEMA:   return new TransactionFormController_PublishSchema ( appState );
        case TRANSACTION_TYPE.REGISTER_MINER:   return new TransactionFormController_RegisterMiner ( appState );
        case TRANSACTION_TYPE.RENAME_ACCOUNT:   return new TransactionFormController_RenameAccount ( appState );
        case TRANSACTION_TYPE.SEND_VOL:         return new TransactionFormController_SendVol ( appState );
    }
    return new TransactionFormController ( appState );
}
