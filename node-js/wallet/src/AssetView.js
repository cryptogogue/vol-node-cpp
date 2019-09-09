/* eslint-disable no-whitespace-before-property */

import { LAYOUT_COMMAND }           from './schema/SchemaBuilder';
import { barcodeToSVG }             from './util/pdf417';
import { fitText, JUSTIFY }         from './util/TextFitter';
import handlebars                   from 'handlebars';
import { observer }                 from 'mobx-react';
import React                        from 'react';
import { forOfStatement } from '@babel/types';

//================================================================//
// AssetView
//================================================================//
export const AssetView = ( props ) => {

    const { inventory, assetId, inches } = props;

    const layout        = inventory.assetLayouts [ assetId ];

    const docX          = props.x || 0;
    const docY          = props.y || 0;
    const scale         = props.scale || 1;

    const docWidth      = layout.width;
    const docHeight     = layout.height;
    const dpi           = layout.dpi;

    const foob = () => {
        console.log( JSON.stringify( inventory.assets[assetId], null, 4 ))
    }

    return (
        <svg
            x = { docX }
            y = { docY }
            width = { inches ? `${( docWidth * scale ) / dpi }in` : docWidth * scale }
            height = { inches ? `${( docHeight * scale ) / dpi }in` : docHeight * scale }
            viewBox = { `0 0 ${ docWidth } ${ docHeight }` }
            preserveAspectRatio = 'none'
            onClick = {() => { foob()}}
        >
            <g dangerouslySetInnerHTML = {{ __html: layout.svg }}/>
        </svg>
    );
}
