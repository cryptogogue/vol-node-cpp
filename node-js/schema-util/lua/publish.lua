
function main ( ledger, caller )
    print ( 'publish schema', caller )
    awardAsset ( ledger, caller, 'pack', 1 )
    awardAsset ( ledger, caller, 'common', 2 )
    awardAsset ( ledger, caller, 'rare', 2 )
    awardAsset ( ledger, caller, 'ultraRare', 1 )
end
