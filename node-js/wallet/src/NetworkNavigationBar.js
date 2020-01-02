/* eslint-disable no-whitespace-before-property */

import { ImportAccountModal }               from './ImportAccountModal';
import { NavigationBar }                    from './NavigationBar';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Redirect }                         from 'react-router';
import { Link }                             from 'react-router-dom';
import { Dropdown, Icon, Label, Menu }      from 'semantic-ui-react';

export const NETWORK_TABS = {
    NETWORK:            'NETWORK',
    CHAIN:              'CHAIN',
    ACCOUNT_REQUESTS:   'ACCOUNT_REQUESTS',
};

//----------------------------------------------------------------//
function getAccountTabTitle ( tab ) {

    switch ( tab ) {
        case NETWORK_TABS.NETWORK:              return 'Network';
        case NETWORK_TABS.CHAIN:                return 'Chain';
        case NETWORK_TABS.ACCOUNT_REQUESTS:     return 'Account Requests';
    }
    return '';
};

//----------------------------------------------------------------//
function getAccountTabURL ( tab ) {

    switch ( tab ) {
        case NETWORK_TABS.NETWORK:              return '/';
        case NETWORK_TABS.CHAIN:                return '/chain';
        case NETWORK_TABS.ACCOUNT_REQUESTS:     return '/account-requests';
    }
    return '/';
};

//================================================================//
// NetworkNavigationBar
//================================================================//
export const NetworkNavigationBar = observer (( props ) => {

    const { appState, navTitle, networkID, tab } = props;

    const chainURL              = `/net/${ networkID }${ getAccountTabURL ( NETWORK_TABS.CHAIN )}`;
    const networkURL            = `/net/${ networkID }${ getAccountTabURL ( NETWORK_TABS.NETWORK )}`;
    const requestAccountURL     = `/net/${ networkID }${ getAccountTabURL ( NETWORK_TABS.ACCOUNT_REQUESTS )}`;

    const importAccountURL      = `/net/${ networkID }/import-account`;

    return (
        <React.Fragment>
            <NavigationBar
                appState    = { appState }
                networkID   = { networkID }
                networkTab  = { getAccountTabURL ( tab )}
                accountID   = { '' }
            />

            <Menu borderless attached = 'bottom'>

                <Dropdown item text = { getAccountTabTitle ( tab )} style = {{ textTransform: 'uppercase' }}>
                    <Dropdown.Menu>
                        <Dropdown.Item text = { getAccountTabTitle ( NETWORK_TABS.NETWORK )} as = { Link } to = { networkURL }/>
                        <Dropdown.Item text = { getAccountTabTitle ( NETWORK_TABS.CHAIN )} as = { Link } to = { chainURL }/>
                        <Dropdown.Item text = { getAccountTabTitle ( NETWORK_TABS.ACCOUNT_REQUESTS )} as = { Link } to = { requestAccountURL }/>
                    </Dropdown.Menu>
                </Dropdown>
                

                <Menu.Menu position = 'right'>

                    <ImportAccountModal
                        appState = { appState }
                        trigger = {
                            <Menu.Item icon = "add square" text = "Import Account"/>
                        }
                    />

                </Menu.Menu>

            </Menu>
        </React.Fragment>
    );
});
