/* eslint-disable no-whitespace-before-property */

import { AppStateService }                  from './AppStateService';
import { KeySelector }                      from './KeySelector';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import { action, computed, extendObservable, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Redirect }                         from 'react-router';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Modal, Segment } from 'semantic-ui-react';

import { LoginForm }                        from './LoginForm';
import { DashboardNavigationBar }           from './DashboardNavigationBar';
import { NodeListView }                     from './NodeListView';
import { PendingTransactionsView }          from './PendingTransactionsView';
import { RegisterForm }                        from './RegisterForm';

import { AccountInfoService }               from './AccountInfoService';
import { NodeInfoService }                  from './NodeInfoService';

import { TransactionFormSelector }          from './TransactionFormSelector';

//================================================================//
// DashboardScreen
//================================================================//
export const DashboardScreen = observer (( props ) => {

    const appState = hooks.useFinalizable (() => new AppStateService ());

    return (
        <SingleColumnContainerView>

            <Choose>

                <When condition = { !appState.hasUser ()}>
                    <RegisterForm appState = { appState }/>
                </When>

                <When condition = { !appState.isLoggedIn ()}>
                    <LoginForm appState = { appState }/>
                </When>

                <Otherwise>
                    <DashboardNavigationBar
                        appState = { appState }
                    />

                    <NodeListView appState = { appState }/>
                    
                </Otherwise>
            </Choose>

        </SingleColumnContainerView>
    );
});
