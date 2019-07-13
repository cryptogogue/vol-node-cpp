/* eslint-disable no-whitespace-before-property */

import { AppStateService }                                                      from './stores/AppStateService';
import { Service, useService }                                                  from './stores/Service';
import * as util                                                                from './utils/util';
import { action, computed, extendObservable, observable, observe }              from 'mobx';
import { observer }                                                             from 'mobx-react';
import React, { useState }                                                      from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Select, Segment }  from 'semantic-ui-react';

import * as transactions            from './transactions';

//================================================================//
// TransactionFormService
//================================================================//
class TransactionFormService extends Service {

    //----------------------------------------------------------------//
    checkFormInputs () {

        let cost = this.transaction.getCost ();
        if ( this.appState.balance < cost ) return false;

        for ( let fieldName in this.schema.fields ) {
            let fieldValue = this.transaction.fieldValues [ fieldName ];
            if ( fieldValue === null ) {
                return false;
            }
        }

        return true;
    }

    //----------------------------------------------------------------//
    constructor ( appState, transactionType ) {
        super ();

        const accountId = appState.accountId;

        this.appState = appState;
        this.schema = transactions.schemaForType ( transactionType );

        const defaultKeyName = appState.getDefaultAccountKeyName ( 'master' );

        let fieldValues = {
            makerAccountName: accountId,
            makerKeyName: defaultKeyName,
            makerNonce: -1,
        };

        Object.keys ( this.schema.fields ).forEach (( name ) => {
            fieldValues [ name ] = fieldValues [ name ] || null;
        });

        let transaction = transactions.makeTransaction ( transactionType, fieldValues );

        extendObservable ( this, { transaction: transaction });
    }

    //----------------------------------------------------------------//
    @action
    handleChange ( event ) {
        
        let typedValue = null;
        const value = event.target.value;

        if ( value && ( value.length > 0 )) {
            const type = event.target.type;
            typedValue = ( type === 'number' ) ? Number ( value ) : String ( value );
        }
        this.transaction.fieldValues [ event.target.name ] = typedValue;

        this.appState.setNextTransactionCost ( this.transaction.getCost ());
    }

    //----------------------------------------------------------------//
    makeFormInputForField ( name, field ) {

        let onChange = ( event ) => { this.handleChange ( event )};

        if ( name === 'makerAccountName' ) return;
        if ( name === 'makerNonce' ) return;

        let value = this.transaction.fieldValues [ name ] === null ? '' : this.transaction.fieldValues [ name ];

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
    Object.keys ( service.schema.fields ).forEach (( name ) => {
        const field = service.schema.fields [ name ];
        orderedFields [ field.hashOrder ] = name;
    });

    // add the fields in order
    let fields = [];
    orderedFields.forEach (( name ) => {
        let formInput = service.makeFormInputForField ( name, service.schema.fields [ name ]);
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
