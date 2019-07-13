/* eslint-disable no-whitespace-before-property */

import { AppStateService }                                                      from './stores/AppStateService';
import { Service, useService }                                                  from './stores/Service';
import * as util                                                                from './utils/util';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                                                             from 'mobx-react';
import React, { useState }                                                      from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment }  from 'semantic-ui-react';

import TransactionForm              from './TransactionForm';
import * as transactions            from './transactions';

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
        this.transactionTypes = transactions.transactionTypes;

        extendObservable ( this, {
            formIsShown: false,
            index: -1,
        });
    }

    //----------------------------------------------------------------//
    handleSubmit ( transaction ) {

        this.appState.pushTransaction ( transaction );
        this.showForm ( false );
    }

    //----------------------------------------------------------------//
    @action
    selectForm ( index ) {

        this.index = index;
    }

    //----------------------------------------------------------------//
    @action
    showForm ( show ) {

        this.formIsShown = show;
        this.index = -1;
    }
}

//================================================================//
// TransactionFormSelector
//================================================================//
const TransactionFormSelector = observer (( props ) => {

    const { appState } = props;

    const service = useService (() => new TransactionSelectorService ( appState ));

    const isShowFormEnabled = ( appState.nonce >= 0 );

    if ( isShowFormEnabled && service.formIsShown ) {

        let transactionForm;

        const index = service.index;

        if ( 0 <= index ) {

            let onSubmit = ( transaction ) => {
                service.handleSubmit ( transaction )
            };

            transactionForm = (
                <TransactionForm
                    appState = { appState }
                    transactionType = { service.transactionTypes [ index ]}
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

    service.transactionTypes.forEach ( function ( transactionType, index ) {
        const schema = transactions.schemaForType ( transactionType );
        options.push ({ key:index, value:index, text:schema.friendlyName });
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

export default TransactionFormSelector;
