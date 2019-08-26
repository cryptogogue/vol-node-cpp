/* eslint-disable no-whitespace-before-property */

import handlebars       from 'handlebars';
import React            from 'react';

//================================================================//
// DebugHandlebarsScreen
//================================================================//
export const DebugHandlebarsScreen = ( props ) => {

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
            <div dangerouslySetInnerHTML = {{ __html: result }}/>
            <div dangerouslySetInnerHTML = {{ __html: svgResult }}/>
        </div>
    );
}
