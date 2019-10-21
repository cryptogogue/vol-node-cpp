import React, { useState, useRef, useLayoutEffect }         from 'react';
import { Link }                                             from 'react-router-dom';
import { Dropdown, Grid, Icon, List, Menu, Card, Group }    from 'semantic-ui-react';

import { FixedSizeList }                                    from 'react-window';
import AutoSizer                                            from 'react-virtualized-auto-sizer';
import InfiniteLoader                                       from 'react-window-infinite-loader';


//================================================================//
// InfiniteScrollView
//================================================================//

export const InfiniteScrollView = ( props ) => {

    const [ totalRows, setTotalRows ]       = useState ( 1 );
    const [ cardsPerRow, setCardsPerRow ]   = useState ( 0 );
    const [ rowWidth, setRowWidth ]         = useState ( 0 );

    const getAsset      = props.onGetAsset;
    const cardWidth     = props.cardWidth;
    const cardHeight    = props.cardHeight;
    const totalCards    = props.totalCards;

    const makeRow = ({ index, style }) => {
        const targetRef                 = useRef ();
        const RESET_TIMEOUT             = 100;
        let movement_timer              = null;

        const checkRowWidth = () => {
            if ( targetRef.current && targetRef.current.offsetWidth != rowWidth ) {
                setRowWidth ( targetRef.current.offsetWidth );
                setCardsPerRow ( Math.floor ( rowWidth / cardWidth ));
                setTotalRows ( Math.ceil ( totalCards / cardsPerRow ) );
            }
        }

        useLayoutEffect (() => {
            checkRowWidth ();
        }, []);

        window.addEventListener ( 'resize', () => {
            clearInterval ( movement_timer );
            movement_timer = setTimeout ( checkRowWidth, RESET_TIMEOUT );
        });

        
        let cards = [];

        if ( rowWidth > 0 ) {
            console.log ( 'Cards Per Row', cardsPerRow );
            for ( let i = 0; i < cardsPerRow; ++i ) {
                const assetID = i + ( index * cardsPerRow );
                if ( assetID < totalCards ) {
                    cards.push ( getAsset ( assetID ));
                }
            }
        }

        return (
            <div style = { style } ref = { targetRef }>
                { cards }
            </div>
        )
    }

    return (
        <div className="AutoSizerWrapper">
            <AutoSizer>
                {({ width, height }) => (
                    <FixedSizeList
                        className = "inventoryList"
                        height = { height }
                        itemCount = { totalRows }
                        itemSize = { cardHeight }
                        width = { width }
                    >
                        { makeRow }
                    </FixedSizeList>
                )}
            </AutoSizer>
        </div>
    );
}