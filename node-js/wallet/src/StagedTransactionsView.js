/* eslint-disable no-whitespace-before-property */

import TransactionListView                                  from './TransactionListView';
import { Transaction }                                      from './util/Transaction';
import React, { useState }                                  from 'react';
import { observer }                                         from 'mobx-react';
import { Button, Divider, Form, Header, Modal, Segment }    from 'semantic-ui-react';

//================================================================//
// StagedTransactionsView
//================================================================//
const StagedTransactionsView = observer (( props ) => {

    const appState = props.appState;

    let onClickSubmit = () => { appState.submitTransactions ()};
    let onClickClear = () => { appState.clearStagedTransactions ()};

    return (
        <div>
            <Header as = "h2">Staged Transactions</Header>
            <TransactionListView transactions = { appState.stagedTransactions }/>
            <Segment stacked>
                <Button color = "teal" fluid disabled = { !appState.canSubmitTransactions } onClick = { onClickSubmit }>Submit</Button>
                <Button color = "red" fluid onClick = { onClickClear }>Clear</Button>
            </Segment>
        </div>
    );
});

export default StagedTransactionsView;