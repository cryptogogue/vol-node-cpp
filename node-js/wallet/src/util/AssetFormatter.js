/* eslint-disable no-whitespace-before-property */

import _                from 'lodash';
import handlebars       from 'handlebars';

function affirmObjectField ( object, fieldName ) {

    let field = object [ fieldName ] || {};
    object [ fieldName ] = field;
    return field;
}

//================================================================//
// AssetFormatter
//================================================================//
export class AssetFormatter {

    //----------------------------------------------------------------//
    applyMeta ( meta ) {

        for ( let templateName in meta.templates ) {
            this.templates [ templateName ] = handlebars.compile ( meta.templates [ templateName ]);
        }
    }

    //----------------------------------------------------------------//
    composeAssetContext ( asset, filters ) {

        let context = {};

        for ( let fieldName in asset.fields ) {

            const field = asset.fields [ fieldName ];
            const alternates = field.alternates;

            context [ fieldName ] = field.value;

            for ( let i in filters ) {
                const filter = filters [ i ];
                if ( _.has ( alternates, filter )) {
                    context [ fieldName ] = alternates [ filter ];
                }
            }
        }
        return context;
    }

    //----------------------------------------------------------------//
    composeAssetLayout ( asset, filters, overrideContext ) {

        let context = this.composeAssetContext ( asset, filters );
        context = Object.assign ( context, overrideContext );

        return this.templates [ context.layout ]( context );
    }

    //----------------------------------------------------------------//
    constructor () {

        this.templates = {};
        this.definitions = {};
        this.filteredDefinitions = {};
    }
}
