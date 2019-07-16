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
            methodName:     '',
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
    isCrafting () {

        return this.methodName.length > 0;
    }

    //----------------------------------------------------------------//
    @action
    setMethodName ( methodName ) {

        const methodBindings = this.inventory.getCraftingMethodBindings ();
        this.methodName = ( methodName && ( methodName in methodBindings ) && methodBindings [ methodName ].valid ) ? methodName : '';
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

    const controller = props.controller;

    const onSortItemClick = ( event, { name }) => controller.setSortMode ( name );

    let methodListItems = [];
    const methodBindings = controller.inventory.getCraftingMethodBindings ();
    for ( let methodName in methodBindings ) {
        const binding = methodBindings [ methodName ];
        const disabled = !binding.valid;
        
        console.log ( 'METHOD', methodName );

        //const name = `${ methodName }: ${ binding.valid ? 'OK' : 'nope' }`;
        // methodListItems.push (<List.Item key = { methodName }>{ name }</List.Item>);
        methodListItems.push (<Dropdown.Item
            key = { methodName }
            text = { methodName }
            disabled = { disabled }
            onClick = {() => controller.setMethodName ( methodName )}
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
            <InventoryFilterMenu controller = { controller }/>
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

    // controller.setMethodName ( 'makeRare' );

    if ( appState.accountId !== accountIdFromEndpoint ) {
        return appState.redirect ( `/accounts/${ appState.accountId }/inventory` );
    }

    if ( inventory.loading === true ) {
        return (<div>{ 'LOADING' }</div>);
    }
    
    const handleSubmit = ( fieldValues ) => {
        // fieldValues.makerNonce = this.props.nonce;
        // this.showForm ( false );
        // this.props.appState.startTransaction ( schema, fieldValues );
    }

    return (
        <div>
            <NavigationBar navTitle = "Inventory" appState = { appState }/>
            <Choose>
                <When condition = { controller.isCrafting ()}>
                    <CraftingForm
                        key         = { controller.methodName }
                        methodName  = { controller.methodName }
                        inventory   = { inventory }
                        onSubmit    = {( fieldValues ) => { handleSubmit ( fieldValues )}}
                    /> 
                </When>
                <Otherwise>
                    <InventoryView controller = { controller }/>
                </Otherwise>
            </Choose>
        </div>
    );
});

export default InventoryScreen;