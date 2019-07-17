/* eslint-disable no-whitespace-before-property */

import AssetView                                            from './AssetView';
import CraftingForm                                         from './CraftingForm';
import { AccountInfoService }                               from './stores/AccountInfoService';
import { AppStateService }                                  from './stores/AppStateService';
import { Service, useService }                              from './stores/Service';
import { InventoryService }                                 from './stores/InventoryService';
import * as util                                            from './util/util';
import { action, computed, extendObservable, observable }   from "mobx";
import { observer }                                         from 'mobx-react';
import NavigationBar                                        from './NavigationBar';
import React, { useState }                                  from 'react';
import { Dropdown, Grid, Icon, List, Menu }                 from 'semantic-ui-react';

//================================================================//
// CraftingFormSelector
//================================================================//
const CraftingFormSelector = observer (( props ) => {

    const { inventory, methodName, onChanged } = props;

    let options = [];

    const methods = inventory.validMethods;
    for ( let i in methods ) {
        options.push ({ key:i, value:methods [ i ], text:methods [ i ]});
    }

    return (
        <div>
            <Dropdown
                placeholder = "Select Method"
                fluid
                search
                selection
                options = { options }
                onChange = {( event, data ) => { onChanged ( data.value )}}
                defaultValue = { methodName !== '' ? methodName : null }
            />
        </div>
    );
});

//================================================================//
// CraftingScreen
//================================================================//
const CraftingScreen = observer (( props ) => {

    const accountIdFromEndpoint = util.getMatch ( props, 'accountId' );
    const methodNameFromEndpoint = util.getMatch ( props, 'methodName' );

    const appState              = useService (() => new AppStateService ( util.getMatch ( props, 'userId' ), accountIdFromEndpoint ));
    const accountInfoService    = useService (() => new AccountInfoService ( appState ));
    const inventory             = useService (() => new InventoryService ());

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
    
    const handleSubmit = ( transaction ) => {
        // fieldValues.makerNonce = this.props.nonce;
        // this.showForm ( false );
        // this.props.appState.startTransaction ( schema, fieldValues );
    }

    const onSelectionChanged = ( methodName ) => {
        setSelectedMethod ( methodName );
    }

    return (
        <div>
            <NavigationBar navTitle = "Crafting" appState = { appState }/>

            <CraftingFormSelector inventory = { inventory } methodName = { methodNameFromEndpoint } onChanged = { onSelectionChanged }/>

            <If condition = { isValid }>
                <CraftingForm
                    appState    = { appState }
                    inventory   = { inventory }
                    methodName  = { methodNameFromEndpoint }
                    onSubmit    = {( transaction ) => { handleSubmit ( transaction )}}
                />
            </If>
        </div>
    );
});

export default CraftingScreen;