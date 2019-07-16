/* eslint-disable no-whitespace-before-property */

import AssetView                                            from './AssetView';
import CraftingForm                                         from './CraftingForm';
import { AppStateService }                                  from './stores/AppStateService';
import { Service, useService }                              from './stores/Service';
import { InventoryService }                                 from './stores/InventoryService';
import * as util                                            from './utils/util';
import { action, computed, extendObservable, observable }   from "mobx";
import { observer }                                         from 'mobx-react';
import NavigationBar                                        from './NavigationBar';
import React, { useState }                                  from 'react';
import { Link }                                             from 'react-router-dom';
import { Dropdown, Grid, Icon, List, Menu }                 from 'semantic-ui-react';

const SORT_MODE = {
    ALPHA_ATOZ:     'ALPHA_ATOZ',
    ALPHA_ZTOA:     'ALPHA_ZTOA',
};

//================================================================//
// InventoryController
//================================================================//
class InventoryController extends Service {

    //----------------------------------------------------------------//
    compareForSort ( asset0, asset1 ) {

        console.log ( 'SORT', asset0, asset1 );

        if ( this.sortMode === SORT_MODE.ALPHA_ATOZ ) {
            return asset0.type.localeCompare ( asset1.type );
        }
        return asset1.type.localeCompare ( asset0.type );
    }

    //----------------------------------------------------------------//
    constructor ( inventory ) {
        super ();

        this.inventory = inventory;

        extendObservable ( this, {
            sortMode:       SORT_MODE.ALPHA_ATOZ,
        });
    }

    //----------------------------------------------------------------//
    getSortedAssets () {

        let assetArray = this.inventory.getAssetArray ();
        assetArray.sort (( asset0, asset1 ) => this.compareForSort ( asset0, asset1 ));
        return assetArray;
    }

    //----------------------------------------------------------------//
    @action
    setSortMode ( sortMode ) {

        this.sortMode = sortMode;
    }
}

//================================================================//
// InventoryFilterMenu
//================================================================//
const InventoryFilterMenu = observer (( props ) => {

    const { appState, controller } = props;

    const onSortItemClick = ( event, { name }) => controller.setSortMode ( name );

    let methodListItems = [];
    const methodBindings = controller.inventory.getCraftingMethodBindings ();
    for ( let methodName in methodBindings ) {
        const binding = methodBindings [ methodName ];
        const disabled = !binding.valid;
        
        console.log ( 'METHOD', methodName );

        methodListItems.push (<Dropdown.Item
            key = { methodName }
            text = { methodName }
            disabled = { disabled }
            as = { Link }
            to = { appState.prefixURL ( `/accounts/${ appState.accountId }/crafting/${ methodName }` )}
        />);
    }

    return (
        <Menu>
            <Menu.Item name = { SORT_MODE.ALPHA_ATOZ } active = { controller.sortMode === SORT_MODE.ALPHA_ATOZ } onClick = { onSortItemClick }>
                <Icon name = 'sort alphabet up'/>
            </Menu.Item>
            <Menu.Item name = { SORT_MODE.ALPHA_ZTOA } active = { controller.sortMode === SORT_MODE.ALPHA_ZTOA } onClick = { onSortItemClick }>
                <Icon name = 'sort alphabet down'/>
            </Menu.Item>

            <Menu.Menu position = "right">
                <Dropdown item icon = "industry">
                    <Dropdown.Menu>
                        { methodListItems }
                    </Dropdown.Menu>
                </Dropdown>
            </Menu.Menu>
        </Menu>
    );
});

//================================================================//
// InventoryView
//================================================================//
const InventoryView = observer (( props ) => {

    const controller    = props.controller;
    const inventory     = controller.inventory;

    let assetArray = controller.getSortedAssets ();

    let assetLayouts = [];
    for ( let i in assetArray ) {
        const asset = assetArray [ i ];
        assetLayouts.push (<AssetView key = { asset.assetID } style = {{ float:'left' }} inventory = { inventory } assetId = { asset.assetID }/>);
    }
    
    return (
        <div>
            { assetLayouts }
        </div>
    );
});

//================================================================//
// InventoryScreen
//================================================================//
const InventoryScreen = observer (( props ) => {

    const accountIdFromEndpoint = util.getMatch ( props, 'accountId' );

    const appState      = useService (() => new AppStateService ( util.getMatch ( props, 'userId' ), accountIdFromEndpoint ));
    const inventory     = useService (() => new InventoryService ());
    const controller    = useService (() => new InventoryController ( inventory ));

    if ( appState.accountId !== accountIdFromEndpoint ) {
        return appState.redirect ( `/accounts/${ appState.accountId }/inventory` );
    }

    if ( inventory.loading === true ) {
        return (<div>{ 'LOADING' }</div>);
    }

    return (
        <div>
            <NavigationBar navTitle = "Inventory" appState = { appState }/>
            <InventoryFilterMenu appState = { appState } controller = { controller }/>
            <InventoryView controller = { controller }/>
        </div>
    );
});

export default InventoryScreen;