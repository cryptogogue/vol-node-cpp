/* eslint-disable no-whitespace-before-property */

import { AppStateService }                                                      from './stores/AppStateService';
import { Service, useService }                                                  from './stores/Service';
import { Transaction }                                                          from './util/Transaction';
import { TransactionTemplate }                                                  from './util/TransactionTemplate';
import * as util                                                                from './util/util';
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
        this.orderedFields      = []; // order fields using hashOrder

        let fieldValues = {
            makerKeyName:       appState.getDefaultAccountKeyName (),
        };

        for ( let fieldName in this.template.fields ) {

            const field = this.template.fields [ fieldName ];
            this.orderedFields [ field.hashOrder ] = fieldName;
            fieldValues [ fieldName ] = fieldValues [ fieldName ] || null; // populate missing fields with null
        }

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
    isUserEditableField ( name ) {

        if ( name === 'makerAccountName' ) return false;
        if ( name === 'makerNonce' ) return false;
    }

    //----------------------------------------------------------------//
    makeFormInputForField ( name ) {

        let onChange = ( event ) => { this.handleChange ( event.target.name, event.target.type, event.target.value )};

        let value = this.fieldValues [ name ] !== null ? this.fieldValues [ name ] : '';

        if ( name === 'makerKeyName' ) {

            const accountKeyNames = this.appState.accountKeyNames;

            const select = [];
            for ( let i in accountKeyNames ) {
                const keyName = accountKeyNames [ i ];
                select.push ({ key: i, text: keyName, value: keyName });
            }

            return (
                <Form.Input
                    fluid
                    key = { name }
                    control = { Select }
                    options = { select }
                    placeholder = 'Account Key'
                    name = { name }
                    value = { value }
                    onChange = { onChange }
                />
            );
        }
        
        let field = this.template.fields [ name ];
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

    // add the fields in order
    let fields = [];
    for ( let i in service.orderedFields ) {
        fields.push ( service.makeFormInputForField ( service.orderedFields [ i ]));
    }

    const isSubmitEnabled = service.checkFormInputs ();

    let onClickSend = () => {
        onSubmit ( service.transaction );
    }

    return (
        <Form size = "large">
            { fields }
            <Button type = 'button' color = "teal" fluid disabled = { !isSubmitEnabled } onClick = { onClickSend }>
                Send
            </Button>
        </Form>
    );
});

export default TransactionForm;
