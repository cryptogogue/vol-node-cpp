/* eslint-disable no-whitespace-before-property */

import { Transaction }      from './Transaction';
import JSONTree             from 'react-json-tree';
import React, { useState }  from 'react';
import { observer }         from 'mobx-react';
import * as UI              from 'semantic-ui-react';

//================================================================//
// TransactionListView
//================================================================//
export const TransactionListView = observer (( props ) => {
    
    const { transactions } = props;
    const showNonce = props.showNonce || false;

    let transactionList = [];
    for ( let i in transactions ) {

        const transaction = transactions [ i ];
        let friendlyName = Transaction.friendlyNameForType ( transaction.type );

        transactionList.push (
            <UI.Table.Row key = { i }>
                <UI.Table.Cell collapsing>
                    <UI.Modal
                        header      = 'Transaction Body'
                        trigger     = {
                            <UI.Header
                                as = 'h5'
                                style = {{ cursor: 'pointer' }}
                            >
                                { friendlyName }
                            </UI.Header>
                        }
                        content     = {
                            <JSONTree
                                hideRoot
                                data = { JSON.parse ( transaction.body )}
                                theme = 'bright'
                            />
                        }
                    />
                </UI.Table.Cell>
                <UI.Table.Cell collapsing>{ transaction.cost }</UI.Table.Cell>
                <UI.Table.Cell>{ transaction.note }</UI.Table.Cell>
                <If condition = { showNonce }>
                    <UI.Table.Cell collapsing>{ transaction.nonce || 0 }</UI.Table.Cell>
                </If>
            </UI.Table.Row>
        );
    }

    return (
        <UI.Table unstackable>
            <UI.Table.Header>
                <UI.Table.Row>
                    <UI.Table.HeaderCell>Type</UI.Table.HeaderCell>
                    <UI.Table.HeaderCell>Cost</UI.Table.HeaderCell>
                    <UI.Table.HeaderCell>Note</UI.Table.HeaderCell>
                    <If condition = { showNonce }>
                        <UI.Table.HeaderCell>Nonce</UI.Table.HeaderCell>
                    </If>
                </UI.Table.Row>
            </UI.Table.Header>
            <UI.Table.Body>
                { transactionList }
            </UI.Table.Body>
        </UI.Table>
    );
});
