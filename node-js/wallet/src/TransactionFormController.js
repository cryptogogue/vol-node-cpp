/* eslint-disable no-whitespace-before-property */

import { Service, useService }              from './Service';
import { Transaction, TRANSACTION_TYPE }    from './Transaction';
import * as inputType                       from './TransactionFormInputTypes';
import _                                    from 'lodash';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Divider, Dropdown, Form, Icon, Modal, Segment, Select } from 'semantic-ui-react';

//----------------------------------------------------------------//
function makerFormat () {
    return {
        gratuity:           'gratuity',
        accountName:        'makerAccountName',
        keyName:            'makerKeyName',
        nonce:              'makerNonce',
    };
}

//================================================================//
// TransactionFormController
//================================================================//
export class TransactionFormController extends Service {

    //----------------------------------------------------------------//
    constructor () {
        super ();
    }

    //----------------------------------------------------------------//
    formatBody ( format ) {
        
        format = format || this.format;
        const fieldValues = this.fieldValues;

        let result = {};
        for ( let fieldName in format ) {

            const fieldSource = format [ fieldName ];
            let fieldValue;

            if (( typeof fieldSource ) === 'object' ) {
                fieldValue = this.formatBody ( fieldSource );
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
    handleChange ( fieldName, type, value ) {
        
        let typedValue = null;

        if ( value && ( value.length > 0 )) {
            typedValue = ( type === 'number' ) ? Number ( value ) : String ( value );
        }
        this.fieldValues [ fieldName ] = typedValue;
        this.transaction = this.makeTransaction ( this.fieldValues );

        this.appState.setNextTransactionCost ( this.transaction.getCost ());

        this.validate ();
    }

    //----------------------------------------------------------------//
    initialize ( appState, type, fields, format ) {

        this.appState   = appState;
        this.type       = type;
        this.fields     = fields;
        this.format     = format;

        let fieldValues = {};
        // populate fields with null
        for ( let field of this.fields ) {
            fieldValues [ field.name ] = null;
        }

        fieldValues.makerKeyName        = appState.getDefaultAccountKeyName (),
        fieldValues.makerAccountName    = appState.accountId;
        fieldValues.makerNonce          = -1;

        extendObservable ( this, {
            fieldValues:    fieldValues,
            fieldErrors:    {},
            isComplete:     false,
        });

        this.transaction = this.makeTransaction ();
    }

    //----------------------------------------------------------------//
    @computed
    get isCompleteAndErrorFree () {

        return this.isComplete && ( Object.keys ( this.fieldErrors ).length === 0 );
    }

    //----------------------------------------------------------------//
    isUserEditableField ( name ) {

        if ( name === 'makerAccountName' ) return false;
        if ( name === 'makerNonce' ) return false;
    }

    //----------------------------------------------------------------//
    makeTransaction () {

        return Transaction.transactionWithBody ( this.type, this.formatBody ());
    }

    //----------------------------------------------------------------//
    @action
    validate () {

        console.log ( 'VALIDATE!' );

        this.fieldErrors = {};
        this.isComplete = false;

        const cost = this.transaction.getCost ();
        if ( this.appState.balance < cost ) return false;

        for ( let field of this.fields ) {
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
            inputType.stringField       ( 'makerKeyName',   'Signing Key Name' ),
            inputType.stringField       ( 'policyName',     'Policy Name' ),
            inputType.stringField       ( 'policy',         'Policy' ),
            inputType.integerField      ( 'gratuity',       'Gratuity' ),
        ];

        const format = {
            maker:              makerFormat (),
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
            inputType.stringField       ( 'makerKeyName',   'Signing Key Name' ),
            inputType.stringField       ( 'keyName',        'Key Name' ),
            inputType.stringField       ( 'key',            'Key' ),
            inputType.stringField       ( 'policyName',     'Policy' ),
            inputType.integerField      ( 'gratuity',       'Gratuity' ),
        ];

        const format = {
            maker:              makerFormat (),
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
            inputType.stringField       ( 'makerKeyName',   'Signing Key Name' ),
            inputType.integerField      ( 'numAssets',      'Copies' ),
            inputType.integerField      ( 'gratuity',       'Gratuity' ),
        ];

        const format = {
            maker:              makerFormat (),
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
            inputType.stringField       ( 'makerKeyName',   'Signing Key Name' ),
            inputType.stringField       ( 'policyName',     'Policy Name' ),
            inputType.stringField       ( 'policy',         'Policy' ),
            inputType.integerField      ( 'gratuity',       'Gratuity' ),
        ];

        const format = {
            maker:              makerFormat (),
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
    constructor ( appState ) {
        super ();

        const type = TRANSACTION_TYPE.OPEN_ACCOUNT;

        const fields = [
            inputType.stringField       ( 'makerKeyName',   'Signing Key Name' ),
            inputType.textField         ( 'request',        'New Account Request', 8 ),
            inputType.integerField      ( 'grant',          'Grant' ),
            inputType.integerField      ( 'gratuity',       'Gratuity' ),
        ];

        const format = {
            maker:              makerFormat (),
            accountName:        'accountName',
            amount:             'amount',
            key:                'key',
            keyName:            'keyName',
        };

        this.initialize ( appState, type, fields, format );
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
            inputType.stringField       ( 'makerKeyName',   'Signing Key Name' ),
            inputType.stringField       ( 'url',            'Miner URL' ),
            inputType.integerField      ( 'gratuity',       'Gratuity' ),
        ];

        const format = {
            maker:              makerFormat (),
            url:                'url',
        };

        this.initialize ( appState, type, fields, format );
    }
}

//================================================================//
// TransactionFormController_SendVol
//================================================================//
export class TransactionFormController_SendVol extends TransactionFormController {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        const type = TRANSACTION_TYPE.SEND_VOL;

        const fields = [
            inputType.stringField       ( 'makerKeyName',   'Signing Key Name' ),
            inputType.stringField       ( 'recipient',      'Recipient' ),
            inputType.integerField      ( 'amount',         'Amount' ),
            inputType.integerField      ( 'gratuity',       'Gratuity' ),
        ];

        const format = {
            maker:              makerFormat (),
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
// factory
//================================================================//
export function makeControllerForTransactionType ( appState, transactionType ) {

    switch ( transactionType ) {
        case TRANSACTION_TYPE.ACCOUNT_POLICY:   return new TransactionFormController_AccountPolicy ( appState );
        case TRANSACTION_TYPE.AFFIRM_KEY:       return new TransactionFormController_AffirmKey ( appState );
        case TRANSACTION_TYPE.BETA_GET_ASSETS:  return new TransactionFormController_BetaGetAssets ( appState );
        case TRANSACTION_TYPE.KEY_POLICY:       return new TransactionFormController_KeyPolicy ( appState );
        case TRANSACTION_TYPE.OPEN_ACCOUNT:     return new TransactionFormController_OpenAccount ( appState );
        case TRANSACTION_TYPE.REGISTER_MINER:   return new TransactionFormController_RegisterMiner ( appState );
        case TRANSACTION_TYPE.SEND_VOL:         return new TransactionFormController_SendVol ( appState );
    }
    return new TransactionFormController ( appState );
}
