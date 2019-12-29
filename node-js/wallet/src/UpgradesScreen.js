/* eslint-disable no-whitespace-before-property */

import { InventoryFilterDropdown }                          from './InventoryFilterDropdown';
import { InventoryTagController }                           from './InventoryTagController';
import { InventoryTagDropdown }                             from './InventoryTagDropdown';
import { NavigationBar }                                    from './NavigationBar';
import { AppStateService }                                  from './AppStateService';
import { Transaction, TRANSACTION_TYPE }                    from './Transaction';
import { AssetModal, AssetTagsModal, inventoryMenuItems, InventoryService, InventoryViewController, InventoryPrintView, InventoryView } from 'cardmotron';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import _                                                    from 'lodash';
import { action, computed, extendObservable, observable }   from "mobx";
import { observer }                                         from 'mobx-react';
import React, { useState }                                  from 'react';
import { Link }                                             from 'react-router-dom';
import { Button, Checkbox, Dropdown, Grid, Icon, Label, List, Menu, Loader, Segment, Table } from 'semantic-ui-react';

//================================================================//
// UpgradesController
//================================================================//
export class UpgradesController {

    @observable upgrades = false;

    //----------------------------------------------------------------//
    @action
    affirm ( inventory, appState ) {

        if ( this.upgrades ) return;

        const assetsUtilized = appState ? appState.assetsUtilized : [];

        this.inventory = inventory;

        const assets = inventory.availableAssetsArray;
        if ( !assets.length ) return;

        const upgrades = [];

        for ( let asset of assets ) {

            if ( assetsUtilized.includes ( asset.assetID )) continue;

            const forAsset = inventory.getUpgradesForAssetID ( asset.assetID );
            if ( forAsset ) {
                upgrades.push ({
                    asset:      asset,
                    assetID:    asset.assetID,
                    selected:   forAsset [ forAsset.length - 1 ],
                    options:    forAsset,
                    enabled:    true,
                });
            }
        }
        this.upgrades = upgrades;
    }

    //----------------------------------------------------------------//
    @action
    clear () {

        this.upgrades = false;
    }

    //----------------------------------------------------------------//
    constructor () {
    }

    //----------------------------------------------------------------//
    @action
    enableAll ( enabled ) {

        for ( let upgrade of this.upgrades ) {
            upgrade.enabled = enabled;
        }
    }

    //----------------------------------------------------------------//
    finalize () {
    }

    //----------------------------------------------------------------//
    getFriendlyName ( option ) {

        if ( this.inventory ) {
            const definition = this.inventory.schema.definitions [ option ];
            return definition.fields.name ? definition.fields.name.value : option;
        }
        return option;
    }

    //----------------------------------------------------------------//
    @computed
    get total () {

        return this.upgrades ? this.upgrades.length : 0;
    }

    //----------------------------------------------------------------//
    @computed
    get totalEnabled () {

        if ( !this.upgrades ) return 0;
        
        let enabled = 0;
        for ( let upgrade of this.upgrades ) {
            if ( upgrade.enabled ) {
                ++enabled;
            }
        }
        return enabled;
    }

    //----------------------------------------------------------------//
    @computed
    get upgradeMap () {

        const map = {};
        for ( let upgrade of this.upgrades ) {
            if ( upgrade.enabled ) {
                map [ upgrade.assetID ] = upgrade.selected;
            }
        }
        return map;
    }

    //----------------------------------------------------------------//
    @action
    select ( upgradeID, option ) {

        this.upgrades [ upgradeID ].selected = option;
    }

    //----------------------------------------------------------------//
    @action
    toggle ( upgradeID ) {

        this.upgrades [ upgradeID ].enabled = !this.upgrades [ upgradeID ].enabled;
    }
};

