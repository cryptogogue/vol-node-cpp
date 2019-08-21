/* eslint-disable no-whitespace-before-property */

import { LAYOUT_COMMAND }           from './schema/SchemaBuilder';
import { barcodeToSVG }             from './util/pdf417';
import { fitText, JUSTIFY }         from './util/TextFitter';
import handlebars                   from 'handlebars';
import { observer }                 from 'mobx-react';
import React                        from 'react';

const assetTemplate = handlebars.compile ( `

    <svg x = "0" y = "0" width = "2.5in" height = "3.5in" viewBox = "0 0 750 1050" preserveAspectRatio = "none">
        {{{ asset }}}
    </svg>
`);

//================================================================//
// AssetView
//================================================================//
const AssetView = observer (( props ) => {

    const { inventory, assetId, inches } = props;

    const docX          = props.x || 0;
    const docY          = props.y || 0;

    const filters       = [ 'EN', 'RGB' ];
    const asset         = inventory.assets [ assetId ];
    const barcode       = barcodeToSVG ( assetId );
    const context       = inventory.composeAssetContext ( asset, filters, { barcode: barcode });

    const layout        = inventory.layouts [ context.layout ]

    const docWidth      = layout.width;
    const docHeight     = layout.height;
    const dpi           = layout.dpi;

    let items = [];
    for ( let i in layout.commands ) {
        
        const command = layout.commands [ i ];
        const value = command.field && ( command.field === '$' ? assetId : context [ command.field ]) || null;

        const x = command.x || 0;
        const y = command.y || 0;
        const w = command.width || 0;
        const h = command.height || 0;

        switch ( command.type ) {

            case LAYOUT_COMMAND.DRAW_BARCODE_FIELD:
                items.push (
                    <g key = { i }>
                        <rect x = { x } y = { y } width = { w } height = { h } fill = 'white'/>
                        <g dangerouslySetInnerHTML = {{ __html: barcodeToSVG ( value, x, y, w, h )}}/>
                    </g>
                );
                break;

            case LAYOUT_COMMAND.DRAW_IMAGE_FIELD:
                items.push (
                    <image
                        key                     = { i }
                        x                       = { command.x }
                        y                       = { command.y }
                        width                   = { command.width }
                        height                  = { command.height }
                        xlinkHref               = { value }
                    />
                );
                break;

            case LAYOUT_COMMAND.DRAW_SVG:
                items.push (
                    <g
                        key = { i }
                        dangerouslySetInnerHTML = {{ __html: command.svg }}
                    />
                );
                break;

            case LAYOUT_COMMAND.DRAW_TEXT_FIELD: {

                const font = inventory.fonts [ command.fontName ];
                if ( font ) {

                    const hJustify  = command.hJustify || JUSTIFY.HORIZONTAL.LEFT;
                    const vJustify  = command.vJustify || JUSTIFY.VERTICAL.TOP;
                    const fill      = command.fill || 'black'

                    const fontSize = command.fontSize || h;
                    const svg = fitText ( value, font, fontSize, x, y, w, h, hJustify, vJustify );

                    items.push (
                        <g
                            key = { i }
                            fill = { fill }
                            dangerouslySetInnerHTML = {{ __html: svg }}
                        />
                    );
                }
                break;
            }
        }
    }

    return (
        <svg
            x = { docX }
            y = { docY }
            width = { inches ? `${ docWidth / dpi }in` : docWidth }
            height = { inches ? `${ docHeight / dpi }in` : docHeight }
            viewBox = { `0 0 ${ docWidth } ${ docHeight }` }
            preserveAspectRatio = 'none'
        >
            { items }
        </svg>
    );
});


export default AssetView;