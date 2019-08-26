/* eslint-disable no-whitespace-before-property */

import { Transaction }                                      from './util/Transaction';
import React, { useState }                                  from 'react';
import { observer }                                         from 'mobx-react';
import { Button, Divider, Form, Header, Modal, Segment }    from 'semantic-ui-react';

//================================================================//
// TransactionListView
//================================================================//
export const TransactionListView = observer (( props ) => {
    

    const { transactions } = props;

    if ( transactions.length === 0 ) return <div/>;

    let transactionList = [];
    for ( let i in transactions ) {

        const memo = transactions [ i ];

        let friendlyName = Transaction.friendlyNameForType ( memo.type );

        if ( memo.note !== '' ) {
            friendlyName = `${ friendlyName } - ${ memo.note }`;
        }

        if ( typeof ( memo.nonce ) === 'number' ) {
            friendlyName = `${ memo.nonce }: ${ friendlyName }`;
        }

        transactionList.push (
            <Modal
                key = { i }
                size = "small"
                trigger = {
                    <Header.Subheader>{ friendlyName }</Header.Subheader>
                }>
                <Modal.Content>
                    <center>
                        <h3>Transaction Body</h3>
                        <Divider/>
                        <p>{ memo.body }</p>
                    </center>
                </Modal.Content>
            </Modal>
        );
    }

    return (
        <div>
            { transactionList }
        </div>
    );
});
