/* eslint-disable no-whitespace-before-property */

import { AppStateService }                          from './AppStateService';
import { Service, useService }                      from './Service';
import { TransactionForm }                          from './TransactionForm';
import { Transaction, TRANSACTION_TYPE }            from './Transaction';
import * as util                                    from './util/util';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                                 from 'mobx-react';
import React, { useState }                          from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment }  from 'semantic-ui-react';

//----------------------------------------------------------------//
export const gTransactionTypes = [
    TRANSACTION_TYPE.SEND_VOL,
    TRANSACTION_TYPE.ACCOUNT_POLICY,
    TRANSACTION_TYPE.BETA_GET_ASSETS,
    TRANSACTION_TYPE.KEY_POLICY,
    TRANSACTION_TYPE.OPEN_ACCOUNT,
    TRANSACTION_TYPE.REGISTER_MINER,
    TRANSACTION_TYPE.RENAME_ACCOUNT,
];

//================================================================//
// TransactionDropdown
//================================================================//
const TransactionDropdown = observer (( props ) => {

    const { onChange } = props;

    let options = [];
    gTransactionTypes.forEach ( function ( transactionType, index ) {
        options.push ({ key:index, value:index, text:Transaction.friendlyNameForType ( transactionType )});
    });

    return (
        <Dropdown
            placeholder = "Create Transaction"
            fluid
            search
            selection
            options = { options }
            onChange = {( event, data ) => { onChange ( data.value )}}
        />
    );
});

//================================================================//
// TransactionFormSelector
//================================================================//
export const TransactionFormSelector = observer (( props ) => {

    const { appState } = props;

    const [ index, setIndex ] = useState ( -1 );
    const [ formIsShown, setFormIsShown ] = useState ( false );

    const canShowForm = ( appState.nonce >= 0 );

    const onSubmit = ( transaction ) => {
        appState.pushTransaction ( transaction );
        setFormIsShown ( false );
    };

    return (
        <Choose>
            <When condition = { canShowForm && formIsShown }>
                <TransactionDropdown onChange = { setIndex }/>
                <If condition = { 0 <= index }>
                    <Segment stacked>
                        <TransactionForm
                            key                 = { index }
                            appState            = { appState }
                            transactionType     = { gTransactionTypes [ index ]}
                            onSubmit            = { onSubmit }
                        />
                    </Segment>
                </If>
                <Divider/>
                <Button color = "red" fluid onClick = {() => { setFormIsShown ( false )}}>
                    Cancel
                </Button>
            </When>
            <Otherwise>
                <Button color = "teal" fluid disabled = { !canShowForm } onClick = {() => { setFormIsShown ( true )}}>
                    New Transaction
                </Button>
            </Otherwise>
        </Choose>
    );
});
