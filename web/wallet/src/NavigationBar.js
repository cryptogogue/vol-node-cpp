/* eslint-disable no-whitespace-before-property */

import './NavigationBar.css';

import { withAppState }         from './AppStateProvider';
import React, { Component }     from 'react';
import { Link }                 from 'react-router-dom';
import { Dropdown, Menu }       from 'semantic-ui-react';

//================================================================//
// NavigationBar
//================================================================//
class NavigationBar extends Component {

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.state = {
        };
    }

    //----------------------------------------------------------------//
    logout () {
        this.props.appState.login ( false );
    }
  
    //----------------------------------------------------------------//
    render () {
        
        const { appState, navTitle } = this.props;
        const { activeItem } = this.state;

        let onClickDeleteStorage    = () => { appState.deleteStorage ()};
        let onClickLogout           = () => { this.logout ()};

        return (
            <Menu>
                <Menu.Item
                    icon = "bars"
                    name = ""
                    as = { Link } to = "/"
                    active = { activeItem === 'home' }
                />

                <span className = "large text">{ navTitle }</span>
        
                <Menu.Menu position = "right">
                    <Dropdown item icon = "settings">
                        <Dropdown.Menu>
                            <Dropdown.Item icon = "add square" text = "Create Account" as = { Link } to = '/accounts/new' />
                            <Dropdown.Item icon = "log out" text = "Logout" onClick = { onClickLogout }/>
                            <Dropdown.Item icon = "globe" text = "DEBUG: Delete Local Storage" onClick = { onClickDeleteStorage }/>
                        </Dropdown.Menu>
                    </Dropdown>
                </Menu.Menu>
            </Menu>
        );
    }
}

export default withAppState ( NavigationBar );