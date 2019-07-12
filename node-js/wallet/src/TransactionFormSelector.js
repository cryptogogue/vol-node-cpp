/* eslint-disable no-whitespace-before-property */

import { AppStateStore }                                                        from './stores/AppStateStore';
import { Service }                                                              from './stores/Service';
import { Store, useStore }                                                      from './stores/Store';
import * as util                                                                from './utils/util';
import { action, computed, extendObservable, observable, observe }              from 'mobx';
import { observer }                                                             from 'mobx-react';
import React, { useState }                                                      from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment }  from 'semantic-ui-react';

import TransactionForm              from './TransactionForm';
import { transactionSchemas }       from './TransactionSchemas';

//================================================================//
// TransactionSelectorService
//================================================================//
class TransactionSelectorService extends Service {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        this.appState = appState;

        // TODO: this is a placeholder; transaction schemas should come from
        // the server.
        this.transactionSchemas = transactionSchemas ();

        extendObservable ( this, {
            formIsShown: false,
            schemaIndex: -1,
        });
    }

    //----------------------------------------------------------------//
    handleSubmit ( schema, fieldValues ) {

        fieldValues.makerNonce = this.props.nonce;

        this.showForm ( false );
        this.appState.startTransaction ( schema, fieldValues );
    }

    //----------------------------------------------------------------//
    @action
    selectForm ( index ) {

        this.schemaIndex = index;
    }

    //----------------------------------------------------------------//
    @action
    showForm ( show ) {

        this.formIsShown = show;
        this.schemaIndex = -1;
    }
}

//================================================================//
// TransactionFormSelector
//================================================================//
const TransactionFormSelector = observer (( props ) => {

    const { appState, accountId, nonce } = props;

    const service = useStore (() => new TransactionSelectorService ( appState ));

    const pendingTransaction = appState.getPendingTransaction ( accountId );
    if ( pendingTransaction ) {
        return renderPendingTransaction ( pendingTransaction );
    }

    const isShowFormEnabled = (
        accountId &&
        ( accountId.length > 0 ) &&
        ( nonce >= 0 )
    );

    if ( isShowFormEnabled && service.formIsShown ) {

        let transactionForm;

        const schemaIndex = service.schemaIndex;

        if ( 0 <= schemaIndex ) {

            let onSubmit = ( schema, fieldValues ) => {
                service.handleSubmit ( schema, fieldValues )
            };

            transactionForm = (
                <TransactionForm
                    appState = { appState }
                    schema = { service.transactionSchemas [ schemaIndex ]}
                    onSubmit = { onSubmit }
                />
            );
        }

        return (
            <div>
                { renderDropdown ( service )}
                { transactionForm }
                <Divider/>
                <Button color = "red" fluid onClick = {() => { service.showForm ( false )}}>
                    Cancel
                </Button>
            </div>
        );
    }

    return (
        <div>
            <Button color = "teal" fluid disabled = { !isShowFormEnabled } onClick = {() => { service.showForm ( true )}}>
                New Transaction
            </Button>
        </div>
    );
});

//----------------------------------------------------------------//
function renderDropdown ( service ) {

    let options = [];

    service.transactionSchemas.forEach ( function ( schema, index ) {
        options.push ({ key:schema.transactionType, value:index, text:schema.friendlyName });
    });

    return (
        <Dropdown
            placeholder = "Create Transaction"
            fluid
            search
            selection
            options = { options }
            onChange = {( event, data ) => { service.selectForm ( data.value )}}
        />
    );
}

//----------------------------------------------------------------//
function renderPendingTransaction ( transaction ) {

    return (
        <p>{ transaction.friendlyName }</p>
    );
}

export default TransactionFormSelector;
