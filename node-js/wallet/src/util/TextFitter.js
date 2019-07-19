/* eslint-disable no-whitespace-before-property */

import * as opentype                        from 'opentype.js';

const WHITESPACE_CHAR_CODES = [
    ' '.charCodeAt ( 0 ),
    '\t'.charCodeAt ( 0 ),
    0,
];

export const JUSTIFY = {
    HORIZONTAL: {
        LEFT:       'LEFT',
        CENTER:     'CENTER',
        RIGHT:      'RIGHT',
    },
    VERTICAL: {
        TOP:        'TOP',
        CENTER:     'CENTER',
        BOTTOM:     'BOTTOM',
    },
}

const DEFAULT_FIT_STEP_CUTOFF       = 0.5;
const DEFAULT_MAX_FIT_ITERATIONS    = 16;

//================================================================//
// TextLine
//================================================================//
class TextLine {

    //----------------------------------------------------------------//
    constructor () {

        this.text = '';
        this.path = null;
        this.xOff = 0;
        this.yOff = 0;
    }

    //----------------------------------------------------------------//
    toSVG ( font, fontSize ) {

        return font.getPath ( this.text, this.xOff, this.yOff, fontSize ).toSVG ();
    }
}

//================================================================//
// TextFitter
//================================================================//
export class TextFitter {

    //----------------------------------------------------------------//
    affirmLine () {

        const nLines = this.lines.length;
        return nLines > 0 ? this.lines [ nLines - 1 ] : this.nextLine ();
    }

    //----------------------------------------------------------------//
    constructor ( font, fontSize, x, y, width, height, hJustify, vJustify ) {

        this.font           = font;
        this.fontSize       = fontSize;

        this.bounds = {
            x0:     x,
            x1:     x + width,
            y0:     y,
            y1:     y + height,
        }

        this.maxWidth   = width;
        this.maxHeight  = height;

        this.hJustify = hJustify || JUSTIFY.HORIZONTAL.LEFT;
        this.vJustify = vJustify || JUSTIFY.VERTICAL.TOP;

        this.lines = [];
    }

    //----------------------------------------------------------------//
    fit ( text ) {

        this.lines = [];

        this.fitWidth ( text );
        if ( this.lines.length === 0 ) return true;

        const hOverflow = this.layoutHorizontal ();
        const vOverflow = this.layoutVertical ();

        return ( hOverflow || vOverflow );
    }

    //----------------------------------------------------------------//
    fitDynamic ( text, fitStepCutoff, maxFitIterations ) {

        this.maxFontSize = this.fontSize;
        this.minFontSize = 0;

        this.fitStepCutoff = fitStepCutoff || DEFAULT_FIT_STEP_CUTOFF;
        this.fitIterations = 0;
        this.maxFitIterations = maxFitIterations || DEFAULT_MAX_FIT_ITERATIONS;

        this.fitDynamicRecurse ( text );
    }

    //----------------------------------------------------------------//
    fitDynamicRecurse ( text ) {

        this.fitIterations = this.fitIterations + 1;

        let overflow = this.fit ( text );

        if ( overflow ) {
            // always get smaller on overflow
            this.maxFontSize = this.fontSize;
            this.fontSize = ( this.minFontSize + this.maxFontSize ) / 2;
            this.fitDynamicRecurse ( text );
        }
        else {
            // no overdlow. maybe get bigger.
            this.minFontSize = this.fontSize;
            if (( this.fitIterations < this.maxFitIterations ) && (( this.maxFontSize - this.minFontSize ) > this.fitStepCutoff )) {
                this.fontSize = ( this.minFontSize + this.maxFontSize ) / 2;
                this.fitDynamicRecurse ( text );
            }
        }
    }

