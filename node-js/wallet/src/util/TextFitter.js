/* eslint-disable no-whitespace-before-property */

import * as color               from './color';
import * as rect                from './rect';
import * as textStyles          from './textStyles';
import * as util                from './util';
import _                        from 'lodash';
import * as opentype            from 'opentype.js';

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

const DEFAULT_FIT_STEP_CUTOFF       = 0.0625;
const DEFAULT_MAX_FIT_ITERATIONS    = 16;

//----------------------------------------------------------------//
export const fitText = ( text, font, fontSize, x, y, width, height, hJustify, vJustify ) => {

    let fitter = new TextFitter ( font, fontSize, x, y, width, height, hJustify || JUSTIFY.HORIZONTAL.LEFT, vJustify || JUSTIFY.VERTICAL.TOP );
    fitter.fitDynamic ( text );
    return fitter.toSVG ();
}

//================================================================//
// TextLine
//================================================================//
class TextLine {

    //----------------------------------------------------------------//
    append ( tokenChars, context ) {

        this.tokenChars = this.tokenChars.concat ( tokenChars );

        let style       = false;
        let buffer      = '';
        let xOff        = 0;

        this.bounds     = false;
        this.segments   = [];
        this.height     = 0;

        const grow = () => {

            if ( buffer.length ) {

                const font      = context.font;
                const size      = style.size * style.scale * ( context.fontScale || 1 );
                const advance   = font.getAdvanceWidth ( buffer, size );
                const path      = font.getPath ( buffer, xOff, 0, size );
                let bb          = path.getBoundingBox ();
                bb              = rect.make ( bb.x1, bb.y1, bb.x2, bb.y2 );

                this.segments.push ({
                    path:       path,
                    style:      style,
                    font:       font,
                    bounds:     bb,
                });

                this.bounds = rect.grow ( this.bounds, bb );
                
                const ascender  = ( font.ascender / font.unitsPerEm ) * size;
                const descender  = -( font.descender / font.unitsPerEm ) * size;

                this.ascender = util.greater ( this.ascender, ascender );
                this.descender = util.greater ( this.descender, descender );

                xOff += advance;
            }
            buffer = '';
        }

        for ( let styledChar of this.tokenChars ) {

            if ( style != styledChar.style ) {
                grow ();
                style = styledChar.style;
            }
            buffer += styledChar.char;
        }
        grow ();
    }

    //----------------------------------------------------------------//
    constructor () {

        this.xOff       = 0;
        this.yOff       = 0;

        this.bounds         = false;
        this.tokenChars     = [];
        this.segments       = [];
        this.ascender       = 0;
        this.descender      = 0;
    }

    //----------------------------------------------------------------//
    makeSnapshot () {
        return {
            length:         this.tokenChars.length,
            bounds:         rect.copy ( this.bounds ),
            segments:       this.segments,
            ascender:       this.ascender,
            descender:      this.descender,
        };
    }

    //----------------------------------------------------------------//
    restoreFromSnapshot ( snapshot ) {
        this.tokenChars     = this.tokenChars.slice ( 0, snapshot.length );
        this.bounds         = snapshot.bounds;
        this.segments       = snapshot.segments;
        this.ascender       = snapshot.ascender;
        this.descender      = snapshot.descender;
    }

    //----------------------------------------------------------------//
    toSVG () {

        const x = this.xOff || 0;
        const y = this.yOff || 0;

        const paths = [];
        paths.push ( `<g transform = 'translate ( ${ x }, ${ y })'>` );

        for ( let segment of this.segments ) {
            const style = segment.style;
            const hexColor = color.toHexRGB ( style.color );

            paths.push ( `<g fill='${ hexColor }'>${ segment.path.toSVG ()}</g>` );
        }
        paths.push ( '</g>' );
        
        // return font.getPath ( this.text, this.xOff, this.yOff, fontSize ).toSVG ();
        return paths.join ( '' );
    }
}

//================================================================//
// TextFitter
//================================================================//
export class TextFitter {

    //----------------------------------------------------------------//
    constructor ( font, fontSize, x, y, width, height, hJustify, vJustify ) {

        this.font           = font;
        this.fontSize       = fontSize;

        this.bounds = rect.make (
            x,
            y,
            x + width,
            y + height,
        );

        this.maxWidth   = width;
        this.maxHeight  = height;

        this.hJustify = hJustify || JUSTIFY.HORIZONTAL.LEFT;
        this.vJustify = vJustify || JUSTIFY.VERTICAL.TOP;

        this.lines = [];

        this.baseStyle = {
            color:  color.make ( 0, 0, 0, 1 ),
            size:   this.fontSize,
            scale:  1,
        }
    }

    //----------------------------------------------------------------//
    fit ( text ) {

        this.lines = [];
        this.styledText = text ? textStyles.parse ( text, this.baseStyle ) : this.styledText;

        const length = this.styledText.length;

        let tokenStart = 0;
        let inToken = false;

        for ( let i = 0; i <= length; ++i ) {

            const charCode = i < length ? this.styledText [ i ].char.charCodeAt ( 0 ) : 0;

            if ( WHITESPACE_CHAR_CODES.includes ( charCode )) {

                if ( inToken ) {
                    this.pushToken ( this.styledText.slice ( tokenStart, i ));
                    inToken = false;
                    tokenStart = i;
                }
            }
            else {
                inToken = true;
            }
        }

        if ( this.lines.length === 0 ) return true;

        const hOverflow = this.layoutHorizontal ();
        const vOverflow = this.layoutVertical ();

        return ( hOverflow || vOverflow );
    }

