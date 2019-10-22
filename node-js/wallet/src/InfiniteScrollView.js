import React, { useState, useRef, useEffect, useLayoutEffect }  from 'react';
import { Link }                                                 from 'react-router-dom';
import { Dropdown, Grid, Icon, List, Menu, Card, Group }        from 'semantic-ui-react';

import { FixedSizeList }                                        from 'react-window';
import AutoSizer                                                from 'react-virtualized-auto-sizer';
import InfiniteLoader                                           from 'react-window-infinite-loader';

const CARD_PADDING = 4; // for whatever reason, cards get wrapped in the row even if they "fit" (according to the measurements).

//================================================================//
// InfiniteScrollView
//================================================================//
export const InfiniteScrollView = ( props ) => {

    const [ rowWidth, setRowWidth ]         = useState ( 0 );
    const [ cardWidth, setCardWidth ]       = useState ( 0 );
    const [ cardHeight, setCardHeight ]     = useState ( 0 );

    const hasDimensions = (( rowWidth > 0 ) && ( cardWidth > 0 ) && ( cardHeight > 0 )); 

    const getAsset      = props.onGetAsset;
    const totalCards    = hasDimensions ? props.totalCards : 1;
    const cardsPerRow   = hasDimensions ? Math.floor ( rowWidth / cardWidth ) : 1;
    const totalRows     = hasDimensions ? Math.ceil ( totalCards / cardsPerRow ) : 1;    

    const onResize = ({ width, height }) => {
        console.log ( 'RESIZE', width, height );
        setRowWidth ( width );
    }

    const rowFactory = ({ index, style }) => {

        let cardRef = null;

        if ( !hasDimensions ) {
            cardRef = useRef ();
            useLayoutEffect (() => {
                if ( cardRef.current ) {
                    // use the offset/outer width
                    setCardWidth ( cardRef.current.offsetWidth + CARD_PADDING );
                    setCardHeight ( cardRef.current.offsetHeight + CARD_PADDING );
                }
            });
        }

        let cards = [];
        for ( let i = 0; i < cardsPerRow; ++i ) {
            const assetID = i + ( index * cardsPerRow );
            if ( assetID < totalCards ) {
                cards.push (
                    <div
                        style = {{ float: 'left' }}
                        ref = {( i === 0 ) ? cardRef : null }
                        key = { assetID }
                    >
                        { getAsset ( assetID )}
                    </div>
                );
            }
        }

        let centerFromLeft = hasDimensions ? ( rowWidth - ( cardsPerRow * cardWidth )) / 2 : 0;

        return (
            <div style = { style }>
                <div style = {{
                    float: 'left',
                    position: 'relative',
                    left: centerFromLeft,
                }}>
                    { cards }
                </div>
            </div>
        );
    }

    return (
        <AutoSizer
            onResize = { onResize }
        >
            {({ width, height }) => (
                <FixedSizeList
                    height = { height }
                    itemCount = { totalRows }
                    itemSize = { hasDimensions ? cardHeight : height }
                    width = { width }
                >
                    { rowFactory }
                </FixedSizeList>
            )}
        </AutoSizer>
    );
}
