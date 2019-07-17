/* eslint-disable no-whitespace-before-property */

import { AppStateService }                                                      from './stores/AppStateService';
import { Service, useService }                                                  from './stores/Service';
import { Transaction, TRANSACTION_TYPE }                                        from './utils/Transaction';
import { TransactionTemplate }                                                  from './utils/TransactionTemplate';
import * as util                                                                from './utils/util';
import { action, computed, extendObservable, observable, observe }              from 'mobx';
import { observer }                                                             from 'mobx-react';
import React, { useState }                                                      from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Select, Segment }  from 'semantic-ui-react';

//================================================================//
// TransactionFormService
//================================================================//
class TransactionFormService extends Service {

    //----------------------------------------------------------------//
    checkFormInputs () {

        let cost = this.transaction.getCost ();
        if ( this.appState.balance < cost ) return false;

        return this.template.checkFieldValues ( this.fieldValues );
    }

    //----------------------------------------------------------------//
    constructor ( appState, transactionType ) {
        super ();

        this.appState           = appState;
        this.transactioNType    = transactionType;
        this.template           = new TransactionTemplate ( transactionType );

        let fieldValues = {
            makerAccountName:   appState.accountId,
            makerKeyName:       appState.getDefaultAccountKeyName ( 'master' ),
            makerNonce:         -1,
        };

        Object.keys ( this.template.fields ).forEach (( name ) => {
            fieldValues [ name ] = fieldValues [ name ] || null;
        });

        let transaction = this.template.makeTransaction ( fieldValues );

        extendObservable ( this, {
            fieldValues:    fieldValues,
            transaction:    transaction,
        });
    }

    //----------------------------------------------------------------//
    @action
    handleChange ( fieldName, type, value ) {
        
        let typedValue = null;

        if ( value && ( value.length > 0 )) {
            typedValue = ( type === 'number' ) ? Number ( value ) : String ( value );
        }
        this.fieldValues [ fieldName ] = typedValue;
        this.transaction = this.template.makeTransaction ( this.fieldValues );

        this.appState.setNextTransactionCost ( this.transaction.getCost ());
    }

    //----------------------------------------------------------------//
    makeFormInputForField ( name, field ) {

        let onChange = ( event ) => { this.handleChange ( event.target.name, event.target.type, event.target.value )};

        if ( name === 'makerAccountName' ) return;
        if ( name === 'makerNonce' ) return;

        let value = this.fieldValues [ name ] === null ? '' : this.fieldValues [ name ];

        if ( name === 'makerKeyName' ) {

            const accountKeyNames = this.appState.accountKeyNames;

            //if ( accountKeyNames.length <= 1 ) return;

            const select = [];
            for ( let i in accountKeyNames ) {
                const keyName = accountKeyNames [ i ];
                select.push ({ key: i, text: keyName, value: keyName });
            }

            return (

                <div key = { name }>
                    <Form.Input
                        fluid
                        control = { Select }
                        options = { select }
                        placeholder = 'Account Key'
                        name = { name }
                        value = { value }
                        onChange = { onChange }
                    />
                </div>
            );
        }
        
        let inputType = field.fieldType === 'INTEGER' ? 'number' : 'string';

        return (
            <Form.Input
                fluid
                key = { name }
                placeholder = { name }
                name = { name }
                type = { inputType }
                value = { value }
                onChange = { onChange }
            />
        );
    }
}

//================================================================//
// TransactionForm
//================================================================//
const TransactionForm = observer (( props ) => {

    const { appState, transactionType, onSubmit } = props;

    const service = useService (() => new TransactionFormService ( appState, transactionType ));

    // order fields using hashOrder
    let orderedFields = [];
    Object.keys ( service.template.fields ).forEach (( name ) => {
        const field = service.template.fields [ name ];
        orderedFields [ field.hashOrder ] = name;
    });

    // add the fields in order
    let fields = [];
    orderedFields.forEach (( name ) => {
        let formInput = service.makeFormInputForField ( name, service.template.fields [ name ]);
        if ( formInput ) {
            fields.push ( formInput );
        }
    });

    const isSubmitEnabled = service.checkFormInputs ();

    let onClickSend = () => {
        onSubmit ( service.transaction );
    }

    return (
        <Form size = "large">
            <Segment stacked>
                { fields }
                <Button type = 'button' color = "teal" fluid disabled = { !isSubmitEnabled } onClick = { onClickSend }>
                    Send
                </Button>
            </Segment>
        </Form>
    );
});

export default TransactionForm;
