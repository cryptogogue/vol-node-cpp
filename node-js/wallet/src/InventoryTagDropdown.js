// Copyright (c) 2019 Cryptogogue, Inc. All Rights Reserved.

import _                                                    from 'lodash';
import { action, computed, extendObservable, observable }   from "mobx";
import { observer }                                         from 'mobx-react';
import React, { Fragment, useState }                        from 'react';
import { Link }                                             from 'react-router-dom';
import { Button, Checkbox, Dropdown, Grid, Icon, Input, List, Menu, Modal, Loader } from 'semantic-ui-react';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';

//================================================================//
// InventoryTagDropdown
//================================================================//
export const InventoryTagDropdown = observer (( props ) => {

    const [ tagInput, setTagInput ]     = useState ( '' );
    const [ isOpen, setIsOpen ]         = useState ( false );

    const { controller, tags } = props;

    const onTagInputKey = ( key ) => {
        if ( key === 'Enter' ) {
            tags.affirmTag ( tagInput );
            setTagInput ( '' );
        }
    }

    const tagNames          = tags.tagNames;
    const selectionSize     = controller.selectionSize;

    let tagList = [];
    for ( let tagName of tagNames ) {

        const withTag           = tags.countSelectedAssetsWithTag ( controller.selection, tagName );
        const allTagged         = (( withTag > 0 ) && ( withTag === selectionSize ));
        const noneTagged        = (( withTag > 0 ) && ( withTag === 0 ));
        const indeterminate     = (( withTag > 0 ) && !( allTagged || noneTagged ));

        console.log ( 'TAGS', tagName, selectionSize, withTag, allTagged, noneTagged, indeterminate );

        tagList.push (
            <div
                key = { tagName }
            >
                <Checkbox
                    label           = { tagName }
                    checked         = { allTagged }
                    indeterminate   = { indeterminate }
                    disabled        = { selectionSize === 0 }
                    onChange        = {( event ) => {
                        tags.tagSelection ( controller.selection, tagName, !allTagged );
                        event.stopPropagation ();
                    }}
                />
                <Button
                    icon            = 'trash'
                    onClick         = {() => { tags.deleteTag ( tagName )}}
                />
            </div>
        );
    }

    return (
        <Menu.Item
            onClick = {() => { setIsOpen ( true )}}
        >
            <Icon name = 'tags'/>
            <Modal
                style = {{ height : 'auto' }}
                size = 'small'
                open = { isOpen }
                onClose = {() => {
                    controller.clearSelection ();
                    setIsOpen ( false )
                }}
            >
                <Modal.Content>
                    <div>
                        { tagList }
                        <Input
                            placeholder = 'New Tag...'
                            value = { tagInput }
                            onChange = {( event ) => { setTagInput ( event.target.value )}}
                            onKeyPress = {( event ) => { onTagInputKey ( event.key )}}
                        />
                    </div>
                </Modal.Content>
            </Modal>
        </Menu.Item>
    );
});