//================================================================//
// UpgradeItem
//================================================================//
const UpgradeItem = observer (( props ) => {

    const { controller, upgradeID } = props;

    const upgrade = controller.upgrades [ upgradeID ];

    const options = [];
    for ( let option of upgrade.options ) {

        options.push (
            <Dropdown.Item
                key         = { option }
                text        = { controller.getFriendlyName ( option )}
                onClick     = {() => { controller.select ( upgradeID, option )}}
            />
        );
    }

    const name = upgrade.asset.fields.name ? upgrade.asset.fields.name.value : upgrade.assetID;

    const toggle = ( event ) => {
        event.stopPropagation ();
        controller.toggle ( upgradeID );
    }

    return (
        <Table.Row>
            <Table.Cell
                collapsing
                onClick             = { toggle }
            >
                { name }
            </Table.Cell>
            <Table.Cell>
                <Dropdown
                    fluid
                    selection
                    text = { controller.getFriendlyName ( upgrade.selected )}
                    disabled = { !upgrade.enabled }
                    options = { options }
                />
            </Table.Cell>
            <Table.Cell collapsing>
                <Checkbox
                    checked         = { upgrade.enabled }
                    onChange        = { toggle }
                />
            </Table.Cell>
        </Table.Row>
    );
});

//================================================================//
// UpgradesScreen
//================================================================//
export const UpgradesScreen = observer (( props ) => {

    const userIDFromEndpoint        = util.getMatch ( props, 'userID' );
    const accountIDFromEndpoint     = util.getMatch ( props, 'accountID' );

    const [ progressMessage, setProgressMessage ]   = useState ( '' );
    const [ upgradeTable, setUpgradeTable ]         = useState ( false );
    const appState      = hooks.useFinalizable (() => new AppStateService ( userIDFromEndpoint, accountIDFromEndpoint ));
    const inventory     = hooks.useFinalizable (() => new InventoryService ( setProgressMessage, appState.node, appState.accountID ));
    const controller    = hooks.useFinalizable (() => new UpgradesController ());

    if ( appState.accountID !== accountIDFromEndpoint ) {
        //TODO 404 error (need make 404 screen)
        return appState.redirect ( `/accounts/${ appState.accountID }/inventory` );
    }

    const hasAssets = (( inventory.loading === false ) && ( inventory.availableAssetsArray.length > 0 ));

    if ( hasAssets ) {
        controller.affirm ( inventory, appState );
    }

    const upgradeList = [];
    for ( let i in controller.upgrades ) {
        upgradeList.push (
            <UpgradeItem
                key = { i }
                controller = { controller }
                upgradeID = { i }
            />
        );
    }

    const onSubmit = () => {

        const upgradeMap = controller.upgradeMap;

        const body = {
            maker: {
                accountName:    appState.accountID,
                keyName:        appState.keyName,
                nonce:          -1,
                gratuity:       0,
            },
            upgrades:   upgradeMap,
        }

        const transaction = new Transaction ( TRANSACTION_TYPE.UPGRADE_ASSETS, body );
        transaction.setAssetsUtilized ( Object.keys ( upgradeMap ));
        appState.pushTransaction ( transaction );

        controller.clear ();
    }

    return (
        <React.Fragment>

            <SingleColumnContainerView>
                <NavigationBar navTitle = "Upgrades" appState = { appState }/>
            
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

                    <When condition = { controller.total > 0 }>

                        <Table celled unstackable>
                            <Table.Header>
                                <Table.Row>
                                    <Table.HeaderCell>Name</Table.HeaderCell>
                                    <Table.HeaderCell>Upgrade</Table.HeaderCell>
                                    <Table.HeaderCell/>
                                </Table.Row>
                            </Table.Header>

                            <Table.Body>
                                { upgradeList }
                            </Table.Body>

                            <Table.Footer fullWidth>
                                <Table.Row>
            
                                    <Table.HeaderCell colSpan='4'>
                                        <Button
                                            floated = 'right'
                                            primary
                                            disabled = { controller.totalEnabled === 0 }
                                            onClick = { onSubmit }
                                        >
                                            Submit
                                        </Button>

                                        <Button
                                            color = 'teal'
                                            disabled = { controller.totalEnabled === controller.total }
                                            onClick = {() => { controller.enableAll ( true )}}
                                        >
                                            Select All
                                        </Button>
                                        
                                        <Button
                                            color = 'red'
                                            disabled = { controller.totalEnabled === 0 }
                                            onClick = {() => { controller.enableAll ( false )}}
                                        >
                                            Deselect All
                                        </Button>
                                    </Table.HeaderCell>
                                </Table.Row>
                            </Table.Footer>
                        </Table>

                    </When>
                </Choose>
            </SingleColumnContainerView>
        </React.Fragment>
    );
});
