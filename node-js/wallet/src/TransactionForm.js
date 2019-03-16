/* eslint-disable no-whitespace-before-property */

import { withAppState }             from './AppStateProvider';
import React, { Component }         from 'react';
import { Button, Form, Segment, Select } from 'semantic-ui-react';

//================================================================//
// TransactionForm
//================================================================//
class TransactionForm extends Component {

    //----------------------------------------------------------------//
    checkFormInputs () {

        const { schema } = this.props;
        let isValid = true;

        Object.keys ( schema.fields ).forEach (( fieldName ) => {
            let fieldValue = this.state [ fieldName ];
            if ( fieldValue === null ) {
                isValid = false;
            }
        });

        return isValid;
    }

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        const { accountId, schema } = this.props;

        const defaultKeyName = this.props.appState.getDefaultAccountKeyName ( accountId, 'master' );

        let fieldValues = {
            makerAccountName: this.props.accountId,
            makerKeyName: defaultKeyName,
            makerNonce: -1,
        };

        Object.keys ( schema.fields ).forEach (( name ) => {
            fieldValues [ name ] = fieldValues [ name ] || null;
        });

        this.state = fieldValues;
    }

    //----------------------------------------------------------------//
    handleChange ( event ) {
        
        let typedValue = null;
        const value = event.target.value;

        if ( value && ( value.length > 0 )) {
            const type = event.target.type;
            typedValue = ( type === 'number' ) ? Number ( value ) : String ( value );
        }

        this.setState ({[ event.target.name ]: typedValue });
    }

    //----------------------------------------------------------------//
    makeFormInputForField ( name, field ) {

        let onChange = ( event ) => { this.handleChange ( event )};

        if ( name === 'makerAccountName' ) return;
        if ( name === 'makerNonce' ) return;

        let value = this.state [ name ] === null ? '' : this.state [ name ];

        if ( name === 'makerKeyName' ) {

            const accountKeyNames = this.props.appState.getAccountKeyNames ( this.props.accountId );

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

    //----------------------------------------------------------------//
    render () {

        const schema = this.props.schema;

        // order fields using hashOrder
        let orderedFields = [];
        Object.keys ( schema.fields ).forEach (( name ) => {
            const field = schema.fields [ name ];
            orderedFields [ field.hashOrder ] = name;
        });

        // add the fields in order
        let fields = [];
        orderedFields.forEach (( name ) => {
            let formInput = this.makeFormInputForField ( name, schema.fields [ name ]);
            if ( formInput ) {
                fields.push ( formInput );
            }
        });

        const isSubmitEnabled = this.checkFormInputs ();

        let onClickSend = () => {
            this.props.onSubmit ( this.props.schema, Object.assign ({}, this.state ))
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
    }
}

export default withAppState ( TransactionForm );
