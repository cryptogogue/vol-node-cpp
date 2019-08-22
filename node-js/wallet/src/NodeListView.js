/* eslint-disable no-whitespace-before-property */

import { NODE_TYPE, NODE_STATUS }   from './AppStateService';
import React, { useState }          from 'react';
import { observer }                 from 'mobx-react';
import { Button, Form, Segment }    from 'semantic-ui-react';
//import validator                    from 'validator';

//================================================================//
// NodeListView
//================================================================//
const NodeListView = observer (( props ) => {
    
    const [ nodeURL, setNodeURL ] = useState ( '' );

    const appState = props.appState;
    const nodes = appState.nodes;

    //const isEnabled = validator.isURL ( this.state.nodeURL, { protocols: [ 'http','https' ], require_protocol: true });
    const isSubmitEnabled = nodeURL.length > 0;
    const isClearEnabled = nodes.length > 0;

    // TODO: this interface is crap, but it gets us started. what we want is a table of nodes, a status indicator
    // for each node and individual delete buttons for each node.

    let onClickAdd = () => {
        appState.affirmNodeURL ( nodeURL );
        setNodeURL ( '' );
    };
    let onClickClear = () => { appState.clearNodeURLs ()};
    let onChange = ( event ) => { setNodeURL ( event.target.value )};

    let urlList = [];
    for ( let url in nodes ) {

        const nodeInfo = appState.getNodeInfo ( url );
        const textColor = nodeInfo.status === NODE_STATUS.OFFLINE ? 'red' : 'black';

        urlList.push (<p style = {{ color: textColor }} key = { urlList.length }>{ `${ nodeInfo.type } - ${ url }` }</p>)
    }

    return (

        <div>

            { urlList }

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

export default NodeListView;
