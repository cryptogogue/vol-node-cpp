/* eslint-disable no-whitespace-before-property */

import { TransactionListView }      from './TransactionListView';
import React, { useState }          from 'react';
import { observer }                 from 'mobx-react';
import { Button, Divider, Form, Header, Icon, Input, Label, Modal, Segment } from 'semantic-ui-react';

const NETWORK_NAME_REGEX     = /^[a-z0-9]+[a-z0-9-]*$/;

//================================================================//
// AddNetworkModal
//================================================================//
export const AddNetworkModal = observer (( props ) => {

    const { appState, trigger } = props;

    const [ name, setName ] = useState ( '' );
    const [ nameError, setNameError ] = useState ( '' );
    const [ nodeURL, setNodeURL ] = useState ( '' );
    const [ testURL, setTestURL ] = useState ( '' );
    const [ nodeUrlError, setNodeUrlError ] = useState ( '' );

    let onClickSubmit = () => { appState.submitTransactions ( password )};
    let onClickClear = () => { appState.clearStagedTransactions ()};

    let onChangeName = ( event ) => {
        const newName = event.target.value;
        setName ( newName )

        let err = '';
        if ( newName && !NETWORK_NAME_REGEX.test ( newName )) {
            err = `Network names must start with a [a-z] or [0-9] and contain only [a-z], [0-9] and '-'.`
        }
        setNameError ( err );
    };

    let onChangeNodeURL = ( event ) => {

        let url = event.target.value;
        setNodeURL ( url );

        if ( url ) {
            url = url.toLowerCase ();
            if ( url.startsWith ( 'http://' ) || url.startsWith ( 'https://' )) {
                setTestURL ( url.endsWith ( '/' ) ? url : `${ url }/` );
            }
        }
    };

    let onCheckNodeURL = () => {
    };

    const hasTestURL = Boolean ( testURL );
    const submitEnabled = name && !( nameError || nodeUrlError );
    
    return (

        <Modal
            size = 'small'
            closeIcon
            trigger = { trigger }
        >
            <Modal.Header>Add Network</Modal.Header>

            <Modal.Content>
                <Form>

                    <Form.Field>
                        <Input
                            fluid
                            placeholder = "Network Name"
                            type = "text"
                            value = { name }
                            onChange = { onChangeName }
                        />
                        { nameError && <Label pointing prompt>{ nameError }</Label> }
                    </Form.Field>

                    <Form.Field>
                        <Input
                            fluid
                            action = {
                                <Button
                                    icon = 'sync alternate'
                                    disabled = { !hasTestURL }
                                    onClick = { onCheckNodeURL }
                                />
                            } 
                            placeholder = "Node URL"
                            name = "nodeURL"
                            type = "url"
                            value = { nodeURL }
                            onChange = { onChangeNodeURL }
                        />
                        { nodeUrlError && <Label pointing prompt>{ nodeUrlError }</Label> }
                    </Form.Field>
                </Form>
            </Modal.Content>

            <Modal.Actions>
                <Button
                    positive
                    disabled = { !submitEnabled }
                    onClick = { onClickSubmit }>Submit
                </Button>
            </Modal.Actions>
        </Modal>
    );
});
