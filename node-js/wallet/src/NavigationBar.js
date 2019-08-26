/* eslint-disable no-whitespace-before-property */

import { observer }             from 'mobx-react';
import React                    from 'react';
import { Link }                 from 'react-router-dom';
import { Dropdown, Menu }       from 'semantic-ui-react';
  
//================================================================//
// NavigationBar
//================================================================//
export const NavigationBar = observer (( props ) => {
        
    const { appState, navTitle } = props;
    const { accountId } = appState;

    let onClickAffirmLocalhostNodes     = () => { appState.affirmLocalhostNodes ()};
    let onClickDeleteAccount            = () => { appState.deleteAccount ()};
    let onClickDeleteAllStorage         = () => { appState.deleteStorage ()};
    let onClickDeleteTransactions       = () => { appState.deleteTransactions ()};
    let onClickDeleteUserStorage        = () => { appState.deleteUserStorage ()};
    let onClickDeleteNodeList           = () => { appState.deleteNodeList ()};
    let onClickLogout                   = () => { appState.login ( false )};

    const accountsURL   = appState.prefixURL ( `/accounts/${ accountId }` );
    const inventoryURL  = appState.prefixURL ( `/accounts/${ accountId }/inventory` );
    const craftingURL   = appState.prefixURL ( `/accounts/${ accountId }/crafting` );

    return (
        <Menu>
            <Menu.Menu position = "left">
                <Dropdown item icon = "bars">
                    <Dropdown.Menu>
                        <Dropdown.Item icon = "add square" text = "Accounts" as = { Link } to = { accountsURL }/>
                        <Dropdown.Item disabled = { !appState.hasAccount } icon = "add square" text = "Inventory" as = { Link } to = { inventoryURL }/>
                        <Dropdown.Item disabled = { !appState.hasAccount } icon = "add square" text = "Crafting" as = { Link } to = { craftingURL }/>
                    </Dropdown.Menu>
                </Dropdown>
            </Menu.Menu>

            <span style = {{
                fontSize: '2rem',
                margin: 'auto',
                paddingLeft: '4.5rem',
            }}>
                { navTitle }
            </span>
    
            <Menu.Menu position = "right">
                <Dropdown item icon = "settings">
                    <Dropdown.Menu>
                        <Dropdown.Item icon = "add square" text = "Create Account" as = { Link } to = { appState.prefixURL ( '/accounts/new' )}/>
                        <Dropdown.Item icon = "add square" text = "Import Account" as = { Link } to = { appState.prefixURL ( '/accounts/import' )}/>
                        <Dropdown.Item icon = "log out" text = "Logout" onClick = { onClickLogout }/>
                        <Dropdown.Item icon = "globe" text = "DEBUG: Affirm Localhost Nodes" onClick = { onClickAffirmLocalhostNodes }/>
                        <Dropdown.Item icon = "globe" text = "DEBUG: Delete Account" onClick = { onClickDeleteAccount }/>
                        <Dropdown.Item icon = "globe" text = "DEBUG: Delete Node List" onClick = { onClickDeleteNodeList }/>
                        <Dropdown.Item icon = "globe" text = "DEBUG: Delete Transactions" onClick = { onClickDeleteTransactions }/>
                        <Dropdown.Item icon = "globe" text = "DEBUG: Delete User Storage" onClick = { onClickDeleteUserStorage }/>
                        <Dropdown.Item icon = "globe" text = "DEBUG: Delete All Storage" onClick = { onClickDeleteAllStorage }/>
                    </Dropdown.Menu>
                </Dropdown>
            </Menu.Menu>
        </Menu>
    );
});
