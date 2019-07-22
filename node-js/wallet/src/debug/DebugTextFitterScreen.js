/* eslint-disable no-whitespace-before-property */

import { Service, useService }              from '../stores/Service';
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
        width="200"
        height="200"
        viewBox="0 0 200 200"
        preserveAspectRatio="none"
        >
        <rect width="100%" height="100%" fill="red" />
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

        const text = 'This is some really really long text that should wrap to the text box!';

        let fitter = new TextFitter ( font, 42, 0, 0, 200, 200, JUSTIFY.HORIZONTAL.LEFT, JUSTIFY.VERTICAL.TOP );
        fitter.fitDynamic ( text );
        console.log ( 'FITERATIONS:', fitter.fitIterations );

        this.setSVG ( SVG_TEMPLATE ({ text: fitter.toSVG ()}));
    }
}

//================================================================//
// DebugOpentypeScreen
//================================================================//
const DebugOpentypeScreen = observer (( props ) => {

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

export default DebugOpentypeScreen;