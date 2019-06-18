/* eslint-disable no-whitespace-before-property */

const schema    = require ( './schema' );

const Binding           = require ( './binding' ).Binding;
const Schema            = require ( './schema' ).Schema;
const buildSchema       = require ( './schema-builder' );
const op                = buildSchema.op;

var exports = module.exports = {
    Binding:        Binding,
    Schema:         Schema,
    buildSchema:    buildSchema,
}
