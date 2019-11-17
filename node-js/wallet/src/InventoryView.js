/* eslint-disable no-whitespace-before-property */

import { AssetView }                                        from './AssetView';
import { InventoryService }                                 from './InventoryService';
import { NavigationBar }                                    from './NavigationBar';
import { Service, useService }                              from './Service';
import * as util                                            from './util/util';
import handlebars                                           from 'handlebars';
import { action, computed, extendObservable, observable }   from 'mobx';
import { observer }                                         from 'mobx-react';
import React, { useState }                                  from 'react';
import { Link }                                             from 'react-router-dom';
import { Dropdown, Grid, Icon, List, Menu, Card, Group, Modal, Divider } from 'semantic-ui-react';
import { is }                                               from '@babel/types';
import { InfiniteScrollView }                               from './InfiniteScrollView';

//================================================================//
// InventoryView
//================================================================//
export const InventoryView = observer (( props ) => {

    const [ selection, setSelection ]   = useState ({});

    const isSelected = ( asset ) => {
        return selection [ asset.assetID ] || false;
    }

    const inventory     = props.inventory;
    const assetArray    = props.assetArray || inventory.availableAssetsArray;
    const scale         = props.scale || 1;

    const onClickCard = ( asset ) => {
        const newSelection = Object.assign ({}, selection );
        newSelection [ asset.assetID ] = !isSelected ( asset );
        setSelection ( newSelection );
    }

    const assetLayoutCache = [];
    const getAsset = ( i, targetRef ) => {
        
        if ( !assetLayoutCache.includes ( i )) {
            
            const asset = assetArray [ i ];
            const color = isSelected ( asset ) ? 'red' : 'white';

            assetLayoutCache [ i ] = (
                <Card
                    key = { asset.assetID }
                    style = {{ border: `2px solid ${ color }` }}
                    onClick = {() => { onClickCard ( asset )}}
                >
                    <AssetView
                        assetId = { asset.assetID }
                        inventory = { inventory }
                        inches = { true }
                        scale = { scale }
                    />
                    { isSelected ( asset ) &&
                        <Modal style={{ height : 'auto' }} size = "small" open = { isSelected ( asset )}>
                            <Modal.Content>
                                <center>
                                    <h3>Card Info</h3>
                                    <Divider/>
                                    <AssetView
                                        assetId = { asset.assetID }
                                        inventory = { inventory }
                                        inches = 'true'
                                        scale = '1.3'
                                    />
                                    <p>Asset ID: { asset.assetID }</p>
                                </center>
                            </Modal.Content>
                        </Modal>
                    }
                    <Icon name = 'circle' />
                    <Icon name = 'ellipsis horizontal'/>
                    <Icon name = 'zoom' />
                </Card>
            );
        }
        return assetLayoutCache [ i ];
    }

    return (
        <InfiniteScrollView 
            onGetAsset  = { getAsset }
            totalCards  = { assetArray.length }
        />
    )
});