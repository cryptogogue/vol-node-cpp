/* eslint-disable no-whitespace-before-property */

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

        for ( let definitionName in meta.definitions ) {
            const definition = meta.definitions [ definitionName ];

            let plainDefinition = {};
            this.definitions [ definitionName ] = plainDefinition;


            for ( let fieldName in definition ) {
                const field = definition [ fieldName ];

                if ( typeof ( field ) === 'object' ) {

                    for ( let filter in field ) {

                        let definitionsForFilter = affirmObjectField ( this.filteredDefinitions, filter );
                        let filteredDefinition = affirmObjectField ( definitionsForFilter, definitionName );
                        filteredDefinition [ fieldName ] = field [ filter ];
                    }
                }
                else {
                    plainDefinition [ fieldName ] = field;
                }
            }
        }

        console.log ( this );
    }

    //----------------------------------------------------------------//
    composeAsset ( asset, filters ) {

        asset = Object.assign ({}, asset );
        const assetType = asset.type;

        const definition = this.definitions [ asset.type ];
        if ( definition ) {
            asset = Object.assign ( asset, definition );
        }

        for ( let i in filters ) {
            const definitionsForFilter = this.filteredDefinitions [ filters [ i ]];
            if ( definitionsForFilter ) {
                const filteredDefinition = definitionsForFilter [ assetType ];
                asset = Object.assign ( asset, filteredDefinition );
            }
        }
        return asset;
    }

    //----------------------------------------------------------------//
    composeAssetLayout ( asset, filters ) {

        return this.formatAssetLayout ( this.composeAsset ( asset, filters ));
    }

    //----------------------------------------------------------------//
    constructor () {

        this.templates = {};
        this.definitions = {};
        this.filteredDefinitions = {};
    }

    //----------------------------------------------------------------//
    formatAssetLayout ( asset ) {

        return this.templates [ asset.template ]( asset );
    }
}
