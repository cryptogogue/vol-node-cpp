/* eslint-disable no-whitespace-before-property */

import _                            from 'lodash';

// https://regex101.com/
const COLOR_REGEX           = /#[0-9a-fA-F]+/;
const POINT_SIZE_REGEX      = /([0-9]*\.[0-9]+)|([0-9]+)p/;
const STYLE_COMMAND_REGEX   = /<\$.*?>/;
const WS_REGEX              = /\s+/;

//================================================================//
// parseTextStyles
//================================================================//

// foop doop <$#00ff77>boop<$> 

//----------------------------------------------------------------//
export function parse ( text, baseStyle ) {

    text = 'foop<$$esc> doop <$#ff007f  arial           b  i 7p>boop<$>de-boop';

    baseStyle = baseStyle || {};

    let index = 0;
    let next = 0;
    let buffer = '';

    const styles = [];
    const styleStack = [ baseStyle ]; // stack of style options

    const changeStyle = () => {
        styles.push ({
            index: buffer.length,
            style: _.last ( styleStack ),
        });
    }
    changeStyle ();

    const popStyle = () => {
        styleStack.pop ();
        changeStyle (); 
    }

    const pushStyle = ( style ) => {
        styleStack.push ( _.merge ( _.clone ( style ), _.last ( styleStack )));
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

    console.log ( buffer );
    console.log ( JSON.stringify ( styles, null, 4 ));

    return {
        text: buffer,
        styles: styles,
    };
}

//----------------------------------------------------------------//
const parseColor = ( param ) => {

    const color = {
        r:  0,
        g:  0,
        b:  0,
        a:  1,
    };

    param = param.slice ( 1 );

    const component = ( i ) => {
        i = i * 2;
        const n = parseInt ( param.slice ( i, i + 2 ), 16 ) / 255;
        return Math.round ( n * 100 ) / 100
    }

    if ( param.length >= 6 ) {
        color.r = component ( 0 );
        color.g = component ( 1 );
        color.b = component ( 2 );
    }

    if ( param.length == 8 ) {
        color.a = component ( 3 );
    }

    return color;
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
            style.color = parseColor ( param );
        }
        else if ( POINT_SIZE_REGEX.test ( param )) {
            style.pointSize = Number ( param.slice ( 0, param.length - 1 ));
        }
        else {
            style.font = param;
        }
    }
    return style;
}
