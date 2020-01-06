/* eslint-disable no-whitespace-before-property */

import { InventoryFilterDropdown }                          from './InventoryFilterDropdown';
import { InventoryTagController }                           from './InventoryTagController';
import { InventoryTagDropdown }                             from './InventoryTagDropdown';
import { AccountNavigationBar, ACCOUNT_TABS }               from './AccountNavigationBar';
import { AppStateService }                                  from './AppStateService';
import { Transaction, TRANSACTION_TYPE }                    from './Transaction';
import { AssetModal, AssetTagsModal, inventoryMenuItems, InventoryService, InventoryViewController, InventoryPrintView, InventoryView } from 'cardmotron';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';
import _                                                    from 'lodash';
import { action, computed, extendObservable, observable }   from "mobx";
import { observer }                                         from 'mobx-react';
import React, { useState }                                  from 'react';
import { Link }                                             from 'react-router-dom';
import * as UI                                              from 'semantic-ui-react';

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
            upgrade.selected = enabled ? upgrade.options [ upgrade.options.length - 1 ] : upgrade.asset.type;
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
    @action
    isEnabled ( upgradeID ) {

        const upgrade = this.upgrades [ upgradeID ];
        return ( upgrade.asset.type !== upgrade.selected );
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
            if ( upgrade.asset.type !== upgrade.selected ) {
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
            if ( upgrade.asset.type !== upgrade.selected ) {
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

        const upgrade = this.upgrades [ upgradeID ];
        const max = upgrade.options [ upgrade.options.length - 1 ];

        upgrade.selected = ( upgrade.asset.type === upgrade.selected ) ? max : upgrade.asset.type;
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
            <UI.Dropdown.Item
                key         = { option }
                text        = { controller.getFriendlyName ( option )}
                onClick     = {() => { controller.select ( upgradeID, option )}}
            />
        );
    }

    const toggle = () => { controller.toggle ( upgradeID )}

    const name = upgrade.asset.fields.name ? upgrade.asset.fields.name.value : upgrade.assetID;
    const enabled = controller.isEnabled ( upgradeID );

    return (
        <UI.Table.Row
            positive = { enabled }
            negative = { !enabled }
        >
            <UI.Table.Cell
                collapsing
                onClick = { toggle }
            >
                { name }
            </UI.Table.Cell>
            <UI.Table.Cell>
                <UI.Dropdown
                    fluid
                    selection
                    text = { controller.getFriendlyName ( upgrade.selected )}
                    options = { options }
                />
            </UI.Table.Cell>
            <UI.Table.Cell
                collapsing
                onClick = { toggle }
            >
                <Choose>
                    <When condition = { enabled }>
                        <UI.Icon name = 'check'/>
                    </When>
                    <Otherwise>
                        <UI.Icon name = 'dont'/>
                    </Otherwise>
                </Choose>
            </UI.Table.Cell>
        </UI.Table.Row>
    );
});

//================================================================//
// UpgradesScreen
//================================================================//
export const UpgradesScreen = observer (( props ) => {

    const networkIDFromEndpoint = util.getMatch ( props, 'networkID' );
    const accountIDFromEndpoint = util.getMatch ( props, 'accountID' );

    const [ progressMessage, setProgressMessage ]   = useState ( '' );
    const [ upgradeTable, setUpgradeTable ]         = useState ( false );
    const appState      = hooks.useFinalizable (() => new AppStateService ( networkIDFromEndpoint, accountIDFromEndpoint ));
    const inventory     = hooks.useFinalizable (() => new InventoryService ( setProgressMessage, appState.network.nodeURL, appState.accountID ));
    const controller    = hooks.useFinalizable (() => new UpgradesController ());

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

                <AccountNavigationBar
                    appState    = { appState }
                    tab         = { ACCOUNT_TABS.UPGRADES }
                />

                <Choose>
                    <When condition = { inventory.loading }>
                        <UI.Loader
                            active
                            inline = 'centered'
                            size = 'massive'
                            style = {{ marginTop:'5%' }}
                        >
                            { progressMessage }
                        </UI.Loader>
                    </When>

                    <When condition = { controller.total > 0 }>

                        <UI.Table celled unstackable>
                            <UI.Table.Header>
                                <UI.Table.Row>
                                    <UI.Table.HeaderCell>Name</UI.Table.HeaderCell>
                                    <UI.Table.HeaderCell>Upgrade</UI.Table.HeaderCell>
                                    <UI.Table.HeaderCell/>
                                </UI.Table.Row>
                            </UI.Table.Header>

                            <UI.Table.Body>
                                { upgradeList }
                            </UI.Table.Body>

                            <UI.Table.Footer fullWidth>
                                <UI.Table.Row>
            
                                    <UI.Table.HeaderCell colSpan='4'>
                                        <UI.Button
                                            floated = 'right'
                                            primary
                                            disabled = { controller.totalEnabled === 0 }
                                            onClick = { onSubmit }
                                        >
                                            Submit
                                        </UI.Button>

                                        <UI.Button
                                            color = 'teal'
                                            disabled = { controller.totalEnabled === controller.total }
                                            onClick = {() => { controller.enableAll ( true )}}
                                        >
                                            Select All
                                        </UI.Button>
                                        
                                        <UI.Button
                                            color = 'red'
                                            disabled = { controller.totalEnabled === 0 }
                                            onClick = {() => { controller.enableAll ( false )}}
                                        >
                                            Deselect All
                                        </UI.Button>
                                    </UI.Table.HeaderCell>
                                </UI.Table.Row>
                            </UI.Table.Footer>
                        </UI.Table>

                    </When>
                </Choose>
            </SingleColumnContainerView>
        </React.Fragment>
    );
});
