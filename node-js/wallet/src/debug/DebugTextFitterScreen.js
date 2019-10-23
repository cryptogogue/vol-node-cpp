/* eslint-disable no-whitespace-before-property */

import { Service, useService }              from '../Service';
import { TextFitter, FONT_FACE, JUSTIFY }   from '../util/textLayout';
import handlebars                           from 'handlebars';
import { action, computed, observable, }    from 'mobx';
import { observer }                         from 'mobx-react';
import * as opentype                        from 'opentype.js';
import React                                from 'react';

// https://malcoded.com/posts/react-file-upload/

const SVG_TEMPLATE = handlebars.compile ( `
    <svg
        version="1.1"
        baseProfile="full"
        xmlns="http://www.w3.org/2000/svg"
        width="{{ width }}"
        height="{{ height }}"
        viewBox="0 0 {{ width }} {{ height }}"
        preserveAspectRatio="none"
    >
        <rect width="100%" height="100%" fill="#7f7f7f" />
        {{{ text }}}
    </svg>
`);

const FONTS = {
    roboto: {
        [ FONT_FACE.REGULAR ]:      'http://localhost:3000/fonts/roboto/roboto-regular.ttf',
        [ FONT_FACE.BOLD ]:         'http://localhost:3000/fonts/roboto/roboto-bold.ttf',
        [ FONT_FACE.ITALIC ]:       'http://localhost:3000/fonts/roboto/roboto-regularitalic.ttf',
        [ FONT_FACE.BOLD_ITALIC ]:  'http://localhost:3000/fonts/roboto/roboto-bolditalic.ttf',
    },
};

//================================================================//
// DebugTextFitterService
//================================================================//
class DebugTextFitterService extends Service {

    @observable svg = '<svg/>';

    //----------------------------------------------------------------//
    constructor ( values ) {
        super ();
        this.fetchFontFiles ();
    }

    //----------------------------------------------------------------//
    async fetchFontFiles () {

        this.fonts = {};

        const fetchFont = async ( url ) => {
            if ( !url ) return false;
            const response  = await this.revocableFetch ( url );
            const buffer    = await response.arrayBuffer ();
            return opentype.parse ( buffer );
        }

        for ( let name in FONTS ) {

            try {
                const fontDesc = FONTS [ name ];
                const faces = {};

                for ( let face in fontDesc ) {
                    const url = fontDesc [ face ];
                    console.log ( 'FETCHING FONT', face, url );
                    faces [ face ] = await fetchFont ( url );
                }
                this.fonts [ name ] = faces;
            }
            catch ( error ) {
                console.log ( error );
            }
        }
        this.testFonts ();
    }

    //----------------------------------------------------------------//
    @action
    testFonts () {

        const text0 = 'This <$#ff0000 1.25%>is<$> some really <$#ffffff i>really<$> <$0.5%>long text that should <$#00ffff b i>wrap<$> to the text <$0.75>box!';
        const text1 = '<$0.25%>middle section middle section middle section middle section middle section';
        const text2 = 'This <$#ff0000 1.25%>is<$> some <$#0000003e>really<$> <$#ffffff>really<$> <$0.5%>long text that <$b>should<$> <$#00ffff>wrap<$> to the text <$0.75>box!';

        let fitter = new TextFitter ( this.fonts, 0, 0, 200, 600, JUSTIFY.VERTICAL.TOP );

        fitter.pushSection ( text0, 'roboto', 42, JUSTIFY.HORIZONTAL.LEFT );
        fitter.pushSection ( text1, 'roboto', 42, JUSTIFY.HORIZONTAL.CENTER );
        fitter.pushSection ( text2, 'roboto', 42, JUSTIFY.HORIZONTAL.RIGHT );
        fitter.fit ( 0 );
        console.log ( 'FITERATIONS:', fitter.fitIterations, fitter.fontScale );

        this.svg = SVG_TEMPLATE ({
            text: fitter.toSVG (),
            width: 200,
            height: 600,
        });
    }
}

//================================================================//
// DebugTextFitterScreen
//================================================================//
export const DebugTextFitterScreen = observer (( props ) => {

    const service = useService (() => new DebugTextFitterService ());

    return (
        <div>
            <div dangerouslySetInnerHTML = {{ __html: service.svg }}/>
        </div>
    );
});
