/* eslint-disable no-whitespace-before-property */

import { AppStateService }                                                      from './stores/AppStateService';
import { Service, useService }                                                  from './stores/Service';
import TransactionForm                                                          from './TransactionForm';
import { Transaction, TRANSACTION_TYPE }                                        from './util/Transaction';
import * as util                                                                from './util/util';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                                                             from 'mobx-react';
import React, { useState }                                                      from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment }  from 'semantic-ui-react';

//----------------------------------------------------------------//
export const transactionTypes = [
    TRANSACTION_TYPE.SEND_VOL,
    TRANSACTION_TYPE.ACCOUNT_POLICY,
    TRANSACTION_TYPE.KEY_POLICY,
    TRANSACTION_TYPE.OPEN_ACCOUNT,
    TRANSACTION_TYPE.REGISTER_MINER,
    TRANSACTION_TYPE.SEND_VOL,
];

//================================================================//
// TransactionSelectorService
//================================================================//
class TransactionSelectorService extends Service {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        this.appState = appState;

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
                    transactionType = { transactionTypes [ index ]}
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

    transactionTypes.forEach ( function ( transactionType, index ) {
        options.push ({ key:index, value:index, text:Transaction.friendlyNameForType ( transactionType )});
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
