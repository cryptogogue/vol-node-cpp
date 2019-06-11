
function main ( ledger, caller, assetArgs, constArgs )
    print ( 'openPack', caller )
    for arg, asset in pairs ( assetArgs ) do
        print ( 'ARG:', arg )
        print ( 'OWNER', asset.owner )
        print ( 'TYPE', asset.type )
        for name, field in pairs ( asset.fields ) do
            print ( '   FIELD', name, field )
        end
    end
end
