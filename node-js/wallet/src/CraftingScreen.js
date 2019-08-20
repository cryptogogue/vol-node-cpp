/* eslint-disable no-whitespace-before-property */

import AssetView                                            from './AssetView';
import CraftingForm                                         from './CraftingForm';
import { AccountInfoService }                               from './AccountInfoService';
import { AppStateService }                                  from './AppStateService';
import { Service, useService }                              from './Service';
import { InventoryService }                                 from './InventoryService';
import * as util                                            from './util/util';
import { action, computed, extendObservable, observable }   from "mobx";
import { observer }                                         from 'mobx-react';
import NavigationBar                                        from './NavigationBar';
import React, { useState }                                  from 'react';
import { Dropdown, Grid, Icon, List, Menu }                 from 'semantic-ui-react';

//================================================================//
// CraftingScreen
//================================================================//
const CraftingScreen = observer (( props ) => {

    const accountIdFromEndpoint = util.getMatch ( props, 'accountId' );
    const methodNameFromEndpoint = util.getMatch ( props, 'methodName' );

    const appState              = useService (() => new AppStateService ( util.getMatch ( props, 'userId' ), accountIdFromEndpoint ));
    const accountInfoService    = useService (() => new AccountInfoService ( appState ));
    const inventory             = useService (() => new InventoryService ( appState ));

    const [ selectedMethod, setSelectedMethod ] = useState ( methodNameFromEndpoint );

    if (( appState.accountId !== accountIdFromEndpoint ) || ( selectedMethod !== methodNameFromEndpoint )) {
        return appState.redirect ( `/accounts/${ appState.accountId }/crafting/${ selectedMethod }` );
    }

    if ( inventory.loading === true ) {
        return (<div>{ 'LOADING' }</div>);
    }

    const isValid = inventory.methodIsValid ( methodNameFromEndpoint );
    if (( methodNameFromEndpoint !== '' ) && !isValid ) {
        setSelectedMethod ( '' );
    }

    let dropdownOptions = [];

    const methodBindings = inventory.getCraftingMethodBindings ();
    for ( let methodName in methodBindings ) {
        const binding = methodBindings [ methodName ];
        const disabled = !binding.valid;

        dropdownOptions.push ({ key:methodName, value:methodName, text:methodName, disabled: disabled });
    }

    const onSelectionChanged = ( methodName ) => {
        setSelectedMethod ( methodName );
    }

    return (
        <div>
            <NavigationBar navTitle = "Crafting" appState = { appState }/>

            <div>
                <Dropdown
                    placeholder = "Select Method"
                    fluid
                    search
                    selection
                    options = { dropdownOptions }
                    onChange = {( event, data ) => { onSelectionChanged ( data.value )}}
                    defaultValue = { methodNameFromEndpoint !== '' ? methodNameFromEndpoint : null }
                />
            </div>

            <If condition = { isValid }>
                <CraftingForm
                    appState    = { appState }
                    inventory   = { inventory }
                    methodName  = { methodNameFromEndpoint }
                />
            </If>
        </div>
    );
});

export default CraftingScreen;