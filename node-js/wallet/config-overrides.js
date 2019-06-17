
const {
    override,
    disableEsLint,
    addDecoratorsLegacy,
    addBabelPlugins,
    addBabelPresets,
    fixBabelImports,
    addLessLoader,
} = require ( "customize-cra" );


module.exports = override (
        disableEsLint (),
        addDecoratorsLegacy (),
        ...addBabelPlugins (
        "babel-plugin-styled-components"
    ),
    fixBabelImports ( "react-app-rewire-mobx", {
        libraryDirectory: "",
        camel2DashComponentName: false
    })
);
