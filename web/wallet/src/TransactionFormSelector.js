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
        this.transactionSchemas = transactionSchemas ();

        this.state = {
            showForm: false,
            schemaIndex: -1,
        };
    }

    //----------------------------------------------------------------//
    handleSubmit ( schema, fieldValues ) {

        fieldValues.makerNonce = this.props.nonce;

        this.showForm ( false );
        this.props.appState.startTransaction ( schema, fieldValues );
    }

    //----------------------------------------------------------------//
    render () {

        const { appState, accountId, nonce }  = this.props;

        const pendingTransaction = appState.getPendingTransaction ( accountId );
        if ( pendingTransaction ) {
            return this.renderPendingTransaction ( pendingTransaction );
        }

        const isShowFormEnabled = (
            accountId &&
            ( accountId.length > 0 ) &&
            ( nonce >= 0 )
        );

        if ( isShowFormEnabled && this.state.showForm ) {

            let transactionForm;

            const schemaIndex = this.state.schemaIndex;

            if ( 0 <= schemaIndex ) {

                let onSubmit = ( schema, fieldValues ) => {
                    this.handleSubmit ( schema, fieldValues )
                };

                transactionForm = (
                    <TransactionForm
                        accountId = { accountId }
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
    renderPendingTransaction ( transaction ) {

        return (
            <p>{ transaction.friendlyName }</p>
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
