/* eslint-disable no-whitespace-before-property */

import { Service, useService }              from './Service';
import { Transaction, TRANSACTION_TYPE }    from './Transaction';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Divider, Dropdown, Form, Icon, Modal, Segment, Select } from 'semantic-ui-react';

//----------------------------------------------------------------//
function integerField ( fieldName, friendlyName ) {
    return {
        fieldType:      'INTEGER',
        name:           fieldName,
        friendlyName:   friendlyName,
    };
}

//----------------------------------------------------------------//
function stringField ( fieldName, friendlyName ) {
    return {
        fieldType:      'STRING',
        name:           fieldName,
        friendlyName:   friendlyName,
    };
}

//----------------------------------------------------------------//
function textField ( fieldName, friendlyName, rows ) {
    return {
        fieldType:      'TEXT',
        name:           fieldName,
        friendlyName:   friendlyName,
        rows:           rows,
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
    type:                           TRANSACTION_TYPE.ACCOUNT_POLICY,
    fields: [
        stringField     ( 'makerKeyName',   'Signing Key Name' ),
        stringField     ( 'policyName',     'Policy Name' ),
        stringField     ( 'policy',         'Policy' ),
        integerField    ( 'gratuity',       'Gratuity' ),
    ],
    format: {
        maker:                      makerFormat (),
        policy:                     'policy',
        policyName:                 'policyName',
    },
}

//----------------------------------------------------------------//
const affirmKey = {
    type:                           TRANSACTION_TYPE.AFFIRM_KEY,
    fields: [
        stringField     ( 'makerKeyName',   'Signing Key Name' ),
        stringField     ( 'keyName',        'Key Name' ),
        stringField     ( 'key',            'Key' ),
        stringField     ( 'policyName',     'Policy' ),
        integerField    ( 'gratuity',       'Gratuity' ),
    ],
    format: {
        maker:                      makerFormat (),
        key:                        'key',
        keyName:                    'keyName',
        policyName:                 'policyName',
    },
}

//----------------------------------------------------------------//
const betaGetAssets = {
    type:                           TRANSACTION_TYPE.BETA_GET_ASSETS,
    fields: [
        stringField     ( 'makerKeyName',   'Signing Key Name' ),
        integerField    ( 'numAssets',      'Copies' ),
        integerField    ( 'gratuity',       'Gratuity' ),
    ],
    format: {
        maker:                      makerFormat (),
        numAssets:                  'numAssets',
    },
}

//----------------------------------------------------------------//
const keyPolicy = {
    type:                           TRANSACTION_TYPE.KEY_POLICY,
    fields: [
        stringField     ( 'makerKeyName',   'Signing Key Name' ),
        stringField     ( 'policyName',     'Policy Name' ),
        stringField     ( 'policy',         'Policy' ),
        integerField    ( 'gratuity',       'Gratuity' ),
    ],
    format: {
        maker:                      makerFormat (),
        policy:                     'policy',
        policyName:                 'policyName',
    },
}

//----------------------------------------------------------------//
const openAccount = {
    type:                           TRANSACTION_TYPE.OPEN_ACCOUNT,
    fields: [
        stringField     ( 'makerKeyName',   'Signing Key Name' ),
        textField       ( 'request',        'New Account Request', 8 ),
        integerField    ( 'grant',          'Grant' ),
        integerField    ( 'gratuity',       'Gratuity' ),
    ],
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
    type:                           TRANSACTION_TYPE.REGISTER_MINER,
    fields: [
        stringField     ( 'makerKeyName',   'Signing Key Name' ),
        stringField     ( 'url',            'Miner URL' ),
        integerField    ( 'gratuity',       'Gratuity' ),
    ],
    format: {
        maker:                      makerFormat (),
        url:                        'url',
    },
}

//----------------------------------------------------------------//
const sendVOL = {
    type:                           TRANSACTION_TYPE.SEND_VOL,
    fields: [
        stringField     ( 'makerKeyName',   'Signing Key Name' ),
        stringField     ( 'accountName',    'Recipient' ),
        integerField    ( 'amount',         'Amount' ),
        integerField    ( 'gratuity',       'Gratuity' ),
    ],
    format: {
        maker:                      makerFormat (),
        accountName:                'accountName',
        amount:                     'amount',
    },
}

//================================================================//
// TransactionFormController
//================================================================//
class TransactionFormController extends Service {

    //----------------------------------------------------------------//
    checkFormInputs () {

        const cost = this.transaction.getCost ();
        if ( this.appState.balance < cost ) return false;

        for ( let field of this.fields ) {
            const fieldValue = this.fieldValues [ field.name ];
            if ( fieldValue === null ) {
                return false;
            }
        }
        return true;
    }

    //----------------------------------------------------------------//
    constructor ( appState, template ) {
        super ();

        this.appState   = appState;

        this.type       = template.type;
        this.fields     = template.fields;
        this.format     = template.format;

        let fieldValues = {
            makerKeyName:       appState.getDefaultAccountKeyName (),
        };

        // populate fields with null
        for ( let field of this.fields ) {
            fieldValues [ field.name ] = fieldValues [ field.name ] || null; 
        }

        extendObservable ( this, {
            fieldValues:    fieldValues,
        });

        this.transaction = this.makeTransaction ();
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
}

//================================================================//
// TransactionFormInput
//================================================================//
const TransactionFormInput = observer (( props ) => {

    const { appState, field, value, onChange } = props;

    const name = field.name;

    if ( name === 'makerKeyName' ) {

        const accountKeyNames = appState.accountKeyNames;

        const select = [];
        for ( let i in accountKeyNames ) {
            const keyName = accountKeyNames [ i ];
            select.push ({ key: i, text: keyName, value: keyName });
        }

        return (
            <Form.Input
                fluid
                control = { Select }
                options = { select }
                placeholder = 'Account Key'
                name = { name }
                value = { value }
                onChange = { onChange }
            />
        );
    }

    switch ( field.fieldType ) {

        case 'INTEGER':
            return (
                 <Form.Input
                    fluid
                    placeholder = { field.friendlyName }
                    name = { name }
                    type = 'number'
                    value = { value }
                    onChange = { onChange }
                />
            );

        case 'STRING':
            return (
                 <Form.Input
                    fluid
                    placeholder = { field.friendlyName }
                    name = { name }
                    type = 'string'
                    value = { value }
                    onChange = { onChange }
                />
            );

        case 'TEXT':
            return (
                 <Form.TextArea
                    rows = { field.rows || 8 }
                    placeholder = { field.friendlyName }
                    name = { name }
                    value = { value }
                    onChange = { onChange }
                />
            );
    }

    return <div/>;
});

//================================================================//
// GenericTransactionForm
//================================================================//
const GenericTransactionForm = observer (( props ) => {

    const { appState, template, onSubmit } = props;

    const controller = useService (() => new TransactionFormController ( appState, template ));

    // add the fields in order
    let fields = [];
    for ( let field of template.fields ) {

        const name = field.name;
        const fieldValues = controller.fieldValues;
        const value = fieldValues [ name ] !== null ? fieldValues [ name ] : '';

        const onChange = ( event ) => { controller.handleChange ( event.target.name, event.target.type, event.target.value )};

        fields.push (<TransactionFormInput
            key         = { name }
            appState    = { appState }
            field       = { field }
            value       = { value }
            onChange    = { onChange }
        />);
    }

    const isSubmitEnabled = controller.checkFormInputs ();

    const onClick = () => {
        onSubmit ( controller.transaction );
    }

    return (
        <Form size = "large">
            { fields }
            <Button type = 'button' color = "teal" fluid disabled = { !isSubmitEnabled } onClick = { onClick }>
                Stage Transaction
            </Button>
        </Form>
    );
});

//================================================================//
// TransactionForm
//================================================================//
export const TransactionForm = observer (( props ) => {

    const { appState, transactionType, onSubmit } = props;

    const pass = {
        appState:       appState,
        onSubmit:       onSubmit,
    };

    switch ( transactionType ) {
        case TRANSACTION_TYPE.ACCOUNT_POLICY:   return <GenericTransactionForm { ...pass }  template = { accountPolicy }/>;
        case TRANSACTION_TYPE.AFFIRM_KEY:       return <GenericTransactionForm { ...pass }  template = { affirmKey }/>;
        case TRANSACTION_TYPE.BETA_GET_ASSETS:  return <GenericTransactionForm { ...pass }  template = { betaGetAssets }/>;
        case TRANSACTION_TYPE.KEY_POLICY:       return <GenericTransactionForm { ...pass }  template = { keyPolicy }/>;
        case TRANSACTION_TYPE.OPEN_ACCOUNT:     return <GenericTransactionForm { ...pass }  template = { openAccount }/>;
        case TRANSACTION_TYPE.REGISTER_MINER:   return <GenericTransactionForm { ...pass }  template = { registerMiner }/>;
        case TRANSACTION_TYPE.SEND_VOL:         return <GenericTransactionForm { ...pass }  template = { sendVOL }/>;
    }
    return <div/>
});
