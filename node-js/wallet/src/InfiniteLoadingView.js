import { AppStateService }                                  from './AppStateService';
import { AssetView }                                        from './AssetView';
import { InventoryService }                                 from './InventoryService';
import { NavigationBar }                                    from './NavigationBar';
import { Service, useService }                              from './Service';
import * as util                                            from './util/util';
import handlebars                                           from 'handlebars';
import { action, computed, extendObservable, observable }   from 'mobx';
import { observer }                                         from 'mobx-react';
import React, { useState, PureComponent }                                  from 'react';
import { Link }                                             from 'react-router-dom';
import { Dropdown, Grid, Icon, List, Menu, Card, Group }    from 'semantic-ui-react';

import { FixedSizeList } from 'react-window';
import InfiniteLoader from 'react-window-infinite-loader';
import AutoSizer from 'react-virtualized-auto-sizer';


//================================================================//
// InfiniteLoadingView
//================================================================//

const LOADING = 1;
const LOADED = 2;
let itemStatusMap = {};

const isItemLoaded = index => !!itemStatusMap [ index ];
const loadMoreItems = ( startIndex, stopIndex ) => {
    for ( let index = startIndex; index <= stopIndex; index++ ) {
        itemStatusMap [ index ] = LOADING;
    }
    return new Promise ( resolve => {
        for ( let index = startIndex; index <= stopIndex; index++ ) {
            itemStatusMap [ index ] = LOADED;
        }
        resolve();
    });
};

class Row extends PureComponent {
    render() {
        const { index, style } = this.props;
        let label;
        if ( itemStatusMap [ index ] === LOADED ) {
            label = `Row ${ index }`;
        } else {
            label = "Loading...";
        }
        return (
            <div className = "ListItem" style = { style }>
                { label }
            </div>
        );
    }
}