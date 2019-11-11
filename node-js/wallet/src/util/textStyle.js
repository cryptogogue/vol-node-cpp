/* eslint-disable no-whitespace-before-property */

import * as color                   from './color';
import { JUSTIFY }                  from './textLayout';
import _                            from 'lodash';

// https://regex101.com/
const COLOR_REGEX           = /#[0-9a-fA-F]+/;
const NAMED_PARAM_REGEX     = /\w+:[\w-.%]+/;
const NUMBER_REGEX          = /([0-9]*\.[0-9]+)|([0-9]+)/;
const PERCENTAGE_REGEX      = /([0-9]*\.[0-9]+)|([0-9]+)%/;
const POINT_SIZE_REGEX      = /([0-9]*\.[0-9]+)|([0-9]+)p/;
const SIMPLE_COMMAND_REGEX  = /^[biulrc]$/;
const STYLE_COMMAND_REGEX   = /<.*?>/;
const WS_REGEX              = /\s+/;

const PARAM_TYPE = {
    COLOR:          'COLOR',
    NUMBER:         'NUMBER',
    PERCENTAGE:     'PERCENTAGE',
    POINT_SIZE:     'POINT_SIZE',
    STRING:         'STRING',
    UNKNOWN:        'UNKNOWN',
};

const PARAM_NAME = {
    ICON_Y:         'icon_y',
    ICON_FIT:       'icon_fit',
};

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

            const skip = () => {
                buffer += text.slice ( 0, index + 1 ) + text.slice ( index + 2, next );
            }

            const fold = () => {
                buffer += text.slice ( 0, next );
            }

            const flush = () => {
                buffer += text.slice ( 0, index );
            }

            const tokenize = () => {
                return match.slice ( 2, match.length - 1 ).split ( WS_REGEX );
            }

            switch ( match.charAt ( 1 )) {

                case '@': {
                    
                    if ( match.charAt ( 2 ) === '@' ) {
                        skip ();
                        break;
                    }

                    flush ();
                    const iconNames = tokenize ();

                    for ( let iconName of iconNames ) {
                        pushStyle ({ icon: iconName });
                        buffer += '#';
                        popStyle ();
                    }
                    break;
                }

                case '$': {

                    if ( match.charAt ( 2 ) === '$' ) {
                        skip ();
                        break;
                    }

                    flush ();
                    const style = parseStyle ( tokenize ());

                    if ( style ) {
                        pushStyle ( style );
                    }
                    else {
                        popStyle ();
                    }
                    break;
                }

                default: {
                    fold ();
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
const parseParam = ( param ) => {


    let type = PARAM_TYPE.STRING;
    let value = param;

    if ( COLOR_REGEX.test ( param )) {
        type    = PARAM_TYPE.COLOR;
        value   = color.fromHex ( param );
    }
    else if ( PERCENTAGE_REGEX.test ( param )) {
        type    = PARAM_TYPE.PERCENTAGE;
        value   = Number ( param.slice ( 0, param.length - 1 )) / 100;
    }
    else if ( POINT_SIZE_REGEX.test ( param )) {
        type    = PARAM_TYPE.POINT_SIZE;
        value   = Number ( param.slice ( 0, param.length - 1 ));
    }
    else if ( NUMBER_REGEX.test ( param )) {
        type    = PARAM_TYPE.NUMBER;
        value   = Number ( param );
    }
    
    return {
        type:   type,
        value:  value,
    };
}

//----------------------------------------------------------------//
const parseStyle = ( params ) => {

    let style = false;
    for ( let param of params ) {

        if ( param.length === 0 ) continue;
        style = style || {};

        if ( SIMPLE_COMMAND_REGEX.test ( param )) {

            switch ( param ) {
                case 'b':
                    style.bold = true;
                    break;
                case 'i':
                    style.italic = true;
                    break;
                case 'u':
                    style.underline = true;
                    break;
                case 'l':
                    style.hJustify = JUSTIFY.HORIZONTAL.LEFT;
                    break;
                case 'r':
                    style.hJustify = JUSTIFY.HORIZONTAL.RIGHT;
                    break;
                case 'c':
                    style.hJustify = JUSTIFY.HORIZONTAL.CENTER;
                    break;
            }
        }
        else if ( NAMED_PARAM_REGEX.test ( param )) {

            const pair = param.split ( ':' );
            const name = pair [ 0 ];
            param = pair [ 1 ];

            const paramInfo = parseParam ( param );

            switch ( name ) {
                
                case PARAM_NAME.ICON_Y:
                    if ( paramInfo.type === PARAM_TYPE.PERCENTAGE ) {
                        style.iconY = paramInfo.value;
                    }
                    break;

                case PARAM_NAME.ICON_FIT:
                    if ( paramInfo.type === PARAM_TYPE.STRING ) {
                        style.iconFit = paramInfo.value;
                    }
                    break;
            }
        }
        else {

            const paramInfo = parseParam ( param );

            switch ( paramInfo.type ) {

                case PARAM_TYPE.COLOR:
                    style.color = paramInfo.value;
                    break;

                case PARAM_TYPE.NUMBER:
                    break;

                case PARAM_TYPE.PERCENTAGE:
                    style.scale = paramInfo.value;
                    break;

                case PARAM_TYPE.POINT_SIZE:
                    style.size = paramInfo.value;
                    break;

                case PARAM_TYPE.STRING:
                    style.font = param;
                    break;
            }
        }
    }
    return style;
}
