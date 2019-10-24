/* eslint-disable no-whitespace-before-property */

import { Service, useService }              from '../Service';
import { TextFitter, FONT_FACE, JUSTIFY }   from '../util/textLayout';
import handlebars                           from 'handlebars';
import { action, computed, observable, }    from 'mobx';
import { observer }                         from 'mobx-react';
import * as opentype                        from 'opentype.js';
import React                                from 'react';

// https://malcoded.com/posts/react-file-upload/

const SVG_TEMPLATE = handlebars.compile (`
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

const SVG_CIRCLE_ICON = {
    svg:    `<circle cx = '0.5' cy = '0.5' r = '0.5'/>`,
    width:  1,
};

const SVG_TRIANGLE_ICON = {
    svg:    `<polygon points = '0,1 1,1 0.5,0'/>`,
    width:  1,
};

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

    @observable svg = [ '<svg/>' ];

    //----------------------------------------------------------------//
    constructor ( values ) {
        super ();
        this.fetchFontFiles ();
        this.resources = {
            fonts: {},
            icons: {
                circle: SVG_CIRCLE_ICON,
                triangle: SVG_TRIANGLE_ICON,
            },
        }
    }

    //----------------------------------------------------------------//
    async fetchFontFiles () {

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
                this.resources.fonts [ name ] = faces;
            }
            catch ( error ) {
                console.log ( error );
            }
        }
        this.testFonts ();
    }

    //----------------------------------------------------------------//
    @action
    pushSVG ( svg, width, height ) {

        svg = SVG_TEMPLATE ({
            text: svg,
            width: width,
            height: height,
        });

        this.svg.push (
            <div
                key = { this.svg.length }
                dangerouslySetInnerHTML = {{ __html: svg }}
            />
        );
    }

    //----------------------------------------------------------------//
    @action
    testFonts () {

        const text0 = 'This <$#ff0000 125%>is<$> some really <$#ffffff i>really<$> <$50%>long text that should <$#00ffff b i>wrap<$> to the text <$100%>box!';
        const text1 = '<$25%>middle section middle section middle section middle section middle section';
        const text2 = 'This <$#ff0000 125%>is<$> some <$#0000003e>really<$> <$#ffffff>really<$> <$50%>long text that <$b>should<$> <$#00ffff>wrap<$> to the text <$100%>box!';

        let fitter = new TextFitter ( this.resources, 0, 0, 200, 600, JUSTIFY.VERTICAL.TOP );

        fitter.pushSection ( text0, 'roboto', 42, JUSTIFY.HORIZONTAL.LEFT );
        fitter.pushSection ( text1, 'roboto', 42, JUSTIFY.HORIZONTAL.CENTER );
        fitter.pushSection ( text2, 'roboto', 42, JUSTIFY.HORIZONTAL.RIGHT );
        fitter.fit ( 0 );
        console.log ( 'FITERATIONS:', fitter.fitIterations, fitter.fontScale );

        this.pushSVG ( fitter.toSVG (), 200, 600 );

        const text3 = '\tThis is\nhow we test    test\ntext with\n   new lines\n and\n \twhitespace.';

        fitter = new TextFitter ( this.resources, 0, 0, 128, 160, JUSTIFY.VERTICAL.TOP );
        fitter.pushSection ( text3, 'roboto', 24, JUSTIFY.HORIZONTAL.LEFT );
        fitter.fit ();

        this.pushSVG ( fitter.toSVG (), 128, 160 );

        const text4 = 'Test <$i>escaping<$> <$$>style<@@> blocks.';

        fitter = new TextFitter ( this.resources, 0, 0, 600, 50, JUSTIFY.VERTICAL.TOP );
        fitter.pushSection ( text4, 'roboto', 42, JUSTIFY.HORIZONTAL.LEFT );
        fitter.fit ();

        this.pushSVG ( fitter.toSVG (), 600, 50 );

        const text5 = 'This is a <@triangle circle triangle> test of inline ic<@circle>ns.';

        fitter = new TextFitter ( this.resources, 0, 0, 600, 50, JUSTIFY.VERTICAL.TOP );
        fitter.pushSection ( text5, 'roboto', 42, JUSTIFY.HORIZONTAL.LEFT );
        fitter.fit ();

        this.pushSVG ( fitter.toSVG (), 600, 50 );
    }
}

//================================================================//
// DebugTextFitterScreen
//================================================================//
export const DebugTextFitterScreen = observer (( props ) => {

    const service = useService (() => new DebugTextFitterService ());

    return (
        <div>
            { service.svg }
        </div>
    );
});
