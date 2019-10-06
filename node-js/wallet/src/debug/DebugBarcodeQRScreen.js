/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { Service, useService }          from '../Service';
import { SingleColumnContainerView }    from '../SingleColumnContainerView'
import { bitmapToSVG, }                 from '../util/bitmapToPaths';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                     from 'mobx-react';
import * as qrcode                       from 'qrcode-generator';
import React, { useState }              from 'react';
import { Button, Divider, Dropdown, Form, Header, Message, Segment, Select } from 'semantic-ui-react';

const alphaNumericCapacity = [];

alphaNumericCapacity [ 1 ] = [ 25, 20, 16, 10, ];
alphaNumericCapacity [ 2 ] = [ 47, 38, 29, 20, ];
alphaNumericCapacity [ 3 ] = [ 77, 61, 47, 35, ];
alphaNumericCapacity [ 4 ] = [ 114, 90, 67, 50, ];
alphaNumericCapacity [ 5 ] = [ 154, 122, 87, 64, ];
alphaNumericCapacity [ 6 ] = [ 195, 154, 108, 84, ];
alphaNumericCapacity [ 7 ] = [ 224, 178, 125, 93, ];
alphaNumericCapacity [ 8 ] = [ 279, 221, 157, 122, ];
alphaNumericCapacity [ 9 ] = [ 335, 262, 189, 143, ];
alphaNumericCapacity [ 10 ] = [ 395, 311, 221, 174, ];
alphaNumericCapacity [ 11 ] = [ 468, 366, 259, 200, ];
alphaNumericCapacity [ 12 ] = [ 535, 419, 296, 227, ];
alphaNumericCapacity [ 13 ] = [ 619, 483, 352, 259, ];
alphaNumericCapacity [ 14 ] = [ 667, 528, 376, 283, ];
alphaNumericCapacity [ 15 ] = [ 758, 600, 426, 321, ];
alphaNumericCapacity [ 16 ] = [ 854, 656, 470, 365, ];
alphaNumericCapacity [ 17 ] = [ 938, 734, 531, 408, ];
alphaNumericCapacity [ 18 ] = [ 1046, 816, 574, 452, ];
alphaNumericCapacity [ 19 ] = [ 1153, 909, 644, 493, ];
alphaNumericCapacity [ 20 ] = [ 1249, 970, 702, 557, ];
alphaNumericCapacity [ 21 ] = [ 1352, 1035, 742, 587, ];
alphaNumericCapacity [ 22 ] = [ 1460, 1134, 823, 640, ];
alphaNumericCapacity [ 23 ] = [ 1588, 1248, 890, 672, ];
alphaNumericCapacity [ 24 ] = [ 1704, 1326, 963, 744, ];
alphaNumericCapacity [ 25 ] = [ 1853, 1451, 1041, 779, ];
alphaNumericCapacity [ 26 ] = [ 1990, 1542, 1094, 864, ];
alphaNumericCapacity [ 27 ] = [ 2132, 1637, 1172, 910, ];
alphaNumericCapacity [ 28 ] = [ 2223, 1732, 1263, 958, ];
alphaNumericCapacity [ 29 ] = [ 2369, 1839, 1322, 1016, ];
alphaNumericCapacity [ 30 ] = [ 2520, 1994, 1429, 1080, ];
alphaNumericCapacity [ 31 ] = [ 2677, 2113, 1499, 1150, ];
alphaNumericCapacity [ 32 ] = [ 2840, 2238, 1618, 1226, ];
alphaNumericCapacity [ 33 ] = [ 3009, 2369, 1700, 1307, ];
alphaNumericCapacity [ 34 ] = [ 3183, 2506, 1787, 1394, ];
alphaNumericCapacity [ 35 ] = [ 3351, 2632, 1867, 1431, ];
alphaNumericCapacity [ 36 ] = [ 3537, 2780, 1966, 1530, ];
alphaNumericCapacity [ 37 ] = [ 3729, 2894, 2071, 1591, ];
alphaNumericCapacity [ 38 ] = [ 3927, 3054, 2181, 1658, ];
alphaNumericCapacity [ 39 ] = [ 4087, 3220, 2298, 1774, ];
alphaNumericCapacity [ 40 ] = [ 4296, 3391, 2420, 1852, ];

const QR_ERR_TO_INDEX = {
    L:  0,
    M:  1,
    Q:  2,
    H:  3,
}

