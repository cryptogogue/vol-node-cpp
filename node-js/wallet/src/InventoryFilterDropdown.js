// Copyright (c) 2019 Cryptogogue, Inc. All Rights Reserved.

import _                                                    from 'lodash';
import { action, computed, extendObservable, observable }   from "mobx";
import { observer }                                         from 'mobx-react';
import React, { Fragment, useState }                        from 'react';
import { Link }                                             from 'react-router-dom';
import { Button, Checkbox, Dropdown, Grid, Icon, Input, List, Menu, Modal, Loader } from 'semantic-ui-react';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, util } from 'fgc';

//================================================================//
// InventoryFilterDropdown
//================================================================//
export const InventoryFilterDropdown = observer (( props ) => {

    const { tags } = props;

    const tagNames = tags.tagNames;

    let options = [];

    options.push (
        <Dropdown.Item
            key         = { '' }
            icon        = 'eye'
            onClick     = {() => { tags.setFilter ( '' )}}
        />
    );

    for ( let tagName of tagNames ) {
        options.push (
            <Dropdown.Item
                key         = { tagName }
                text        = { tagName }
                onClick     = {() => { tags.setFilter ( tagName )}}
            />
        );
    }

    return (
        <Dropdown
            item
            icon = { tags.filter === '' ? 'eye' : null }
            text = { tags.filter }
        >
            <Dropdown.Menu>
                { options }
            </Dropdown.Menu>
        </Dropdown>
    );
});
