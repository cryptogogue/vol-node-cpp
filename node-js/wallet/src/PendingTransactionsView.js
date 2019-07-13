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
    const pendingTransactions = appState.pendingTransactions;

    if ( pendingTransactions.length === 0 ) return <div/>;

    let transactionList = [];
    for ( let i in pendingTransactions ) {
        
        const memo = pendingTransactions [ i ];
        const transaction = transactions.makeTransaction ( memo.type, memo.fieldValues );

        if ( transaction ) {
            transactionList.push (<p key = { i }>{ `${ memo.nonce }: ${ transaction.schema.friendlyName }` }</p>);
        }
    }

    return (
        <div>
            <Header as = "h2">Pending Transactions</Header>
            { transactionList }
        </div>
    );
});

export default StagedTransactionsView;
