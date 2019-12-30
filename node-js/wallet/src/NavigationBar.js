/* eslint-disable no-whitespace-before-property */

import { StagedTransactionsModal }          from './StagedTransactionsModal';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Redirect }                         from 'react-router';
import { Link }                             from 'react-router-dom';
import { Dropdown, Icon, Label, Menu }      from 'semantic-ui-react';
  
//================================================================//
// NavigationBar
//================================================================//
export const NavigationBar = observer (( props ) => {

    const { appState, navTitle, networkID, accountID } = props;
    const tab = props.tab || '';

    const [ redirect, setRedirect ] = useState ( false );

    if ( redirect ) {
        const temp = redirect;
        setRedirect ( false );
        return (<Redirect to = { temp }/>); 
    }

    let onClickAffirmLocalhostNodes     = () => { appState.affirmLocalhostNodes ()};
    let onClickDeleteAccount            = () => { appState.deleteAccount ()};
    let onClickDeleteAllStorage         = () => { appState.deleteStorage ()};
    let onClickDeleteTransactions       = () => { appState.deleteTransactions ()};
    let onClickDeleteUserStorage        = () => { appState.deleteUserStorage ()};
    let onClickDeleteNodeList           = () => { appState.deleteNodeList ()};
    let onClickLogout                   = () => { appState.login ( false )};
    let onClickResetMiningNode          = () => { appState.affirmLocalhostNodes ()};

    const accountsURL           = `/${ networkID }/accounts/${ accountID }`;
    const inventoryURL          = `/${ networkID }/accounts/${ accountID }/inventory`;
    const craftingURL           = `/${ networkID }/accounts/${ accountID }/crafting`;
    const resetURL              = `/${ networkID }/accounts/${ accountID }/reset`;
    const upgradesURL           = `/${ networkID }/accounts/${ accountID }/upgrades`;
    const previewSchemaURL      = `/debug/schema`;

    const networks = {
        network0:       [ '9090', '9091', '9092' ],
        network1:       [ '9093', '9094' ],
        network2:       [ '9095', '9096', '9097' ],
    };

    const accountDropdown = [];
    const accounts = networks [ networkID ];
    for ( let account of accounts ) {
        accountDropdown.push (
            <Dropdown.Item
                key         = { account }
                as          = { Link }
                to          = { `/${ networkID }/accounts/${ account }/${ tab }` }
            >
                { account }
            </Dropdown.Item>
        );
    }


    const networkDropdown = [];
    for ( let network in networks ) {

        networkDropdown.push (
            <Dropdown.Item
                key         = { network }
                as          = { Link }
                to          = { `/${ network }` }
            >
                <span className='text'>{ network }</span>
            </Dropdown.Item>
        );
    }

    const stagedTransactions = appState.stagedTransactions.length;

    return (
        <React.Fragment>

            <Menu attached = 'top' borderless inverted>

                <Menu.Item
                    icon = 'globe'
                />

                <Dropdown
                    item
                    text = { networkID }
                    placeholder = 'Network'
                >
                    <Dropdown.Menu>
                        { networkDropdown }
                    </Dropdown.Menu>
                </Dropdown>

                <Dropdown
                    item
                    text = { accountID }
                    placeholder = 'Account'
                >
                    <Dropdown.Menu>
                        { accountDropdown }
                    </Dropdown.Menu>
                </Dropdown>

                <Menu.Menu position = "right">
                    <Menu.Item
                        icon = 'power off'
                    />
                </Menu.Menu>
            </Menu>

            <Menu attached = 'bottom'>
                <Menu.Menu position = "left">
                    <Dropdown item icon = "bars">
                        <Dropdown.Menu>
                            <Dropdown.Item text = "Account" as = { Link } to = { accountsURL }/>
                            <Dropdown.Item text = "Inventory" as = { Link } to = { inventoryURL }/>
                            <Dropdown.Item text = "Crafting" as = { Link } to = { craftingURL }/>
                            <Dropdown.Item text = "Upgrades" as = { Link } to = { upgradesURL }/>
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
                    <Menu.Item>
                        <StagedTransactionsModal appState = { appState }/>
                    </Menu.Item>
                </Menu.Menu>
            </Menu>
        </React.Fragment>
    );
});

/*
<Menu.Menu position = "right">
    <Dropdown item icon = "settings">
        <Dropdown.Menu>
            <Dropdown.Item icon = "add square"  text = "Create Account"                 as = { Link } to = { '/account-new' }/>
            <Dropdown.Item icon = "add square"  text = "Import Account"                 as = { Link } to = { '/account-import' }/>
            <Dropdown.Item icon = "log out"     text = "Logout"                         onClick = { onClickLogout }/>
            <Dropdown.Item icon = "globe"       text = "DEBUG: Preview Schema"          as = { Link } to = { previewSchemaURL }/>
            <Dropdown.Item icon = "globe"       text = "DEBUG: Reset Mining Node"       as = { Link } to = { resetURL }/>
            <Dropdown.Item icon = "globe"       text = "DEBUG: Affirm Localhost Nodes"  onClick = { onClickAffirmLocalhostNodes }/>
            <Dropdown.Item icon = "globe"       text = "DEBUG: Delete Account"          onClick = { onClickDeleteAccount }/>
            <Dropdown.Item icon = "globe"       text = "DEBUG: Delete Node List"        onClick = { onClickDeleteNodeList }/>
            <Dropdown.Item icon = "globe"       text = "DEBUG: Delete Transactions"     onClick = { onClickDeleteTransactions }/>
            <Dropdown.Item icon = "globe"       text = "DEBUG: Delete User Storage"     onClick = { onClickDeleteUserStorage }/>
            <Dropdown.Item icon = "globe"       text = "DEBUG: Delete All Storage"      onClick = { onClickDeleteAllStorage }/>
        </Dropdown.Menu>
    </Dropdown>
</Menu.Menu>
*/
