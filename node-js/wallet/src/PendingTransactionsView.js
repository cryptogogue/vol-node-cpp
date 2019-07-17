/* eslint-disable no-whitespace-before-property */

import { Transaction }                      from './utils/Transaction';
import React, { useState }                  from 'react';
import { observer }                         from 'mobx-react';
import { Button, Form, Header, Segment }    from 'semantic-ui-react';

//================================================================//
// PendingTransactionsView
//================================================================//
const PendingTransactionsView = observer (( props ) => {
    

    const appState = props.appState;
    const pendingTransactions = appState.pendingTransactions;

    if ( pendingTransactions.length === 0 ) return <div/>;

    let transactionList = [];
    for ( let i in pendingTransactions ) {
        
        const memo = pendingTransactions [ i ];
        transactionList.push (<p key = { i }>{ `${ memo.body.maker.nonce }: ${ Transaction.friendlyNameForType ( memo.type )}` }</p>);
    }

    return (
        <div>
            <Header as = "h2">Pending Transactions</Header>
            { transactionList }
        </div>
    );
});

export default PendingTransactionsView;
