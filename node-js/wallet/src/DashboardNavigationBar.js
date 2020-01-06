/* eslint-disable no-whitespace-before-property */

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

    let onClickDeleteLocalStorage           = () => { appState.deleteStorage ()};

    const previewSchemaURL      = `/util/schema`;

    return (
        <React.Fragment>
            <NavigationBar
                appState    = { appState }
            />

            <Menu borderless attached = 'bottom'>

                <Menu.Menu position = 'right'>
                    <Dropdown item icon = "settings">
                        <Dropdown.Menu>
                            <Dropdown.Item icon = "wrench"          text = 'Schema Util'            as = { Link } to = { previewSchemaURL }/>
                            <Dropdown.Item icon = "warning circle"  text = 'Delete Local Storage'   onClick = { onClickDeleteLocalStorage }/>
                        </Dropdown.Menu>
                    </Dropdown>
                </Menu.Menu>
            </Menu>
        </React.Fragment>
    );
});

