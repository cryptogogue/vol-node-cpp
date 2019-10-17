/* eslint-disable no-whitespace-before-property */


// https://regex101.com/
const COLOR_REGEX           = /#[0-9a-fA-F]+/;
const POINT_SIZE_REGEX      = /([0-9]*\.[0-9]+)|([0-9]+)p/;
const STYLE_COMMAND_REGEX   = /<\$.*?>/;
const WS_REGEX              = /\s+/;

export const STYLE_COMMAND = {
    TEXT:   'TEXT',
    PUSH:   'PUSH',
    POP:    'POP',
};

//================================================================//
// parseTextStyles
//================================================================//

// foop doop <$#00ff77>boop<$> 

//----------------------------------------------------------------//
export function parse ( text ) {

    text = 'foop<$$esc> doop <$#ff007f  arial           b  i 7p>boop<$>de-boop';

    let index = 0;
    let next = 0;
    let buffer = '';

    let commands = [];

    const pushCommand = ( command, body ) => {

        switch ( command ) {

            case STYLE_COMMAND.TEXT:
                if ( body.length > 0 ) {
                    console.log ( 'TEXT', body );
                    commands.push ({
                        command:    STYLE_COMMAND.TEXT,
                        text:       body,
                    });
                }
                break;

            case STYLE_COMMAND.PUSH:
                commands.push ({
                    command:    STYLE_COMMAND.PUSH,
                    options:    body,
                });
                break;
                
            case STYLE_COMMAND.POP:
                commands.push ({
                    command:    STYLE_COMMAND.POP,
                });
                break;
        }
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
                
                pushCommand ( STYLE_COMMAND.TEXT, buffer + text.slice ( 0, index ));
                buffer = '';

                const options = parseOptions ( match.slice ( 2, match.length - 1 ).split ( WS_REGEX ));

                if ( options ) {
                    pushCommand ( STYLE_COMMAND.PUSH, options );
                }
                else {
                    pushCommand ( STYLE_COMMAND.POP );
                }
            }
        }
        else {
            pushCommand ( STYLE_COMMAND.TEXT, text );
            index = -1;
        }

    } while ( index != -1 );

    console.log ( JSON.stringify ( commands, null, 4 ));
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
const parseOptions = ( params ) => {

    let options = false;
    for ( let param of params ) {

        if ( param.length === 0 ) continue;
        options = options || {};

        if ( param === 'b' ) {
            options.bold = true;
        }
        else if ( param === 'i' ) {
            options.italic = true;
        }
        else if ( COLOR_REGEX.test ( param )) {
            options.color = parseColor ( param );
        }
        else if ( POINT_SIZE_REGEX.test ( param )) {
            options.pointSize = Number ( param.slice ( 0, param.length - 1 ));
        }
        else {
            options.font = param;
        }
    }
    return options;
}
