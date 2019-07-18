/* eslint-disable no-whitespace-before-property */

import TransactionListView                  from './TransactionListView';
import { Transaction }                      from './util/Transaction';
import React, { useState }                  from 'react';
import { observer }                         from 'mobx-react';
import { Button, Form, Header, Segment }    from 'semantic-ui-react';

//================================================================//
// PendingTransactionsView
//================================================================//
const PendingTransactionsView = observer (( props ) => {
    
    const appState = props.appState;

    return (
        <div>
            <Header as = "h2">Pending Transactions</Header>
            <TransactionListView transactions = { appState.pendingTransactions }/>
        </div>
    );
});

export default PendingTransactionsView;