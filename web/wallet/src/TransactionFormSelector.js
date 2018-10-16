/* eslint-disable no-whitespace-before-property */

import { withAppState }             from './AppStateProvider';
import TransactionForm              from './TransactionForm';
import React, { Component }         from 'react';
import { Button, Divider, Dropdown } from 'semantic-ui-react';

//================================================================//
// TransactionFormSelector
//================================================================//
class TransactionFormSelector extends Component {

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        // TODO: this is a placeholder; transaction schemas should come from
        // the server.
        const sendVOLTransactionSchema = {
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

        this.transactionSchemas = [
            sendVOLTransactionSchema,
        ];

        this.state = {
            showForm: false,
            schemaIndex: -1,
        };
    }

    //----------------------------------------------------------------//
    handleSubmit ( schema, fieldValues ) {

        fieldValues.makerNonce = this.props.nonce;

        let transaction = {
            transactionType: schema.transactionType,
            friendlyName: schema.friendlyName,
            fieldValues: fieldValues,
        }

        this.showForm ( false );
        this.props.appState.pushTransaction ( transaction );
    }

    //----------------------------------------------------------------//
    render () {

        const isShowFormEnabled = ( this.props.nonce >= 0 );

        if ( isShowFormEnabled && this.state.showForm ) {

            let transactionForm;

            const schemaIndex = this.state.schemaIndex;

            if ( 0 <= schemaIndex ) {

                let onSubmit = ( schema, fieldValues ) => {
                    this.handleSubmit ( schema, fieldValues )
                };

                transactionForm = (
                    <TransactionForm
                        accountId = { this.props.accountId }
                        schema = { this.transactionSchemas [ schemaIndex ]}
                        onSubmit = { onSubmit }
                    />
                );
            }

            return (
                <div>
                    { this.renderDropdown ()}
                    { transactionForm }
                    <Divider/>
                    <Button color = "red" fluid onClick = {() => { this.showForm ( false )}}>
                        Cancel
                    </Button>
                </div>
            );
        }

        return (
            <div>
                <Button color = "teal" fluid disabled = { !isShowFormEnabled } onClick = {() => { this.showForm ( true )}}>
                    New Transaction
                </Button>
            </div>
        );
    }

    //----------------------------------------------------------------//
    renderDropdown () {

        let options = [];

        this.transactionSchemas.forEach ( function ( schema, index ) {
            options.push ({ key:schema.transactionType, value:index, text:schema.friendlyName });
        });

        return (
            <Dropdown
                placeholder = "Create Transaction"
                fluid
                search
                selection
                options = { options }
                onChange = {( event, data ) => { this.selectForm ( data.value )}}
            />
        );
    }

    //----------------------------------------------------------------//
    selectForm ( index ) {

        this.setState ({ schemaIndex: index });
    }

    //----------------------------------------------------------------//
    showForm ( show ) {

        this.setState ({
            showForm: show,
            schemaIndex: -1,
        });
    }
}

export default withAppState ( TransactionFormSelector );
