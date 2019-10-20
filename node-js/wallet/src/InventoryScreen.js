/* eslint-disable no-whitespace-before-property */

import './InventoryScreen.css';

import { AssetView }                                        from './AssetView';
import { NavigationBar }                                    from './NavigationBar';
import { AppStateService }                                  from './AppStateService';
import { Service, useService }                              from './Service';
import { InventoryService }                                 from './InventoryService';
import * as util                                            from './util/util';
import { InventoryView, INVENTORY_LAYOUT, getInventoryLayoutFriendlyName } from './InventoryView';
import { action, computed, extendObservable, observable }   from "mobx";
import { observer }                                         from 'mobx-react';
import React, { useState }                                  from 'react';
import { Link }                                             from 'react-router-dom';
import { Dropdown, Grid, Icon, List, Menu, Loader }         from 'semantic-ui-react';
import { SingleColumnContainerView }                        from './SingleColumnContainerView';

const SORT_MODE = {
    ALPHA_ATOZ:     'ALPHA_ATOZ',
    ALPHA_ZTOA:     'ALPHA_ZTOA',
};

//================================================================//
// InventoryScreenController
//================================================================//
class InventoryScreenController extends Service {

    //----------------------------------------------------------------//
    compareForSort ( asset0, asset1 ) {

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
            layoutMode:     INVENTORY_LAYOUT.WEB,
            sortMode:       SORT_MODE.ALPHA_ATOZ,
        });
    }

    //----------------------------------------------------------------//
    @computed
    get sortedAssets () {

        let assetArray = this.inventory.availableAssetsArray;
        assetArray.sort (( asset0, asset1 ) => this.compareForSort ( asset0, asset1 ));
        return assetArray;
    }

    //----------------------------------------------------------------//
    @action
    setLayoutMode ( layoutMode ) {

        this.layoutMode = layoutMode;
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

    const onSortItemClick = ( event, { name }) => {
        controller.setSortMode ( name );
    }

    let methodListItems = [];
    const methodBindings = controller.inventory.getCraftingMethodBindings ();
    for ( let methodName in methodBindings ) {
        const binding = methodBindings [ methodName ];
        const disabled = !binding.valid;
        
        methodListItems.push (<Dropdown.Item
            key = { methodName }
            text = { methodName }
            disabled = { disabled }
            as = { Link }
            to = { appState.prefixURL ( `/accounts/${ appState.accountID }/crafting/${ methodName }` )}
        />);
    }

    const layoutDropdown = [
        INVENTORY_LAYOUT.WEB,
        INVENTORY_LAYOUT.US_LETTER,
        INVENTORY_LAYOUT.US_LEGAL,
        INVENTORY_LAYOUT.US_LEDGER,
        INVENTORY_LAYOUT.A4,
        INVENTORY_LAYOUT.A3,
    ];

    let layoutOptions = [];
    for ( let i in layoutDropdown ) {
        const layoutMode = layoutDropdown [ i ];
        layoutOptions.push (
            <Dropdown.Item
                key         = { layoutMode }
                text        = { getInventoryLayoutFriendlyName ( layoutMode )}
                onClick     = {() => { controller.setLayoutMode ( layoutMode )}}
            />
        );
    }

    return (
        <Menu>
            <Menu.Item name = { SORT_MODE.ALPHA_ATOZ } active = { controller.sortMode === SORT_MODE.ALPHA_ATOZ } onClick = { onSortItemClick }>
                <Icon name = 'sort alphabet up'/>
            </Menu.Item>

            <Menu.Item name = { SORT_MODE.ALPHA_ZTOA } active = { controller.sortMode === SORT_MODE.ALPHA_ZTOA } onClick = { onSortItemClick }>
                <Icon name = 'sort alphabet down'/>
            </Menu.Item>
            
            <Menu.Item name = "Print" onClick = {() => { window.print ()}}>
                <Icon name = 'print'/>
            </Menu.Item>

            <Dropdown item text = { getInventoryLayoutFriendlyName ( controller.layoutMode )}>
                <Dropdown.Menu>
                { layoutOptions }
                </Dropdown.Menu>
            </Dropdown>

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
// InventoryScreen
//================================================================//
export const InventoryScreen = observer (( props ) => {

    const accountIDFromEndpoint = util.getMatch ( props, 'accountID' );

    const [ progressMessage, setProgressMessage ] = useState ( '' );
    const appState      = useService (() => new AppStateService ( util.getMatch ( props, 'userID' ), accountIDFromEndpoint ));
    const inventory     = useService (() => new InventoryService ( appState, setProgressMessage ));
    const controller    = useService (() => new InventoryScreenController ( inventory ));

    if ( appState.accountID !== accountIDFromEndpoint ) {
        //TODO 404 error (need make 404 screen)
        return appState.redirect ( `/accounts/${ appState.accountID }/inventory` );
    }

    //TODO elegant loading screen
    if ( inventory.loading === true ) {
        return (
            <div>
                <Loader active inline='centered' size='massive' style={{marginTop:'5%'}}>{ progressMessage }</Loader>
            </div>
        );
    }

    return (
        <div>
            <div className = "no-print">
                <SingleColumnContainerView>
                    <NavigationBar navTitle = "Inventory" appState = { appState }/>
                    <InventoryFilterMenu appState = { appState } controller = { controller }/>
                </SingleColumnContainerView>
            </div>
            <If condition = { inventory.loading === false }>
                <InventoryView
                    key         = { controller.sortMode }
                    inventory   = { controller.inventory }
                    assetArray  = { controller.sortedAssets }
                    layout      = { controller.layoutMode }
                />
            </If>
        </div>
    );
});
