/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { Service, useService }          from '../Service';
import { SingleColumnContainerView }    from '../SingleColumnContainerView'
import * as qrcode                      from '../util/qrcode';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                     from 'mobx-react';
import React, { useState }              from 'react';
import { Button, Divider, Dropdown, Form, Header, Message, Segment, Select } from 'semantic-ui-react';


const QR_ERR_OPTIONS = [
    { key: 'L', value: 'L', text: 'Low - 7%' },
    { key: 'M', value: 'M', text: 'Medium - 15%' },
    { key: 'Q', value: 'Q', text: 'Quartile - 25%' },
    { key: 'H', value: 'H', text: 'High - 30%' },
];



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

    const overflow = qrcode.calculateOverflow ( qrType, qrErr, data.length );

    if ( qrcode.isLegal ( data )) {

        let autoType = qrType;

        if ( overflow > 0 ) {

            autoType = qrcode.autoSelectType ( qrErr, data.length )
            const autoOverflow = qrcode.calculateOverflow ( autoType, qrErr, data.length );

            if ( autoOverflow ) {
                errorMsg = `Input string exceeds auto-selected QR code type ${ autoType } capacity by ${ autoOverflow } characters.`;
            }
            else {
                errorMsg = `Input string exceeds original QR code type of ${ qrType } capacity by ${ overflow } characters; auto-selected type ${ autoType }.`;
            }
        }

        barcodeSVG = qrcode.makeSVGTag ( data, 0, 0, w, h, qrErr, autoType );
        
        if ( barcodeSVG === false ) {
            errorMsg = 'Error generating QR code.';
        }
    }
    else {
        errorMsg = 'Input string contains illegal characters.';
    }

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
                        onChange = {( event ) => { setQRType ( qrcode.clampType ( event.target.value ))}}
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
