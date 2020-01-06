/* eslint-disable no-whitespace-before-property */

import { ChangePasswordModal }              from './ChangePasswordModal';
import { NavigationBar }                    from './NavigationBar';
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
    const [ changePasswordModalOpen, setChangePasswordModalOpen ] = useState ( false );

    return (
        <React.Fragment>
            <NavigationBar
                appState    = { appState }
            />

            <Menu borderless attached = 'bottom'>
                <Menu.Menu position = 'right'>
                    <Dropdown
                        item
                        icon = "settings"
                        disabled = { appState.flags.promptFirstNetwork }
                    >
                        <Dropdown.Menu>
                            <Dropdown.Item icon = "wrench"          text = 'Schema Util'            as = { Link } to = { `/util/schema` }/>
                            <Dropdown.Item icon = "lock"            text = 'Change Password'        onClick = {() => { setChangePasswordModalOpen ( true )}}/>
                            <Dropdown.Item icon = "warning circle"  text = 'Delete Local Storage'   onClick = {() => { appState.deleteStorage ()}}/>
                        </Dropdown.Menu>
                    </Dropdown>
                </Menu.Menu>
            </Menu>

            <ChangePasswordModal
                appState    = { appState }
                open        = { changePasswordModalOpen }
                onClose     = {() => { setChangePasswordModalOpen ( false )}}
            />

        </React.Fragment>
    );
});

