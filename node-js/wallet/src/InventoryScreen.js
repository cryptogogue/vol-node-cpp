/* eslint-disable no-whitespace-before-property */

import './InventoryScreen.css';

import { AssetView }                                        from './AssetView';
import { NavigationBar }                                    from './NavigationBar';
import { AppStateService }                                  from './AppStateService';
import { Service, useService }                              from './Service';
import { InventoryService }                                 from './InventoryService';
import * as util                                            from './util/util';
import { InventoryPrintView, PRINT_LAYOUT }                 from './InventoryPrintView';
import { InventoryView }                                    from './InventoryView';
import _                                                    from 'lodash';
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

const WEB_LAYOUT = 'WEB';

const LAYOUT_DROPDOWN_OPTIONS = [
        WEB_LAYOUT,
        PRINT_LAYOUT.US_LETTER,
        PRINT_LAYOUT.US_LEGAL,
        PRINT_LAYOUT.US_LEDGER,
        PRINT_LAYOUT.A4,
        PRINT_LAYOUT.A3,
    ];

function getLayoutFriendlyName ( layoutName ) {

    switch ( layoutName ) {
        case WEB_LAYOUT:                return 'Web';
        case PRINT_LAYOUT.US_LETTER:    return 'US Letter (8.5" x 11")';
        case PRINT_LAYOUT.US_LEGAL:     return 'US Legal (14" x 8.5")';
        case PRINT_LAYOUT.US_LEDGER:    return 'US Ledger (17" x 11")';
        case PRINT_LAYOUT.A4:           return 'A4 (210mm x 297mm)';
        case PRINT_LAYOUT.A3:           return 'A3 (420mm x 297mm)';
        case PRINT_LAYOUT.A2:           return 'A2 (420mm x 594mm)';
    }
}

function isPrintLayout ( layoutName ) {

    return _.has ( PRINT_LAYOUT, layoutName );
}

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
            layoutName:     WEB_LAYOUT,
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
    setLayoutMode ( layoutName ) {

        this.layoutName = layoutName;
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

    let layoutOptions = [];
    for ( let i in LAYOUT_DROPDOWN_OPTIONS ) {
        const layoutName = LAYOUT_DROPDOWN_OPTIONS [ i ];
        layoutOptions.push (
            <Dropdown.Item
                key         = { layoutName }
                text        = { getLayoutFriendlyName ( layoutName )}
                onClick     = {() => { controller.setLayoutMode ( layoutName )}}
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

            <Dropdown item text = { getLayoutFriendlyName ( controller.layoutName )}>
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
        <div style = {{
            display: 'flex',
            flexFlow: 'column',
            height: '100vh',
        }}>
            <div className = "no-print">
                <SingleColumnContainerView>
                    <NavigationBar navTitle = "Inventory" appState = { appState }/>
                    <InventoryFilterMenu appState = { appState } controller = { controller }/>
                </SingleColumnContainerView>
            </div>

            <If condition = { inventory.loading === false }>
                <Choose>
                    <When condition = { isPrintLayout ( controller.layoutName )}>
                        <InventoryPrintView
                            key         = { controller.sortMode }
                            inventory   = { controller.inventory }
                            assetArray  = { controller.sortedAssets }
                            layoutName  = { controller.layoutName }
                        />
                    </When>
                    <Otherwise>
                        <div style = {{ flex: 1 }}>
                            <InventoryView
                                key         = { controller.sortMode }
                                inventory   = { controller.inventory }
                                assetArray  = { controller.sortedAssets }
                            />
                        </div>
                    </Otherwise>
                </Choose>
            </If>
        </div>
    );
});
