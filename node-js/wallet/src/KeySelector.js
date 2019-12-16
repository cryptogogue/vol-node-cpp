/* eslint-disable no-whitespace-before-property */

import { AppStateService }                          from './AppStateService';
import { TransactionForm }                          from './TransactionForm';
import { Transaction, TRANSACTION_TYPE }            from './Transaction';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                                 from 'mobx-react';
import React, { useState }                          from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment }  from 'semantic-ui-react';

//================================================================//
// KeySelector
//================================================================//
export const KeySelector = observer (( props ) => {

    const { appState, onChange } = props;

    const account = appState.account;
    const accountKeyNames = appState.accountKeyNames;
    const options = [];
    for ( let i in accountKeyNames ) {

        const keyName = accountKeyNames [ i ];
        const key = account.keys [ keyName ];

        options.push ({
            key: i,
            text: keyName,
            value: keyName,
            disabled: !Boolean ( key.policy ),
        });
    }

    return (
        <Dropdown
            placeholder     = "Select Key"
            fluid
            search
            selection       
            options         = { options }
            defaultValue    = { appState.keyName }
            onChange        = {( event, data ) => { onChange && onChange ( data.value )}}
        />
    );
});
