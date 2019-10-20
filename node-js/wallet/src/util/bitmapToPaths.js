/* eslint-disable no-whitespace-before-property */
   

const EDGE_CELL_OFFSETS = [
    { X_OFF: -1,    Y_OFF: 0 },    // offset to W
    { X_OFF: 0,     Y_OFF: 1 },    // offset to S
    { X_OFF: 1,     Y_OFF: 0 },    // offset to E
    { X_OFF: 0,     Y_OFF: -1 },   // offset to N
];

const CORNER_INDEX_OFFSETS = [
    { X_OFF: 0,     Y_OFF: 0 },    // offset to NW
    { X_OFF: 0,     Y_OFF: 1 },    // offset to SW
    { X_OFF: 1,     Y_OFF: 1 },    // offset to SE
    { X_OFF: 1,     Y_OFF: 0 },    // offset to NE
];

const DIRECTION_MASK = {
    S:      0b0001,
    E:      0b0010,
    N:      0b0100,
    W:      0b1000,
};

const CORNER_EDGE_DIRECTIONS = [
    0b0001,                         // S
    0b0010,                         // E
    0b0100,                         // N
    0b1000,                         // W
];

const HEADING = {
    S: 0,
    E: 1,
    N: 2,
    W: 3,
};

const TURN = {
    NONE: 0,
    LEFT: 1,
    BACK: 2,
    RIGHT: 3,
};

const STEP = [
    { X_OFF: 0,     Y_OFF: 1 },    // S
    { X_OFF: 1,     Y_OFF: 0 },    // E
    { X_OFF: 0,     Y_OFF: -1 },   // N
    { X_OFF: -1,    Y_OFF: 0 },    // W
];

export const PATH_COMMAND = {
    BEGIN:     'BEGIN',
    EXTEND:    'EXTEND',
    END:       'END',
};

//----------------------------------------------------------------//
export function bitmapToPaths ( sampler, width, height, onCommand ) {

    if ( typeof ( sampler ) !== 'function' ) return;
    if ( typeof ( onCommand ) !== 'function' ) return;

    const corners = [];

    const sample = ( x, y ) => {
        if (( x < 0 ) || ( width <= x )) return false;
        if (( y < 0 ) || ( height <= y )) return false;
        return ( sampler ( x, y ) === true );
    }

    const checkEdge = ( x, y, xOff, yOff ) => {
        const g = sample ( x, y );
        const n = sample ( x + xOff, y + yOff );
        return ( g && !n );
    }

    const setCorner = ( edge, x, y ) => {
        const { X_OFF, Y_OFF } = CORNER_INDEX_OFFSETS [ edge ];
        corners [ y + Y_OFF ][ x + X_OFF ] |= CORNER_EDGE_DIRECTIONS [ edge ];
    }

    const setEdge = ( edge, x, y ) => {
        const { X_OFF, Y_OFF } = EDGE_CELL_OFFSETS [ edge ];
        if ( checkEdge ( x, y, X_OFF, Y_OFF )) {
            setCorner ( edge, x, y );
        }
    }

    for ( let y = 0; y < ( height + 1 ); ++y ) {
        corners [ y ] = [];
        for ( let x = 0; x < ( width + 1 ); ++x ) {
            corners [ y ][ x ] = 0;
        }
    }

    for ( let y = 0; y < height; ++y ) {
        for ( let x = 0; x < width; ++x ) {
            setEdge ( 0, x, y );
            setEdge ( 1, x, y );
            setEdge ( 2, x, y );
            setEdge ( 3, x, y );
        }
    }

    for ( let y = 0; y < height; ++y ) {
        for ( let x = 0; x < width; ++x ) {
            trace ( corners, x, y, onCommand );
        }
    }
}

//----------------------------------------------------------------//
export function bitmapToSVG ( sampler, numCols, numRows, xOff, yOff, bw, bh, decimals ) {

    bw = ( bw / numCols ) || 1;
    bh = ( bh / numRows ) || 1;

    decimals = decimals || 2;

    const paths = [];
    let commands = [];
    
    const onCommand = ( command, x, y ) => {

        x = Number ( xOff + ( x * bw )).toFixed ( 2 );
        y = Number ( yOff + ( y * bh )).toFixed ( 2 );

        switch ( command ) {

            case PATH_COMMAND.BEGIN:
                commands.push (`M ${ x } ${ y }` );
                break;

            case PATH_COMMAND.EXTEND:
                commands.push ( ` L ${ x } ${ y }` );
                break;

            case PATH_COMMAND.END:
                paths.push ( `    ${ commands.join ( '' )} Z\n` );
                commands = [];
                break;
        }
    }

    bitmapToPaths ( sampler, numCols, numRows, onCommand );

    return `<g fill = 'black'><path d = "\n${ paths.join ( '' ) }"/></g>`;
}

//----------------------------------------------------------------//
function trace ( corners, x, y, onCommand ) {

    if ( !( corners [ y ][ x ] & DIRECTION_MASK.S )) return;

    // console.log ( 'TRACE', x, y );

    let tx = x; // turtle x
    let ty = y; // turtle y
    let h = HEADING.S; // heading

    onCommand ( PATH_COMMAND.BEGIN, x, y, `M ${ tx } ${ ty }` ); // let's get started

    const isDegenerate = ( t ) => {
        if ( t === 0b0000 ) return true;
        if ( t & 0b0100 ) return true; // turn back
        if (( t & 0b0001 ) && ( t & 0b1010 )) return true; // forward and left or right
        return false;
    }

    do {

        // clear the chosen direction.
        corners [ ty ][ tx ] &= ~CORNER_EDGE_DIRECTIONS [ h ];

        // step in the direction we're heading.
        tx += STEP [ h ].X_OFF;
        ty += STEP [ h ].Y_OFF;

        // get the corner directions.
        let c = corners [ ty ][ tx ];

        // console.log ( 'STEP', h, tx, ty, c );

        // get directions relative to turtle.
        c = ( c | ( c << 4 )) >> h;

        // can't continue. this can happen when returning to
        // the start, so it isn't always an error.
        if ( isDegenerate ( c )) break;

        // choose the best direction.
        let t =
            ( c & 0b0001 ) ? TURN.NONE : // keep going forward.
            ( c & 0b0010 ) ? TURN.LEFT : // turn to the left.
            TURN.RIGHT; // turn to the right.

        if ( t !== TURN.NONE ) {
            onCommand ( PATH_COMMAND.EXTEND, tx, ty, ` L ${ tx } ${ ty }` );
        }

        // get heading relative to world.
        h = ( h + t ) % 4;
    } 
    while (( tx !== x ) || ( ty !== y ));

    onCommand ( PATH_COMMAND.END, tx, ty, ` Z` );
}

