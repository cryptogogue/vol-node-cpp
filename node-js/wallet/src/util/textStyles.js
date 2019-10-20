/* eslint-disable no-whitespace-before-property */

import * as color                   from './color';
import _                            from 'lodash';

// https://regex101.com/
const COLOR_REGEX           = /#[0-9a-fA-F]+/;
const FONT_SCALE_REGEX      = /([0-9]*\.[0-9]+)|([0-9]+)%/;
const POINT_SIZE_REGEX      = /([0-9]*\.[0-9]+)|([0-9]+)p/;
const STYLE_COMMAND_REGEX   = /<\$.*?>/;
const WS_REGEX              = /\s+/;

//================================================================//
// parseTextStyles
//================================================================//

// foop doop <$#00ff77>boop<$> 

//----------------------------------------------------------------//
export function parse ( text, baseStyle ) {

    // text = 'foop<$$esc> doop <$#ff007f  arial           b  i 7p>boop<$>de-boop';

    baseStyle = baseStyle || {};

    let index = 0;
    let next = 0;
    let buffer = '';

    const styleSpans = [];
    const styleStack = [ baseStyle ]; // stack of style options

    const changeStyle = () => {
        styleSpans.push ({
            index:  buffer.length,
            style:  _.last ( styleStack ),
        });
    }
    changeStyle ();

    const popStyle = () => {
        if ( styleStack.length > 1 ) {
            styleStack.pop ();
            changeStyle ();
        }
    }

    const pushStyle = ( style ) => {
        styleStack.push ( _.merge ( _.cloneDeep ( _.last ( styleStack )), style ));
        changeStyle ();
    }

    do {

        text = text.slice ( next );
        const result = text.match ( STYLE_COMMAND_REGEX );

        if ( result ) {

            const match = result [ 0 ];
            index = result.index;
            next = index + match.length;

            if ( match.charAt ( 2 ) === '$' ) {
                buffer += text.slice ( 0, index + 1 ) + text.slice ( index + 2, next );
            }
            else {
                
                buffer += text.slice ( 0, index );

                const style = parseStyle ( match.slice ( 2, match.length - 1 ).split ( WS_REGEX ));

                if ( style ) {
                    pushStyle ( style );
                }
                else {
                    popStyle ();
                }
            }
        }
        else {
            buffer += text;
            index = -1;
        }

    } while ( index != -1 );
    
    const styledChars = [];

    for ( let i = 0; i < styleSpans.length; ++i ) {

        const styleSpan = styleSpans [ i ];
        let size = 0;

        if ( i === ( styleSpans.length - 1 )) {
            size = buffer.length - styleSpan.index;
        }
        else {
            size = styleSpans [ i + 1 ].index - styleSpan.index;
        }

        for ( let j = 0; j < size; ++j ) {
            styledChars.push ({
                char: buffer.charAt ( styleSpan.index + j ),
                style: styleSpan.style,
            });
        }
    }

    return styledChars;
}

//----------------------------------------------------------------//
const parseStyle = ( params ) => {

    let style = false;
    for ( let param of params ) {

        if ( param.length === 0 ) continue;
        style = style || {};

        if ( param === 'b' ) {
            style.bold = true;
        }
        else if ( param === 'i' ) {
            style.italic = true;
        }
        else if ( COLOR_REGEX.test ( param )) {
            style.color = color.fromHex ( param );
        }
        else if ( FONT_SCALE_REGEX.test ( param )) {
            style.scale = Number ( param.slice ( 0, param.length - 1 ));
        }
        else if ( POINT_SIZE_REGEX.test ( param )) {
            style.size = Number ( param.slice ( 0, param.length - 1 ));
        }
        else {
            style.font = param;
        }
    }
    return style;
}
