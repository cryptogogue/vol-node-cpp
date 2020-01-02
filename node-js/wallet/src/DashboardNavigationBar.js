/* eslint-disable no-whitespace-before-property */

import { AddNetworkModal }                  from './AddNetworkModal';
import { NavigationBar }                    from './NavigationBar';
import { StagedTransactionsModal }          from './StagedTransactionsModal';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Redirect }                         from 'react-router';
import { Link }                             from 'react-router-dom';
import { Dropdown, Icon, Label, Menu }      from 'semantic-ui-react';
  
//================================================================//
// DashboardNavigationBar
//================================================================//
export const DashboardNavigationBar = observer (( props ) => {

    const { appState } = props;

    let onClickAffirmLocalhostNodes     = () => { appState.affirmLocalhostNodes ()};
    let onClickDeleteAccount            = () => { appState.deleteAccount ()};
    let onClickDeleteAllStorage         = () => { appState.deleteStorage ()};
    let onClickDeleteTransactions       = () => { appState.deleteTransactions ()};
    let onClickDeleteUserStorage        = () => { appState.deleteUserStorage ()};
    let onClickDeleteNodeList           = () => { appState.deleteNodeList ()};
    let onClickResetMiningNode          = () => { appState.affirmLocalhostNodes ()};

    const previewSchemaURL      = `/debug/schema`;
    const resetURL              = `/debug/reset`;

    return (
        <React.Fragment>
            <NavigationBar
                appState    = { appState }
            />

            <Menu borderless attached = 'bottom'>

                <Menu.Menu position = 'right'>

                    <AddNetworkModal
                        appState = { appState }
                        trigger = {
                            <Menu.Item icon = "add square" text = "Add Network"/>
                        }
                    />

                    <Dropdown item icon = "settings">
                        <Dropdown.Menu>
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
                
            </Menu>
        </React.Fragment>
    );
});

