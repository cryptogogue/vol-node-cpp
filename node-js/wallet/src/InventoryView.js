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
import { Dropdown, Grid, Icon, List, Menu, Card, Group, Modal, Divider }    from 'semantic-ui-react';
import { is } from '@babel/types';
import { InfiniteScrollView }                               from './InfiniteScrollView';

const DPI = 300;
const MM_TO_IN = 0.03937007874;

// TODO: get these dynamically
const ASSET_WIDTH   = 750;
const ASSET_HEIGHT  = 1050;

export const INVENTORY_LAYOUT = {
    WEB:                'WEB',
    US_LETTER:          'US_LETTER',
    US_LEGAL:           'US_LEGAL',
    US_LEDGER:          'US_LEDGER',
    A4:                 'A4',
    A3:                 'A3',
    A2:                 'A2',
};

export function getInventoryLayoutFriendlyName ( layout ) {

    switch ( layout ) {
        case INVENTORY_LAYOUT.WEB:          return 'Web';
        case INVENTORY_LAYOUT.US_LETTER:    return 'US Letter (8.5" x 11")';
        case INVENTORY_LAYOUT.US_LEGAL:     return 'US Legal (14" x 8.5")';
        case INVENTORY_LAYOUT.US_LEDGER:    return 'US Ledger (17" x 11")';

        case INVENTORY_LAYOUT.A4:           return 'A4 (210mm x 297mm)';
        case INVENTORY_LAYOUT.A3:           return 'A3 (420mm x 297mm)';
        case INVENTORY_LAYOUT.A2:           return 'A2 (420mm x 594mm)';
    }
}

function getAssetsPerPageSize ( layout ) {

    switch ( layout ) {
        case INVENTORY_LAYOUT.US_LETTER:    return 9;
        case INVENTORY_LAYOUT.US_LEGAL:     return 10;
        case INVENTORY_LAYOUT.US_LEDGER:    return 18;

        case INVENTORY_LAYOUT.A4:           return 9;
        case INVENTORY_LAYOUT.A3:           return 18;
        case INVENTORY_LAYOUT.A2:           return 36;
    }
    return -1;
}

function getPageDimensions ( layout ) {

    switch ( layout ) {
        case INVENTORY_LAYOUT.US_LETTER:    return { width: 8.5, height: 11 };
        case INVENTORY_LAYOUT.US_LEGAL:     return { width: 14, height: 8.5 };
        case INVENTORY_LAYOUT.US_LEDGER:    return { width: 17, height: 11 };

        case INVENTORY_LAYOUT.A4:           return { width: 210 * MM_TO_IN, height: 297 * MM_TO_IN };
        case INVENTORY_LAYOUT.A3:           return { width: 420 * MM_TO_IN, height: 297 * MM_TO_IN };
        case INVENTORY_LAYOUT.A2:           return { width: 420 * MM_TO_IN, height: 594 * MM_TO_IN };
    }
}

