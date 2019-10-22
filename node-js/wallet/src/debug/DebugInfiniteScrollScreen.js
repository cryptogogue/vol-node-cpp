import { InfiniteScrollView }                               from '../InfiniteScrollView';
import React, { useState, useRef, useLayoutEffect }         from 'react';
import { Dropdown, Grid, Icon, List, Menu, Card, Group }    from 'semantic-ui-react';

const CARD_WIDTH  = 194;
const CARD_HEIGHT = 266;
const CARD_MARGIN = 6;
const TOTAL_CARDS = 1000;

const cardArray = [];
for ( let i = 0; i < TOTAL_CARDS; ++i) {
    cardArray.push (
        <svg width = { CARD_WIDTH } height = { CARD_HEIGHT } key = { i }>
            <rect width = { CARD_WIDTH - CARD_MARGIN } height = { CARD_HEIGHT - CARD_MARGIN } fill = "blue" stroke = "rgb( 0,0,0 )" strokeWidth = "3" />
        </svg>
    )
}

const getCard = ( i ) => {
    return cardArray [ i ];
}

export const DebugInfiniteScrollScreen = () => {
    return (
        <div style = {{ height: '100vh' }}>
            <InfiniteScrollView 
                onGetAsset  = { getCard }
                totalCards  = { cardArray.length }
            />
        </div>
    );
}