const QR_ERR_OPTIONS = [
    { key: 'L', value: 'L', text: 'Low - 7%' },
    { key: 'M', value: 'M', text: 'Medium - 15%' },
    { key: 'Q', value: 'Q', text: 'Quartile - 25%' },
    { key: 'H', value: 'H', text: 'High - 30%' },
];

const QR_LEGAL_CHARS = /^[0-9A-Z$%*+-./ ]*$/;

//----------------------------------------------------------------//
function clampQRType ( type ) {

    type = type < 1 ? 1 : type;
    type = type > 40 ? 40 : type;
    return type;
}

//----------------------------------------------------------------//
function getCapacity ( qrType, qrErr ) {

    const i = QR_ERR_TO_INDEX [ qrErr ];
    return alphaNumericCapacity [ qrType ][ i ];
}

//================================================================//
// DebugBarcodeQRScreen
//================================================================//
export const DebugBarcodeQRScreen = observer (( props ) => {

    const [ data, setData ]             = useState ( 'XXXXX.XXXXX.XXXXX-123' );
    const [ docWidth, setDocWidth ]     = useState ( 0.25 );
    const [ docHeight, setDocHeight ]   = useState ( 0.25 );
    const [ qrType, setQRType ]         = useState ( 1 );
    const [ qrErr, setQRErr ]           = useState ( 'L' );

    let errorMsg        = '';
    let barcodeSVG      = '<g/>';

    const w             = docWidth * 100;
    const h             = docWidth * 100;

    const capacity      = getCapacity ( qrType, qrErr );
    const overflow      = data.length > capacity ? data.length - capacity : 0;

    if ( QR_LEGAL_CHARS.test ( data )) {

        try {
            const qr = qrcode ( qrType, qrErr );
            qr.addData ( data.substring ( 0, capacity ), 'Alphanumeric' );
            qr.make ();

            const moduleCount = qr.getModuleCount ();
            console.log ( 'MODULE COUNT', moduleCount );

            const sampler = ( x, y ) => {
                return qr.isDark ( y, x );
            }

            barcodeSVG = bitmapToSVG ( sampler, moduleCount, moduleCount, 0, 0, w, h );
            
            if ( overflow > 0 ) {
                errorMsg = `Input string exceeds QR code capacity by ${ overflow } characters.`;
            }
        }
        catch ( error ) {
            errorMsg = 'Error generating QR code.';
        }
    }
    else {
        errorMsg = 'Input string contains illegal characters.';
    }

    console.log ( barcodeSVG );

    return (
        <SingleColumnContainerView title = 'Test QR Code'>
            <Form size = 'large' error = { errorMsg.length > 0 }>
                <Segment stacked>
                    <Form.TextArea
                        rows = { 5 }
                        placeholder = "Barcode Data"
                        value = { data }
                        onChange = {( event ) => { setData ( event.target.value.toUpperCase ())}}
                    />
                    <Form.Input
                        fluid
                        type = 'number'
                        value = { docWidth }
                        onChange = {( event ) => { setDocWidth ( event.target.value )}}
                    />
                    <Form.Input
                        fluid
                        type = 'number'
                        value = { docHeight }
                        onChange = {( event ) => { setDocHeight ( event.target.value )}}
                    />
                    <Form.Input
                        fluid
                        type = 'number'
                        value = { qrType }
                        onChange = {( event ) => { setQRType ( clampQRType ( event.target.value ))}}
                    />
                    <Select
                        fluid
                        value = { qrErr }
                        options = { QR_ERR_OPTIONS }
                        onChange = {( value, text ) => { setQRErr ( text.value )}}
                    />
                    <Message
                        error
                        header  = 'QR Code Error'
                        content = { errorMsg }
                    />
                    <div className = 'ui hidden divider' ></div>
                    <svg
                        version = '1.1'
                        baseProfile = 'basic'
                        xmlns = 'http://www.w3.org/2000/svg'
                        xmlnsXlink = 'http://www.w3.org/1999/xlink'
                        width = { `${ docWidth }in` }
                        height = { `${ docHeight }in` }
                        viewBox = { `0 0 ${ w } ${ h }` }
                        preserveAspectRatio = 'none'
                    >
                        <g>
                            <rect x = '0' y = '0' width = { w } height = { h } fill = 'white'/>
                            <g dangerouslySetInnerHTML = {{ __html: barcodeSVG }}/>
                        </g>
                    </svg>
                </Segment>
            </Form>
        </SingleColumnContainerView>
    );
});
