/* eslint-disable no-whitespace-before-property */

import React, { useState }                  from 'react';
import { observer }                         from 'mobx-react';
import { Button, Form, Header, Segment }    from 'semantic-ui-react';

import * as transactions            from './transactions';

//================================================================//
// StagedTransactionsView
//================================================================//
const StagedTransactionsView = observer (( props ) => {
    

    const appState = props.appState;
    const stagedTransactions = appState.stagedTransactions;

    if ( stagedTransactions.length === 0 ) return <div/>;

    let onClickSubmit = () => { appState.submitTransactions ()};
    let onClickClear = () => { appState.clearStagedTransactions ()};

    let transactionList = [];
    for ( let i in stagedTransactions ) {

        const memo = stagedTransactions [ i ];
        const transaction = transactions.makeTransaction ( memo.type, memo.fieldValues );

        if ( transaction ) {
            transactionList.push (<p key = { i }>{ transaction.schema.friendlyName }</p>);
        }
    }

    return (
        <div>
            <Header as = "h2">Staged Transactions</Header>

            { transactionList }

            <Segment stacked>
                <Button color = "teal" fluid disabled = { !appState.canSubmitTransactions } onClick = { onClickSubmit }>Submit</Button>
                <Button color = "red" fluid onClick = { onClickClear }>Clear</Button>
            </Segment>
        </div>
    );
});

export default StagedTransactionsView;