    //----------------------------------------------------------------//
    fitWidth ( text, fontSize ) {

        const length = text.length;
        
        let ws = 0;
        let tokenStart = 0;
        let inToken = false;

        for ( let i = 0; i <= length; ++i ) {

            const charCode = i < length ? text.charCodeAt ( i ) : 0;
            if ( WHITESPACE_CHAR_CODES.includes ( charCode )) {

                if ( inToken ) {
                    this.pushToken ( text.substring ( tokenStart, i ));
                    inToken = false;
                    tokenStart = i;
                    ws = 0;
                }
                ws++;
            }
            else {
                inToken = true;
            }
        }
    }

    //----------------------------------------------------------------//
    layoutHorizontal () {

        if ( this.lines.length === 0 ) return false;

        let overflow = false;

        for ( let i in this.lines ) {
            let line = this.lines [ i ];
            const bb = line.path.getBoundingBox ();

            if ( this.maxWidth < ( bb.x2 - bb.x1 )) {
                overflow = true;
            }

            const lineLeft = -bb.x1;

            switch ( this.hJustify ) {

                case JUSTIFY.HORIZONTAL.LEFT:
                    line.xOff = this.bounds.x0 + lineLeft;
                    break;

                case JUSTIFY.HORIZONTAL.CENTER: {
                    line.xOff = ((( this.bounds.x0 + this.bounds.x1 ) - ( bb.x2 - bb.x1 )) / 2 ) + lineLeft;
                    break;
                }
                case JUSTIFY.HORIZONTAL.RIGHT:
                    line.xOff = ( this.bounds.x1 - ( bb.x2 - bb.x1 )) + lineLeft;
                    break;
            }
        }
        return overflow;
    }

    //----------------------------------------------------------------//
    layoutVertical () {

        if ( this.lines.length === 0 ) return false;

        let overflow = false;

        //const yOff = ( this.font.ascender / this.font.unitsPerEm ) * this.fontSize;
        let y0 = null;
        let y1 = null;

        for ( let i in this.lines ) {
            let line = this.lines [ i ];
            const bb = line.path.getBoundingBox ();

            const base = i * this.fontSize;

            const lineTop = bb.y1 + base;
            const lineBottom = bb.y2 + base;

            if (( y0 === null ) || ( y0 > lineTop )) {
                y0 = lineTop;
            }

            if (( y1 === null ) || ( y1 < lineBottom )) {
                y1 = lineBottom;
            }
        }

        if ( this.maxHeight < ( y1 - y0 )) {
            overflow = true;
        }

        for ( let i in this.lines ) {
            let line = this.lines [ i ];
            const bb = line.path.getBoundingBox ();

            const lineTop = ( i * this.fontSize ) - y0;

            switch ( this.vJustify ) {

                case JUSTIFY.VERTICAL.TOP:
                    line.yOff = this.bounds.y0 + lineTop;
                    break;

                case JUSTIFY.VERTICAL.CENTER: {
                    line.yOff = ((( this.bounds.y0 + this.bounds.y1 ) - ( y1 - y0 )) / 2 ) + lineTop;
                    break;
                }
                case JUSTIFY.VERTICAL.BOTTOM:
                    line.yOff = ( this.bounds.x1 - ( y1 - y0 )) + lineTop;
                    break;
            }
        }
        return overflow;
    }

    //----------------------------------------------------------------//
    nextLine () {

        let line = new TextLine ();
        this.lines.push ( line );
        return line;
    }

    //----------------------------------------------------------------//
    pushToken ( token ) {

        let line = this.affirmLine ();

        let text = line.text.concat ( token );
        let path = this.font.getPath ( text, 0, 0, this.fontSize );

        let bb = path.getBoundingBox ();
        let over = this.maxWidth < ( bb.x2 - bb.x1 );

        if (( line.text.length === 0 ) || ( !over )) {
            line.text = text;
            line.path = path;
        }
        else {
            this.nextLine ();
            this.pushToken ( token );
        }
    }

    //----------------------------------------------------------------//
    toSVG () {

        if ( this.lines.length === 0 ) return '';

        let svg = [];
        for ( let i in this.lines ) {
            svg.push ( this.lines [ i ].toSVG ( this.font, this.fontSize ));
        }
        return svg.join ();
    }
}