    //----------------------------------------------------------------//
    fitDynamic ( text, fitStepCutoff, maxFitIterations ) {

        this.styledText = textStyles.parse ( text, this.baseStyle );

        this.fontScale = 1;
        this.maxFontScale = this.fontScale;
        this.minFontScale = 0;

        this.fitStepCutoff = fitStepCutoff || DEFAULT_FIT_STEP_CUTOFF;
        this.fitIterations = 0;
        this.maxFitIterations = maxFitIterations || DEFAULT_MAX_FIT_ITERATIONS;

        this.fitDynamicRecurse ();
    }

    //----------------------------------------------------------------//
    fitDynamicRecurse () {

        this.fitIterations = this.fitIterations + 1;

        let overflow = this.fit ();

        if ( overflow ) {
            // always get smaller on overflow
            this.maxFontScale = this.fontScale;
            this.fontScale = ( this.minFontScale + this.maxFontScale ) / 2;
            this.fitDynamicRecurse ();
        }
        else {
            // no overflow. maybe get bigger.
            this.minFontScale = this.fontScale;
            if (( this.fitIterations < this.maxFitIterations ) && (( this.maxFontScale - this.minFontScale ) > this.fitStepCutoff )) {
                this.fontScale = ( this.minFontScale + this.maxFontScale ) / 2;
                this.fitDynamicRecurse ();
            }
        }
    }

    //----------------------------------------------------------------//
    layoutHorizontal () {

        if ( this.lines.length === 0 ) return false;

        let overflow = false;

        for ( let i in this.lines ) {

            const line = this.lines [ i ];
            const bb = line.bounds;
            const lineLeft = -bb.x0;
            const lineWidth = rect.width ( bb );

            if ( this.maxWidth < lineWidth ) {
                overflow = true;
            }

            switch ( this.hJustify ) {

                case JUSTIFY.HORIZONTAL.LEFT:
                    line.xOff = this.bounds.x0 + lineLeft;
                    break;

                case JUSTIFY.HORIZONTAL.CENTER:
                    line.xOff = ((( this.bounds.x0 + this.bounds.x1 ) - lineWidth ) / 2 ) + lineLeft;
                    break;
                
                case JUSTIFY.HORIZONTAL.RIGHT:
                    line.xOff = ( this.bounds.x1 - lineWidth ) + lineLeft;
                    break;
            }
        }
        return overflow;
    }

    //----------------------------------------------------------------//
    layoutVertical () {

        if ( this.lines.length === 0 ) return false;

        let overflow = false;

        let y0 = null;
        let y1 = null;

        let base = 0;

        for ( let i in this.lines ) {

            let line = this.lines [ i ];
            base += line.ascender;
            
            const bb = line.bounds;

            const lineTop = bb.y0 + base;
            const lineBottom = bb.y1 + base;

            if (( y0 === null ) || ( y0 > lineTop )) {
                y0 = lineTop;
            }

            if (( y1 === null ) || ( y1 < lineBottom )) {
                y1 = lineBottom;
            }

            base += line.descender;
        }

        if ( this.maxHeight < ( y1 - y0 )) {
            overflow = true;
        }

        base = 0;

        for ( let i in this.lines ) {

            const line = this.lines [ i ];
            base += line.ascender;

            switch ( this.vJustify ) {

                case JUSTIFY.VERTICAL.TOP:
                    line.yOff = this.bounds.y0 + base;
                    break;

                case JUSTIFY.VERTICAL.CENTER: {
                    line.yOff = ((( this.bounds.y0 + this.bounds.y1 ) - ( y1 - y0 )) / 2 ) + base;
                    break;
                }
                case JUSTIFY.VERTICAL.BOTTOM:
                    line.yOff = ( this.bounds.x1 - ( y1 - y0 )) + base;
                    break;
            }
            base += line.descender;
        }
        return overflow;
    }

    //----------------------------------------------------------------//
    pushToken ( token ) {

        if ( this.lines.length === 0 ) {
            this.lines = [ new TextLine ()];
        }

        const line = _.last ( this.lines );
        const snapshot = line.makeSnapshot ();
        const isNewLine = ( snapshot.length === 0 );

        if ( isNewLine ) {
            for ( let i = 0; i < token.length; ++i ) {
                const charCode = token [ i ].char.charCodeAt ( 0 );
                if ( WHITESPACE_CHAR_CODES.includes ( charCode ) === false ) {
                    token = token.slice ( i );
                    break;
                }
            }
        }

        if ( token.length === 0 ) return;

        line.append ( token, this );

        const bb = line.bounds;
        const over = bb ? this.maxWidth < rect.width ( bb ) : false;

        // only try new line if line was *not* originally empty
        if ( over && ( isNewLine === false )) {
            line.restoreFromSnapshot ( snapshot );
            this.lines.push ( new TextLine ());
            this.pushToken ( token );
        }
    }

    //----------------------------------------------------------------//
    toSVG () {

        if ( this.lines.length === 0 ) return '';

        let svg = [];
        for ( let i in this.lines ) {
            svg.push ( this.lines [ i ].toSVG ());
        }
        return svg.join ();
    }
}
