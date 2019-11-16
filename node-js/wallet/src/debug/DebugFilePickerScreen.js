/* eslint-disable no-whitespace-before-property */

import { Service, useService }              from '../Service';
import { SingleColumnContainerView }        from '../SingleColumnContainerView'
import * as excel                           from '../util/excel'
import { TextFitter, FONT_FACE, JUSTIFY }   from '../util/textLayout';
import handlebars                           from 'handlebars';
import { action, computed, observable, }    from 'mobx';
import { observer }                         from 'mobx-react';
import * as opentype                        from 'opentype.js';
import React, { useState, useRef }          from 'react';
import { Button, Divider, Dropdown, Form, Header, Message, Segment, Select } from 'semantic-ui-react';

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

        const fonts = {
            [ 'roboto' ]: {
                [ FONT_FACE.REGULAR ]: font,
            },
        }

        const text0 = 'This <$#ff0000 125%>is<$> some really <$#ffffff>really<$> <$50%>long text that should <$#00ffff>wrap<$> to the text <$100%>box!';
        const text1 = '<$25%>middle section middle section middle section middle section middle section';
        const text2 = 'This <$#ff0000 125%>is<$> some some <$#0000003e>really<$> <$#ffffff>really<$> <$50%>long text that should <$#00ffff>wrap<$> to the text <$100%>box!';

        let fitter = new TextFitter ( fonts, 0, 0, 200, 600, JUSTIFY.VERTICAL.TOP );
        
        fitter.pushSection ( text0, 'roboto', 42, JUSTIFY.HORIZONTAL.LEFT );
        fitter.pushSection ( text1, 'roboto', 42, JUSTIFY.HORIZONTAL.CENTER );
        fitter.pushSection ( text2, 'roboto', 42, JUSTIFY.HORIZONTAL.RIGHT );
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
// DebugFilePickerScreen
//================================================================//
export const DebugFilePickerScreen = observer (( props ) => {

    const [ file, setFile ]                 = useState ( false );
    const [ fileString, setFileString ]     = useState ( '' );
    const filePickerRef                     = useRef ();
    const service                           = useService (() => new DebugOpentypeService ());

    const reloadFile = ( picked ) => {

        if ( !picked ) return;

        const reader = new FileReader ();
        reader.onabort = () => { console.log ( 'file reading was aborted' )}
        reader.onerror = () => { console.log ( 'file reading has failed' )}
        reader.onload = () => {
            setFileString ( reader.result );
        }
        reader.readAsBinaryString ( picked );

        // const reader = new FileReader ();

        // reader.onabort = () => { console.log ( 'file reading was aborted' )}
        // reader.onerror = () => { console.log ( 'file reading has failed' )}
        // reader.onload = () => {
        //     const book = new excel.Workbook ( reader.result, { type: 'binary' });
        //     const layoutSheet = book.getSheet ( 'layouts' );
        //     console.log ( layoutSheet );
        // }
        // reader.readAsBinaryString ( file );
    }

    const onFilePickerChange = ( event ) => {
        const picked = event.target.files.length > 0 ? event.target.files [ 0 ] : false;
        if ( picked ) {
            setFile ( picked );
            reloadFile ( picked );
        }
    }

    const hasFile = ( file !== false );

    return (
        
        <div>
            <SingleColumnContainerView title = 'Test File Picker'>

                <input
                    style = {{ display:'none' }}
                    ref = { filePickerRef }
                    type = 'file'
                    onChange = { onFilePickerChange }
                />
            
                <Form size = 'large' error = { false }>
                    <Segment stacked>
                    <Button
                        fluid
                        onClick = {() => filePickerRef.current.click ()}
                    >
                        Choose File
                    </Button>

                    <div className = 'ui hidden divider' ></div>

                    <Button
                        fluid
                        disabled = { !hasFile }
                        onClick = {() => { reloadFile ( file )}}
                    >
                        { hasFile ? file.name : 'No File Chosen' }
                    </Button>
                    </Segment>
                    <Message
                        error
                        header  = 'QR Code Error'
                        content = { '' }
                    />
                </Form>

            </SingleColumnContainerView>

            <div>
                { fileString }
            </div>
        </div>
    );
});
