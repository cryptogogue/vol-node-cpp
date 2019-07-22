/* eslint-disable no-whitespace-before-property */

import handlebars                                   from 'handlebars';
import React, { useEffect, useState }               from 'react';
import { Button, Form, Grid, Header, Segment }      from 'semantic-ui-react';

//================================================================//
// DebugPrintScreen
//================================================================//
const DebugPrintScreen = ( props ) => {

    // TODO: this is a hacky hammer approach. can redo with CSS media queries.

    const [ isPrinting, setIsPrinting ] = useState ( false );

    useEffect (() => {
        if ( isPrinting > 0 ) {
            window.print ();
            setIsPrinting ( false );
        }
    });

    let template = handlebars.compile ( '<h3>{{ message }}</h3>' );
    let result = template ({ message: 'Hello from Handlebars!' });

    let svgTemplate = handlebars.compile ( `
        <svg version="1.1" baseProfile="full" width="200" height="200" xmlns="http://www.w3.org/2000/svg">
            <rect width="100%" height="100%" fill="red" />
            <circle cx="100" cy="100" r="80" fill="black" />
            <text x="100" y="125" font-size="60" text-anchor="middle" fill="white">{{ message }}</text>
        </svg>
    `);
    let svgResult = svgTemplate ({ message: 'VOL' });

    return (
        <div>
            <If condition = { isPrinting == 0 }>
                <Button onClick = {() => { setIsPrinting ( true )}}>Print</Button>
                <div dangerouslySetInnerHTML = {{ __html: result }}/>
            </If>
            <div dangerouslySetInnerHTML = {{ __html: svgResult }}/>
        </div>
    );
}

export default DebugPrintScreen;
