/* eslint-disable no-whitespace-before-property */

import { AssetView }                        from '../AssetView';
import { InventoryService }                 from '../InventoryService';
import { InventoryView }                    from '../InventoryView';
import { Service, useService }              from '../Service';
import { SingleColumnContainerView }        from '../SingleColumnContainerView'
import * as textStyle                       from '../util/textStyle';
import { action, computed, extendObservable, runInAction, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Card, Divider, Dropdown, Form, Grid, Header, Icon, Message, Modal, Segment, TextArea } from 'semantic-ui-react';

//================================================================//
// DebugTextStyleScreen
//================================================================//
export const DebugTextStyleScreen = observer (( props ) => {

    const [ text, setText ]     = useState ( '' );

    const onSubmit = () => {
        textStyle.parse ( text );
    }

    return (
        <div>
            <SingleColumnContainerView>
                <Segment>
                    <Form>
                    
                        <TextArea
                            rows = { 8 }
                            value = { text }
                            onChange = {( event ) => { setText ( event.target.value )}}
                        />

                        <div className = "ui hidden divider" ></div>

                        <Button type = 'button' color = "teal" fluid onClick = { onSubmit }>
                            Refresh
                        </Button>
                    </Form>
                </Segment>

            </SingleColumnContainerView>
        </div>
    );
});
