/* eslint-disable no-whitespace-before-property */

import './InventoryScreen.css';

import { NavigationBar }                                    from './NavigationBar';
import { AppStateService }                                  from './AppStateService';
import { AssetModal, AssetTagsModal, inventoryMenuItems, InventoryService, InventoryViewController, InventoryPrintView, InventoryView } from 'cardmotron';
import { assert, excel, hooks, Service, SingleColumnContainerView, util } from 'fgc';
import _                                                    from 'lodash';
import { action, computed, extendObservable, observable }   from "mobx";
import { observer }                                         from 'mobx-react';
import React, { useState }                                  from 'react';
import { Link }                                             from 'react-router-dom';
import { Dropdown, Grid, Icon, List, Menu, Loader }         from 'semantic-ui-react';

//================================================================//
// InventoryMenu
//================================================================//
const InventoryMenu = observer (( props ) => {

    const { appState, controller } = props;

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

    return (
        <Menu>
            <inventoryMenuItems.SortModeFragment        controller = { controller }/>
            <inventoryMenuItems.LayoutOptionsDropdown   controller = { controller }/>
            <inventoryMenuItems.ZoomOptionsDropdown     controller = { controller }/>
            <inventoryMenuItems.SelectionTags           controller = { controller }/>
            <inventoryMenuItems.VisibilityDropdown      controller = { controller }/>

            <Menu.Menu position = "right">

                <If condition = { controller.isPrintLayout }>
                    <Menu.Item name = "Print" onClick = {() => { window.print ()}}>
                        <Icon name = 'print'/>
                    </Menu.Item>
                </If>

                <If condition = { !controller.isPrintLayout }>
                    <Dropdown item icon = "industry">
                        <Dropdown.Menu>
                            { methodListItems }
                        </Dropdown.Menu>
                    </Dropdown>
                </If>
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
    const [ zoomedAssetID, setZoomedAssetID ] = useState ( false );
    const appState      = hooks.useFinalizable (() => new AppStateService ( util.getMatch ( props, 'userID' ), accountIDFromEndpoint ));
    const inventory     = hooks.useFinalizable (() => new InventoryService ( setProgressMessage, appState.node, appState.accountID ));
    const controller    = hooks.useFinalizable (() => new InventoryViewController ( inventory ));

    if ( appState.accountID !== accountIDFromEndpoint ) {
        //TODO 404 error (need make 404 screen)
        return appState.redirect ( `/accounts/${ appState.accountID }/inventory` );
    }

    const onAssetSelect = ( asset ) => {
        controller.toggleAssetSelection ( asset );
    }

    const onAssetMagnify = ( asset ) => {
        setZoomedAssetID ( asset.assetID );
    }

    const onAssetEllipsis = ( asset ) => {
        console.log ( 'ELLIPSIS!' );
    }

    const onDeselect = () => {
        controller.clearSelection ();
    }

    const hasAssets = (( inventory.loading === false ) && ( inventory.availableAssetsArray.length > 0 ));

    return (
        <div style = {{
            display: 'flex',
            flexFlow: 'column',
            height: '100vh',
        }}>
            <div className = "no-print">
                <SingleColumnContainerView>
                    <NavigationBar navTitle = "Inventory" appState = { appState }/>
                    <InventoryMenu appState = { appState } controller = { controller }/>
                </SingleColumnContainerView>
            </div>

            <Choose>

                <When condition = { inventory.loading }>
                    <Loader
                        active
                        inline = 'centered'
                        size = 'massive'
                        style = {{ marginTop:'5%' }}
                    >
                        { progressMessage }
                    </Loader>
                </When>

                <When condition = { hasAssets }>
                    <Choose>
                        <When condition = { controller.isPrintLayout }>
                            <InventoryPrintView
                                key             = { controller.sortMode }
                                inventory       = { controller.inventory }
                                assetArray      = { controller.sortedAssets }
                                layoutName      = { controller.layoutName }
                            />
                        </When>
                        <Otherwise>
                            <div style = {{ flex: 1 }}>
                                <InventoryView
                                    key         = { `${ controller.sortMode } ${ controller.zoom }` }
                                    controller  = { controller }
                                    onSelect    = { onAssetSelect }
                                    onMagnify   = { onAssetMagnify }
                                    onEllipsis  = { onAssetEllipsis }
                                    onDeselect  = { onDeselect }
                                />
                            </div>
                            <AssetModal
                                inventory       = { controller.inventory }
                                assetID         = { zoomedAssetID }
                                onClose         = {() => { setZoomedAssetID ( false )}}
                            />
                        </Otherwise>
                    </Choose>
                </When>

                <Otherwise>
                    <img 
                        src = { './cardmotron.png' }
                        style = {{
                            width: '100%',
                            height: 'auto',
                        }}
                    />
                </Otherwise>

            </Choose>
        </div>
    );
});
