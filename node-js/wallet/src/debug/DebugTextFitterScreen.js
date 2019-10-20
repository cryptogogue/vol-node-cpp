/* eslint-disable no-whitespace-before-property */

import { Service, useService }              from '../Service';
import { TextFitter, JUSTIFY }              from '../util/TextFitter';
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

//================================================================//
// DebugOpentypeService
//================================================================//
class DebugOpentypeService extends Service {

    @observable svg = '<svg/>';

    //----------------------------------------------------------------//
    constructor ( values ) {
        super ();
    }

    //----------------------------------------------------------------//
    @action
    fetchFontFile ( url ) {

        this.revocableFetch ( url )
        .then (( response ) => {
            response.arrayBuffer ()
            .then (( buffer ) => {
                let font = opentype.parse ( buffer );
                this.testFont ( font );
            });
        })
        .catch (( error ) => {
            console.log ( error );
        });
    }

    //----------------------------------------------------------------//
    @action
    loadFontFile ( fontFile ) {

        const reader = new FileReader ();

        reader.onabort = () => { console.log ( 'file reading was aborted' )}
        reader.onerror = () => { console.log ( 'file reading has failed' )}
        reader.onload = () => {
          let font = opentype.parse ( reader.result );
          this.testFont ( font );
        }

        reader.readAsArrayBuffer ( fontFile );
    }

    //----------------------------------------------------------------//
    @action
    setSVG ( svg ) {
        this.svg = svg;
    }

    //----------------------------------------------------------------//
    @action
    testFont ( font ) {

        const text0 = 'This <$#ff0000 1.25%>is<$> some really <$#ffffff>really<$> <$0.5%>long text that should <$#00ffff>wrap<$> to the text <$0.75>box!';
        const text1 = '<$0.25%>middle section middle section middle section middle section middle section';
        const text2 = 'This <$#ff0000 1.25%>is<$> some really <$#ffffff>really<$> <$0.5%>long text that should <$#00ffff>wrap<$> to the text <$0.75>box!';

        let fitter = new TextFitter ( 0, 0, 200, 600, JUSTIFY.VERTICAL.TOP );
        fitter.pushSection ( text0, font, 42, JUSTIFY.HORIZONTAL.LEFT )
        fitter.pushSection ( text1, font, 42, JUSTIFY.HORIZONTAL.CENTER )
        fitter.pushSection ( text2, font, 42, JUSTIFY.HORIZONTAL.RIGHT )
        fitter.fit ( 0 );
        console.log ( 'FITERATIONS:', fitter.fitIterations, fitter.fontScale );

        this.setSVG ( SVG_TEMPLATE ({
            text: fitter.toSVG (),
            width: 200,
            height: 600,
        }));
    }
}

//================================================================//
// DebugTextFitterScreen
//================================================================//
export const DebugTextFitterScreen = observer (( props ) => {

    const service = useService (() => new DebugOpentypeService ());

    const onChange = ( event ) => {
        const file = event.target.files.length > 0 ? event.target.files [ 0 ] : false;
        if ( file ) {
            service.loadFontFile ( file );
        }
    }

    return (
        <div>
            <label>Choose a font file:</label>
            <input type = "file"
                id = "font"
                name = "font"
                accept = ".ttf, .otf"
                onChange = { onChange }
            />
            <div dangerouslySetInnerHTML = {{ __html: service.svg }}/>
        </div>
    );
});
