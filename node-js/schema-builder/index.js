/* eslint-disable no-whitespace-before-property */

const schema    = require ( './schema' );

const Inventory         = require ( './inventory' ).Inventory;
const Schema            = require ( './schema' ).Schema;
const buildSchema       = require ( './schema-builder' );
const op                = buildSchema.op;

var exports = module.exports = {
    Inventory:      Inventory,
    Schema:         Schema,
    buildSchema:    buildSchema,
}
