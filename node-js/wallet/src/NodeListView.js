/* eslint-disable no-whitespace-before-property */

import { NODE_TYPE, NODE_STATUS }   from './AppStateService';
import React, { useState }          from 'react';
import { observer }                 from 'mobx-react';
import { Button, Form, Segment, Icon }    from 'semantic-ui-react';
//import validator                    from 'validator';

//================================================================//
// NodeListView
//================================================================//
export const NodeListView = observer (( props ) => {
    
    const [ nodeURL, setNodeURL ] = useState ( '' );

    const appState = props.appState;
    const nodes = appState.nodes;

    //const isEnabled = validator.isURL ( this.state.nodeURL, { protocols: [ 'http','https' ], require_protocol: true });
    const isSubmitEnabled = nodeURL.length > 0;
    const isClearEnabled = nodes.length > 0;

    // TODO: this interface is crap, but it gets us started. what we want is a table of nodes, a status indicator
    // for each node and individual delete buttons for each node.

    const onClickAdd = () => {

        if ( nodeURL.endsWith ( '/' )) {
            appState.affirmNodeURL ( nodeURL.toLowerCase().slice( 0, -1 ));
        }
        else if ( !nodeURL.startsWith ( 'http' )) {
            console.log ( 'URL must start with http or https' );
        }
        else {
            appState.affirmNodeURL ( nodeURL.toLowerCase() );
        }
        setNodeURL ( '' );
    };

    const onClickClear = ( url ) => { appState.deleteNode ( url )};
    let onChange = ( event ) => { setNodeURL ( event.target.value )};

    let urlList = [];
    for ( let url in nodes ) {

        const nodeInfo = appState.getNodeInfo ( url );
        let textColor;
        switch ( nodeInfo.status ) {
            case 'ONLINE':
                textColor = 'green';
                break;
            case 'OFFLINE':
                textColor = 'red';
                break;
            case 'UNKNOWN':
                textColor = 'gray';
                break;
            default:
                textColor = 'pink';
        }
        urlList.push (
            <div style = {{ color: textColor, flex: '1 1 auto', float: 'left' }} key = { urlList.length }>
                <Icon fitted name = 'trash alternate' style = {{ paddingRight: '10px'}} onClick = { () => onClickClear ( url ) }/> { `${ nodeInfo.type } - ${ url }` }
            </div>
        )
    }

    return (

        <div>
            <div style = {{ display: 'flex', flexDirection: 'column', alignItems: 'flex-start', padding: '0 6px', marginBottom: '12px' }}>
                { urlList }
            </div>
            <Form size = "large">
                <Segment stacked>
                    <Form.Input
                        fluid
                        placeholder = "Node URL"
                        name = "nodeURL"
                        value = { nodeURL }
                        onChange = { onChange }
                    />
                    <Button color = "teal" fluid disabled = { !isSubmitEnabled } onClick = { onClickAdd }>
                        Add
                    </Button>
                    { isClearEnabled && <Button color = "red" fluid onClick = { onClickClear }>Clear</Button> }
                </Segment>
            </Form>
        </div>
    );
});
