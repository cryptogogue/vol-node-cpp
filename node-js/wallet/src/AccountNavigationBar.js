/* eslint-disable no-whitespace-before-property */

import { NavigationBar }                    from './NavigationBar';
import { StagedTransactionsModal }          from './StagedTransactionsModal';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Redirect }                         from 'react-router';
import { Link }                             from 'react-router-dom';
import { Button, Dropdown, Header, Icon, Label, Menu } from 'semantic-ui-react';

export const ACCOUNT_TABS = {
    ACCOUNT:        'ACCOUNT',
    INVENTORY:      'INVENTORY',
    CRAFTING:       'CRAFTING',
    UPGRADES:       'UPGRADES',
};

//----------------------------------------------------------------//
function getAccountTabTitle ( tab ) {

    switch ( tab ) {
        case ACCOUNT_TABS.ACCOUNT:      return 'Account';
        case ACCOUNT_TABS.INVENTORY:    return 'Inventory';
        case ACCOUNT_TABS.CRAFTING:     return 'Crafting';
        case ACCOUNT_TABS.UPGRADES:     return 'Upgrades';
    }
    return '';
};

//----------------------------------------------------------------//
function getAccountTabURL ( tab ) {

    switch ( tab ) {
        case ACCOUNT_TABS.ACCOUNT:      return '/';
        case ACCOUNT_TABS.INVENTORY:    return '/inventory';
        case ACCOUNT_TABS.CRAFTING:     return '/crafting';
        case ACCOUNT_TABS.UPGRADES:     return '/upgrades';
    }
    return '/';
};

//================================================================//
// AccountNavigationBar
//================================================================//
export const AccountNavigationBar = observer (( props ) => {

    const { appState, networkID, accountID, tab } = props;

    const accountsURL           = `/net/${ networkID }/account/${ accountID }${ getAccountTabURL ( ACCOUNT_TABS.ACCOUNT )}`;
    const inventoryURL          = `/net/${ networkID }/account/${ accountID }${ getAccountTabURL ( ACCOUNT_TABS.INVENTORY )}`;
    const craftingURL           = `/net/${ networkID }/account/${ accountID }${ getAccountTabURL ( ACCOUNT_TABS.CRAFTING )}`;
    const upgradesURL           = `/net/${ networkID }/account/${ accountID }${ getAccountTabURL ( ACCOUNT_TABS.UPGRADES )}`;

    const stagedTransactions = appState.stagedTransactions.length;

    return (
        <React.Fragment>
            <NavigationBar
                appState    = { appState }
                networkID   = { networkID }
                accountID   = { accountID }
                accountTab  = { getAccountTabURL ( tab )}
            />

            <Menu borderless attached = 'bottom'>

                <Dropdown item text = { getAccountTabTitle ( tab )} style = {{ textTransform: 'uppercase' }}>
                    <Dropdown.Menu>
                        <Dropdown.Item text = { getAccountTabTitle ( ACCOUNT_TABS.ACCOUNT )} as = { Link } to = { accountsURL }/>
                        <Dropdown.Item text = { getAccountTabTitle ( ACCOUNT_TABS.INVENTORY )} as = { Link } to = { inventoryURL }/>
                        <Dropdown.Item text = { getAccountTabTitle ( ACCOUNT_TABS.CRAFTING )} as = { Link } to = { craftingURL }/>
                        <Dropdown.Item text = { getAccountTabTitle ( ACCOUNT_TABS.UPGRADES )} as = { Link } to = { upgradesURL }/>
                    </Dropdown.Menu>
                </Dropdown>

                <Menu.Menu position = 'right'>

                    <Menu.Item>
                        <StagedTransactionsModal
                            appState = { appState }
                            trigger = {
                                <Label
                                    color = { stagedTransactions > 0 ? 'green' : 'grey' }
                                    disabled = { stagedTransactions === 0 }
                                >
                                    <Icon name = 'cloud upload'/>
                                    { stagedTransactions }
                                </Label>
                            }
                        />
                    </Menu.Item>
                    
                </Menu.Menu>
            </Menu>
        </React.Fragment>
    );
});
