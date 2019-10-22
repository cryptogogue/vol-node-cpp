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

        let items = [];
        items.push ( `<g>` );

        for ( let i in layout.commands ) {
            
            const command = layout.commands [ i ];
            

            const x = command.x || 0;
            const y = command.y || 0;
            const w = command.width || 0;
            const h = command.height || 0;

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

                    items.push (`
                        <g>
                            <rect x = ${ x } y = ${ y } width = ${ w } height = ${ h } fill = 'white'/>
                            ${ svgTag }
                        </g>
                    `);
                    break;
                }

                case LAYOUT_COMMAND.DRAW_SVG: {

                    const value = command.template && command.template ( context ) || null;
                    if ( !value ) break;

                    items.push ( `<g>${ value }</g>` );
                    break;
                }

                case LAYOUT_COMMAND.DRAW_TEXT_BOX: {

                    const fitter = new TextFitter ( inventory.fonts, x, y, w, h, command.vJustify );

                    for ( let segment of command.segments ) {

                        const value = segment.template && segment.template ( context ) || null;
                        if ( !value ) continue;

                        fitter.pushSection ( value, segment.fontName, segment.fontSize, segment.hJustify );
                    }
                    fitter.fit ();
                    items.push ( `<g>${ fitter.toSVG ()}</g>` );
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
