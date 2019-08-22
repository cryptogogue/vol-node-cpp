/* eslint-disable no-whitespace-before-property */

import { LAYOUT_COMMAND }           from './schema/SchemaBuilder';
import { barcodeToSVG }             from './util/pdf417';
import { fitText, JUSTIFY }         from './util/TextFitter';
import handlebars                   from 'handlebars';
import moize                        from 'moize';

//================================================================//
// AssetLayout
//================================================================//
export class AssetLayout {

    //----------------------------------------------------------------//
    constructor ( inventory, assetId, filters ) {

        console.log ( 'RENDER ASSET VIEW ITEMS' );

        const asset         = inventory.assets [ assetId ];
        const barcode       = barcodeToSVG ( assetId );
        const context       = inventory.composeAssetContext ( asset, filters, {[ '$' ]: assetId, barcode: barcode });

        const layout        = inventory.layouts [ context.layout ]

        let items = [];
        items.push ( `<g>` );

        for ( let i in layout.commands ) {
            
            const command = layout.commands [ i ];
            const value = context [ command.field ] || null;

            const x = command.x || 0;
            const y = command.y || 0;
            const w = command.width || 0;
            const h = command.height || 0;

            switch ( command.type ) {

                case LAYOUT_COMMAND.DRAW_BARCODE_FIELD:

                    items.push (`
                        <g>
                            <rect x = ${ x } y = ${ y } width = ${ w } height = ${ h } fill = 'white'/>
                            ${ barcodeToSVG ( value, x, y, w, h )}
                        </g>
                    `);
                    break;

                case LAYOUT_COMMAND.DRAW_IMAGE_FIELD:

                    items.push (`
                        <image
                            x                       = ${ command.x }
                            y                       = ${ command.y }
                            width                   = ${ command.width }
                            height                  = ${ command.height }
                            xlink:href              = ${ value }
                        />
                    `);
                    break;

                case LAYOUT_COMMAND.DRAW_SVG:

                    items.push ( `<g>${ command.svg }</g>` );
                    break;

                case LAYOUT_COMMAND.DRAW_TEXT_FIELD: {

                    const font = inventory.fonts [ command.fontName ];
                    if ( font ) {

                        const hJustify  = command.hJustify || JUSTIFY.HORIZONTAL.LEFT;
                        const vJustify  = command.vJustify || JUSTIFY.VERTICAL.TOP;
                        const fill      = command.fill || 'black'

                        const fontSize = command.fontSize || h;
                        const svg = fitText ( value, font, fontSize, x, y, w, h, hJustify, vJustify );

                        items.push ( `<g fill = ${ fill }>${ svg }</g>` );
                    }
                    break;
                }
            }
        }

        items.push ( `</g>` );

        this.width      = layout.width;
        this.height     = layout.height;
        this.dpi        = layout.dpi;

        this.context    = context;
        this.svg        = items.join ( '' );
    }
}
