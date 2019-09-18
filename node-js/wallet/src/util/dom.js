/* eslint-disable no-whitespace-before-property */

import { Parser }                   from 'saxen';
import _                            from 'lodash';

//================================================================//
// dom
//================================================================//

//----------------------------------------------------------------//
export function parse ( xml, multiroot ) {

    var parser = new Parser ();

    let roots = [];
    let stack = [];
    let top = -1;

    parser.on ( 'openTag', ( elementName, attrGetter, decodeEntities, selfClosing, getContext ) => {

        const element = {
            name:       elementName,
            attr:       attrGetter (),
            children:   [],
        };
        stack.push ( element );
        top++;
    });

    parser.on ( 'text', ( value, decodeEntities, contextGetter ) => {

        stack [ top ].text = value;
    });

    parser.on ( 'closeTag', ( elementName, decodeEntities, selfClosing, contextGetter ) => {

        const element = stack.pop ();
        top--;

        if ( top >= 0 ) {
            stack [ top ].children.push ( element );
        }
        else {
            roots.push ( element );
        }
    });

    parser.parse ( xml );
    return multiroot ? roots : roots [ 0 ];
}
