/* eslint-disable no-whitespace-before-property */

import { LAYOUT_COMMAND }           from './schema/SchemaBuilder';
import * as pdf417                  from './util/pdf417';
import * as qrcode                  from './util/qrcode';
import { TextFitter, JUSTIFY }      from './util/textLayout';
import moize                        from 'moize';

//================================================================//
// AssetLayout
//================================================================//
export class AssetLayout {

    //----------------------------------------------------------------//
    constructor ( inventory, assetId, filters ) {

        const asset         = inventory.assets [ assetId ];
        const context       = inventory.composeAssetContext ( asset, filters, {[ '$' ]: assetId });
        const layout        = inventory.layouts [ context.layout ]

        const resources = {
            fonts:      inventory.fonts,
            icons:      inventory.icons,
        };

        let items = [];

        for ( let i in layout.commands ) {
            
            const command = layout.commands [ i ];
            
            const x = command.x || 0;
            const y = command.y || 0;
            const w = command.width || 0;
            const h = command.height || 0;

            let svg = false;

            switch ( command.type ) {

                case LAYOUT_COMMAND.DRAW_BARCODE: {

                    const value = command.template && command.template ( context ) || null;
                    if ( !value ) break;

                    let svgTag = '<g/>';

                    switch ( command.codeType || pdf417.CONSTS.ID ) {

                        case pdf417.CONSTS.ID: {
                            svgTag = pdf417.makeSVGTag ( value, x, y, w, h );
                            break;
                        }

                        case qrcode.CONSTS.ID: {

                            const valueUpper = value.toUpperCase ();

                            if ( qrcode.isLegal ( valueUpper )) {

                                const options = command.options || {};
                                const qrErr = options.qrErr || qrcode.CONSTS.ERROR_LEVEL.LOW;
                                let qrType = options.qrType || qrcode.CONSTS.AUTOSELECT_TYPE;
                                qrType = ( qrType !== qrcode.CONSTS.AUTOSELECT_TYPE )
                                    ? qrType
                                    : qrcode.autoSelectType ( qrErr, valueUpper.length );

                                svgTag = qrcode.makeSVGTag ( valueUpper, x, y, w, h, qrErr, qrType );
                            }
                            break;
                        }
                    }

                    svg = `
                        <rect x = ${ x } y = ${ y } width = ${ w } height = ${ h } fill = 'white'/>
                        ${ svgTag }
                    `;
                    break;
                }

                case LAYOUT_COMMAND.DRAW_SVG: {

                    svg = command.template && command.template ( context ) || false;
                    break;
                }

                case LAYOUT_COMMAND.DRAW_TEXT_BOX: {

                    const fitter = new TextFitter ( resources, x, y, w, h, command.vJustify );

                    for ( let segment of command.segments ) {

                        const value = segment.template && segment.template ( context ) || null;
                        if ( !value ) continue;

                        fitter.pushSection ( value, segment.fontName, segment.fontSize, segment.hJustify );
                    }
                    fitter.fit ();
                    svg = fitter.toSVG ();
                    break;
                }
            }

            if ( svg ) {
                svg = command.wrap ? command.wrap ({ body: svg }) : `<g>${ svg }</g>`;
                items.push ( svg );
            }
        }

        const svg = items.join ( '' );

        this.width      = layout.width;
        this.height     = layout.height;
        this.dpi        = layout.dpi;

        this.context    = context;
        this.svg        = layout.wrap ? layout.wrap ({ body: svg }) : `<g>${ svg }</g>`;
    }
}
