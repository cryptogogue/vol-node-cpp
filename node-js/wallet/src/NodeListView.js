/* eslint-disable no-whitespace-before-property */

import { withAppState }             from './AppStateProvider';
import React, { Component }         from 'react';
import { Button, Form, Segment }    from 'semantic-ui-react';
//import validator                    from 'validator';

//================================================================//
// NodeListView
//================================================================//
class NodeListView extends Component {

    //----------------------------------------------------------------//
    affirmNodeURL () {

        this.props.appState.affirmNodeURL ( this.state.nodeURL );
        this.setState ({ nodeURL : '' });
    }

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.state = {
            nodeURL : '',
        };
    }
  
    //----------------------------------------------------------------//
    handleChange ( event ) {

        this.setState ({[ event.target.name ]: event.target.value });
    }

    //----------------------------------------------------------------//
    clearNodeURLs () {

        this.props.appState.clearNodeURLs ();
    }

    //----------------------------------------------------------------//
    render () {
        
        const { appState } = this.props;
        const { nodes } = appState.state;

        //const isEnabled = validator.isURL ( this.state.nodeURL, { protocols: [ 'http','https' ], require_protocol: true });
        const isSubmitEnabled = this.state.nodeURL.length > 0;
        const isClearEnabled = nodes.length > 0;

        // TODO: this interface is crap, but it gets us started. what we want is a table of nodes, a status indicator
        // for each node and individual delete buttons for each node.

        let onClickAdd      = () => { this.affirmNodeURL ()};
        let onClickClear    = () => { this.clearNodeURLs ()};
        let onChange        = ( event ) => { this.handleChange ( event )};

        let urlList = [];
        for ( let idx in nodes ) {
            urlList.push (<p key = { 'url' + idx }>{ nodes [ idx ]}</p>)
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
                            value = { this.state.nodeURL }
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
    }
}

export default withAppState ( NodeListView );
