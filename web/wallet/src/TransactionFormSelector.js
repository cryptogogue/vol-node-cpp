/* eslint-disable no-whitespace-before-property */

import { withAppState }             from './AppStateProvider';
import TransactionForm              from './TransactionForm';
import { transactionSchemas }       from './TransactionSchemas';
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
        
        // placeholder; eventuall this will be local to the form. because
        // schemas will be pulled from the server and may be unique to
        // the account.
        this.transactionSchemas = transactionSchemas ();

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
