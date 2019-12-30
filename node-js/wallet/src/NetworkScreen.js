/* eslint-disable no-whitespace-before-property */

import { AppStateService }                  from './AppStateService';
import { KeySelector }                      from './KeySelector';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import { action, computed, extendObservable, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Redirect }                         from 'react-router';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment } from 'semantic-ui-react';

import { NavigationBar }                    from './NavigationBar';
import { NodeListView }                     from './NodeListView';
import { PendingTransactionsView }          from './PendingTransactionsView';

import { AccountInfoService }               from './AccountInfoService';
import { NodeInfoService }                  from './NodeInfoService';

import { TransactionFormSelector }          from './TransactionFormSelector';

//================================================================//
// NetworkScreen
//================================================================//
export const NetworkScreen = observer (( props ) => {

    const networkIDFromEndpoint     = util.getMatch ( props, 'networkID' );
    const appState                  = hooks.useFinalizable (() => new AppStateService ());

    return (
        <SingleColumnContainerView>

            <NavigationBar
                navTitle = 'Accounts'
                appState = { appState }
                networkID = { networkIDFromEndpoint }
                accountID = { '' }
            />

        </SingleColumnContainerView>
    );
});
