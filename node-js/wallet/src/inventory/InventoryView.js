/* eslint-disable no-whitespace-before-property */

import AssetView                                            from '../AssetView';
import NavigationBar                                        from '../NavigationBar';
import { AppStateService }                                  from '../stores/AppStateService';
import { Service, useService }                              from '../stores/Service';
import { InventoryService }                                 from '../stores/InventoryService';
import * as util                                            from '../util/util';
import handlebars                                           from 'handlebars';
import { action, computed, extendObservable, observable }   from 'mobx';
import { observer }                                         from 'mobx-react';
import React, { useState }                                  from 'react';
import { Link }                                             from 'react-router-dom';
import { Dropdown, Grid, Icon, List, Menu }                 from 'semantic-ui-react';

export const INVENTORY_LAYOUT = {
    WEB:                'WEB',
    US_LETTER:          'US_LETTER',
};

export function getInventoryLayoutFriendlyName ( layout ) {

    switch ( layout ) {
        case INVENTORY_LAYOUT.WEB:          return 'Web';
        case INVENTORY_LAYOUT.US_LETTER:    return 'US Letter (8.5" x 11")';
    }
}

function getAssetsPerTemplate ( layout ) {

    switch ( layout ) {
        case INVENTORY_LAYOUT.WEB:          return -1;
        case INVENTORY_LAYOUT.US_LETTER:    return 9;
    }
}

const templates = {};

templates [ INVENTORY_LAYOUT.US_LETTER ] = handlebars.compile ( `
    <svg
        version = "1.1"
        baseProfile = "basic"
        xmlns = "http://www.w3.org/2000/svg"
        xmlns:xlink = "http://www.w3.org/1999/xlink"
        width = "8.5in"
        height = "11in"
        viewBox = "0 0 2550 3300"
        preserveAspectRatio = "none"
        >
        <g style = "stroke:#000000;stroke-width:1">
            <line x1 = "150" y1 = "0" x2 = "150" y2 = "3300"/>
            <line x1 = "900" y1 = "0" x2 = "900" y2 = "3300"/>
            <line x1 = "1650" y1 = "0" x2 = "1650" y2 = "3300"/>
            <line x1 = "2400" y1 = "0" x2 = "2400" y2 = "3300"/>

            <line x1 = "0" y1 = "75" x2 = "2550" y2 = "75"/>
            <line x1 = "0" y1 = "1125" x2 = "2550" y2 = "1125"/>
            <line x1 = "0" y1 = "2175" x2 = "2550" y2 = "2175"/>
            <line x1 = "0" y1 = "3225" x2 = "2550" y2 = "3225"/>
        </g>
        <g transform = "translate ( 150, 75 )">

            <svg x = "0" y = "0" width = "750" height = "1050" viewBox = "0 0 750 1050" preserveAspectRatio = "none">
                {{{ asset0 }}}
            </svg>
            <svg x = "750" y = "0" width = "750" height = "1050" viewBox = "0 0 750 1050" preserveAspectRatio = "none">
                {{{ asset1 }}}
            </svg>
            <svg x = "1500" y = "0" width = "750" height = "1050" viewBox = "0 0 750 1050" preserveAspectRatio = "none"> 
                {{{ asset2 }}}
            </svg>

            <svg x = "0" y = "1050" width = "750" height = "1050" viewBox = "0 0 750 1050" preserveAspectRatio = "none">
                {{{ asset3 }}}
            </svg>
            <svg x = "750" y = "1050" width = "750" height = "1050" viewBox = "0 0 750 1050" preserveAspectRatio = "none"> 
                {{{ asset4 }}}
            </svg>
            <svg x = "1500" y = "1050" width = "750" height = "1050" viewBox = "0 0 750 1050" preserveAspectRatio = "none"> 
                {{{ asset5 }}}
            </svg>

            <svg x = "0" y = "2100" width = "750" height = "1050" viewBox = "0 0 750 1050" preserveAspectRatio = "none"> 
                {{{ asset6 }}}
            </svg>
            <svg x = "750" y = "2100" width = "750" height = "1050" viewBox = "0 0 750 1050" preserveAspectRatio = "none"> 
                {{{ asset7 }}}
            </svg>
            <svg x = "1500" y = "2100" width = "750" height = "1050" viewBox = "0 0 750 1050" preserveAspectRatio = "none">
                {{{ asset8 }}}
            </svg>
        </g>
        <g style = "stroke:#ffffff;stroke-width:3">

            <line x1 = "900" y1 = "75" x2 = "900" y2 = "3225"/>
            <line x1 = "1650" y1 = "75" x2 = "1650" y2 = "3225"/>

            <line x1 = "150" y1 = "1125" x2 = "2400" y2 = "1125"/>
            <line x1 = "150" y1 = "2175" x2 = "2400" y2 = "2175"/>
        </g>
    </svg>
`);

//================================================================//
// InventoryView
//================================================================//
export const InventoryView = observer (( props ) => {

    const { controller, layout } = props;

    const inventory = controller.inventory;
    const assetArray = controller.sortedAssets;

    let assetLayouts = [];
    
    const template = templates [ layout ];
    
    if ( template ) {

        const step = getAssetsPerTemplate ( layout );
        let base = 0;
        for ( let i = 0; i < assetArray.length; i += step ) {

            let pageContext = {};
            for ( let j = 0; ( j < step ) && (( i + j ) < assetArray.length ); ++j ) {
                const assetID = assetArray [ i + j ].assetID;
                pageContext [ 'asset' + String ( j )] = inventory.assetLayouts [ assetID ];
            }
            const svg = template ( pageContext );
            assetLayouts.push (<div key = { i } dangerouslySetInnerHTML = {{ __html: svg }}/>);
        }
    }
    else {
        for ( let i in assetArray ) {
            const asset = assetArray [ i ];
            assetLayouts.push (<AssetView
                key = { asset.assetID }
                style = {{ float:'left' }}
                inventory = { inventory }
                assetId = { asset.assetID }
            />);
        }
    }

    return (
        <div>
            { assetLayouts }
        </div>
    );
});
