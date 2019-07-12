/* eslint-disable no-whitespace-before-property */

import { AppStateStore }                                                        from './stores/AppStateStore';
import { Service }                                                              from './stores/Service';
import { Store, useStore }                                                      from './stores/Store';
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

        let isValid = true;

        Object.keys ( this.schema.fields ).forEach (( fieldName ) => {
            let fieldValue = this.fieldValues [ fieldName ];
            console.log ( 'FIELD', fieldName, fieldValue );
            if ( fieldValue === null ) {
                isValid = false;
            }
        });

        return isValid;
    }

    //----------------------------------------------------------------//
    constructor ( appState, schema ) {
        super ();

        const accountId = appState.accountId;

        this.appState = appState;
        this.schema = schema;

        const defaultKeyName = appState.getDefaultAccountKeyName ( 'master' );

        let fieldValues = {
            makerAccountName: accountId,
            makerKeyName: defaultKeyName,
            makerNonce: -1,
        };

        Object.keys ( schema.fields ).forEach (( name ) => {
            fieldValues [ name ] = fieldValues [ name ] || null;
        });

        extendObservable ( this, { fieldValues: fieldValues });
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
        this.fieldValues [ event.target.name ] = typedValue;
    }

    //----------------------------------------------------------------//
    makeFormInputForField ( name, field ) {

        let onChange = ( event ) => { this.handleChange ( event )};

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

    const { appState, schema } = props;

    const service = useStore (() => new TransactionFormService ( appState, schema ));

    // order fields using hashOrder
    let orderedFields = [];
    Object.keys ( schema.fields ).forEach (( name ) => {
        const field = schema.fields [ name ];
        orderedFields [ field.hashOrder ] = name;
    });

    // add the fields in order
    let fields = [];
    orderedFields.forEach (( name ) => {
        let formInput = service.makeFormInputForField ( name, schema.fields [ name ]);
        if ( formInput ) {
            fields.push ( formInput );
        }
    });

    const isSubmitEnabled = service.checkFormInputs ();

    let onClickSend = () => {
        this.props.onSubmit ( schema, Object.assign ({}, service.fieldValues ))
    };

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
