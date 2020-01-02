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

    const { appState } = props;
    const networkID = props.networkID || '';
    const accountID = props.accountID || '';
    const accountTab = props.accountTab || '';
    const networkTab = props.networkTab || '';

    const [ redirect, setRedirect ] = useState ( false );

    if ( redirect ) {
        const temp = redirect;
        setRedirect ( false );
        return (<Redirect to = { temp }/>); 
    }

    const networks = {
        network0:       [ '9090', '9091', '9092' ],
        network1:       [ '9093', '9094' ],
        network2:       [ '9095', '9096', '9097' ],
    };

    const accountDropdown = [];
    if ( networkID.length > 0 ) {
        const accounts = networks [ networkID ];
        for ( let account of accounts ) {
            accountDropdown.push (
                <Dropdown.Item
                    key         = { account }
                    as          = { Link }
                    to          = { `/net/${ networkID }/account/${ account }/${ accountTab }` }
                >
                    { account }
                </Dropdown.Item>
            );
        }
    }

    let onClickLogout = () => { appState.login ( false )};

    const networkDropdown = [];
    for ( let network in networks ) {

        networkDropdown.push (
            <Dropdown.Item
                key         = { network }
                as          = { Link }
                to          = { `/net/${ network }/${ networkTab }` }
            >
                <span className='text'>{ network }</span>
            </Dropdown.Item>
        );
    }

    return (
        <Menu attached = 'top' borderless inverted>

            <Menu.Item
                icon = 'globe'
                as          = { Link }
                to          = { `/` }
            />

            <Dropdown
                item
                text = { networkID }
                placeholder = '--'
            >
                <Dropdown.Menu>
                    { networkDropdown }
                </Dropdown.Menu>
            </Dropdown>

            <If condition = { accountDropdown.length > 0 }>
                <Dropdown
                    item
                    text = { accountID }
                    placeholder = '--'
                >
                    <Dropdown.Menu>
                        { accountDropdown }
                    </Dropdown.Menu>
                </Dropdown>

            </If>

            <Menu.Menu position = "right">
                <Menu.Item
                    icon = 'power off'
                    onClick = {() => { onClickLogout ()}}
                />
            </Menu.Menu>
        </Menu>
    );
});