//================================================================//
// InventoryPageView
//================================================================//
const InventoryPageView = ( props ) => {

    const MARGIN = 0.125;

    const { assetIDs, inventory, pageSize } = props;
    const doc = getPageDimensions ( pageSize );

    doc.width = doc.width - MARGIN;
    doc.height = doc.height - MARGIN;

    const width = doc.width * DPI;
    const height = doc.height * DPI;

    const maxCols = Math.floor ( width / ASSET_WIDTH );
    const maxRows = Math.floor ( height / ASSET_HEIGHT );

    const xOff = ( width - ( maxCols * ASSET_WIDTH ) ) / 2;
    const yOff = ( height - ( maxRows * ASSET_HEIGHT ) ) / 2;

    const ASSET_FRAME_STYLE = {
        fill:           'none',
        stroke:         '#ffffff',
        strokeWidth:    3,
    };

    const GUIDE_LINE_STYLE = {
        fill:           'none',
        stroke:         '#000000',
        strokeWidth:    1,
    };

    let guidelines = [];

    for ( let col = 0; col < ( maxCols + 1 ); ++col ) {
        const x = xOff + ( col * ASSET_WIDTH );
        guidelines.push (<line key = { guidelines.length } x1 = { x } y1 = { 0 } x2 = { x } y2 = { height }/>);
    }

    for ( let row = 0; row < ( maxRows + 1 ); ++row ) {
        const y = yOff + ( row * ASSET_HEIGHT );
        guidelines.push (<line key = { guidelines.length } x1 = { 0 } y1 = { y } x2 = { width } y2 = { y }/>); 
    }

    let assets = [];

    for ( let row = 0; row < maxRows; ++row ) {
        for ( let col = 0; col < maxCols; ++col ) {

            const i = (( row * maxCols ) + col );
            if ( i >= assetIDs.length ) break;

            let x = xOff + ( col * ASSET_WIDTH );
            let y = yOff + ( row * ASSET_HEIGHT );

            assets.push (
                <g key = { i }>
                    <AssetView x = { x } y = { y } inventory = { inventory } assetId = { assetIDs [ i ]}/>
                    <rect x = { x } y = { y } width = { ASSET_WIDTH } height = { ASSET_HEIGHT } style = { ASSET_FRAME_STYLE }/>
                </g>
            );
        }
    }

    return (
        <svg
            version = "1.1"
            baseProfile = "basic"
            xmlns = "http://www.w3.org/2000/svg"
            xmlnsXlink = "http://www.w3.org/1999/xlink"
            width = { `${ doc.width }in` }
            height = { `${ doc.height }in` }
            viewBox = { `0 0 ${ width } ${ height }` }
            preserveAspectRatio = "none"
            >
            <g style = { GUIDE_LINE_STYLE }>
                { guidelines }
            </g>
            { assets }
        </svg>
    );
}

//================================================================//
// InventoryView
//================================================================//
export const InventoryView = observer (( props ) => {

    const [ selection, setSelection ]   = useState ({});

    const isSelected = ( asset ) => {
        return selection [ asset.assetID ] || false;
    }

    const onClickCard = ( asset ) => {
        const newSelection = Object.assign ({}, selection );
        newSelection [ asset.assetID ] = !isSelected ( asset );
        setSelection ( newSelection );
    }

    const layout        = props.layout || INVENTORY_LAYOUT.WEB;
    const inventory     = props.inventory;
    const assetArray    = props.assetArray || inventory.availableAssetsArray;

    let assetLayouts = [];

    const step = getAssetsPerPageSize ( layout );
    
    if ( step > 0 ) {
        for ( let i = 0; i < assetArray.length; i += step ) {

            let pageAssetIDs = [];

            for ( let j = 0; ( j < step ) && (( i + j ) < assetArray.length ); ++j ) {
                pageAssetIDs.push ( assetArray [ i + j ].assetID );
            }

            if ( pageAssetIDs.length > 0 ) {
                assetLayouts.push (
                    <div
                        className = 'page-break'
                        key = { i }
                    >
                        <InventoryPageView
                            assetIDs = { pageAssetIDs }
                            inventory = { inventory }
                            pageSize = { layout }
                        />
                    </div>
                );
            }
        }

        return (
            <div className = "asset-wrapper">
                { assetLayouts }
            </div>
        );
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
                    onClick = {() => { onclickCard ( asset )}}
                >
                    <AssetView
                        assetId = { asset.assetID }
                        inventory = { inventory }
                        inches = { true }
                        scale = { 0.75 }
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
                </Card>
            );
        }

        return assetLayoutCache [ i ];

    }

    return (
        <InfiniteScrollView 
            onGetAsset  = { getAsset }
            cardWidth   = { ASSET_WIDTH / 3 }
            cardHeight  = { ASSET_HEIGHT / 3 }
            totalCards  = { assetArray.length }
        />
    )
});