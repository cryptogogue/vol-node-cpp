// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#define JSON_STR(...) #__VA_ARGS__

static const char* schema_json = JSON_STR (
    {
        "decks": {},
        "fonts": {},
        "icons": {},
        "lua": "",
        "definitions": {
            "asset-type-0": {
                "fields": {
                    "name": {
                        "type": "STRING",
                        "value": "Asset Type 0",
                        "mutable": false,
                        "scriptable": true,
                        "alternates": {}
                    },
                    "type": {
                        "type": "STRING",
                        "value": "common",
                        "mutable": false,
                        "scriptable": true,
                        "alternates": {}
                    }
                }
            },
            "asset-type-1": {
                "fields": {
                    "name": {
                        "type": "STRING",
                        "value": "Asset Type 1",
                        "mutable": false,
                        "scriptable": true,
                        "alternates": {}
                    },
                    "type": {
                        "type": "STRING",
                        "value": "rare",
                        "mutable": false,
                        "scriptable": true,
                        "alternates": {}
                    }
                }
            },
            "asset-type-2": {
                "fields": {
                    "name": {
                        "type": "STRING",
                        "value": "Asset Type 2",
                        "mutable": false,
                        "scriptable": true,
                        "alternates": {}
                    },
                    "type": {
                        "type": "STRING",
                        "value": "ultra-rare",
                        "mutable": false,
                        "scriptable": true,
                        "alternates": {}
                    }
                }
            },
            "pack": {
                "fields": {
                    "name": {
                        "type": "STRING",
                        "value": "Pack",
                        "mutable": false,
                        "scriptable": true,
                        "alternates": {}
                    },
                    "type": {
                        "type": "STRING",
                        "value": "pack",
                        "mutable": false,
                        "scriptable": true,
                        "alternates": {}
                    }
                }
            }
        },
        "layouts": {},
        "meta": "",
        "methods": {
            "makeRare": {
                "weight": 1,
                "maturity": 0,
                "description": "",
                "assetArgs": {
                    "asset0": {
                        "op": "EQUAL",
                        "left": {
                            "op": "INDEX",
                            "value": "type"
                        },
                        "right": {
                            "op": "CONST",
                            "const": {
                                "type": "STRING",
                                "value": "common"
                            }
                        }
                    },
                    "asset1": {
                        "op": "EQUAL",
                        "left": {
                            "op": "INDEX",
                            "value": "type"
                        },
                        "right": {
                            "op": "CONST",
                            "const": {
                                "type": "STRING",
                                "value": "common"
                            }
                        }
                    }
                },
                "constArgs": {},
                "constraints": [],
                "lua": "function main ( caller, assetArgs, constArgs )\r\n    print ( 'makeRare', caller )\r\nend"
            },
            "makeUltraRare": {
                "weight": 1,
                "maturity": 0,
                "description": "",
                "assetArgs": {
                    "asset0": {
                        "op": "EQUAL",
                        "left": {
                            "op": "INDEX",
                            "value": "type"
                        },
                        "right": {
                            "op": "CONST",
                            "const": {
                                "type": "STRING",
                                "value": "rare"
                            }
                        }
                    },
                    "asset1": {
                        "op": "EQUAL",
                        "left": {
                            "op": "INDEX",
                            "value": "type"
                        },
                        "right": {
                            "op": "CONST",
                            "const": {
                                "type": "STRING",
                                "value": "rare"
                            }
                        }
                    }
                },
                "constArgs": {},
                "constraints": [],
                "lua": "function main ( caller, assetArgs, constArgs )\r\n    print ( 'makeUltraRare', caller )\r\nend"
            },
            "openPack": {
                "weight": 1,
                "maturity": 0,
                "description": "",
                "assetArgs": {
                    "pack": {
                        "op": "EQUAL",
                        "left": {
                            "op": "INDEX",
                            "value": "type"
                        },
                        "right": {
                            "op": "CONST",
                            "const": {
                                "type": "STRING",
                                "value": "pack"
                            }
                        }
                    }
                },
                "constArgs": {},
                "constraints": [],
                "lua": "function main ( caller, assetArgs, constArgs )\r\n    print ( 'openPack:', caller, assetArgs.pack.assetID )\r\n    randomAward ( caller, 'commons', assetArgs.pack.assetID, 3 )\r\n    randomAward ( caller, 'rares', assetArgs.pack.assetID, 2 )\r\n    randomAward ( caller, 'ultraRares', assetArgs.pack.assetID, 1 )\r\n    revokeAsset ( caller, assetArgs.pack.assetID )\r\nend"
            }
        },
        "sets": {
            "commons": {
                "asset-type-0": 1
            },
            "rares": {
                "asset-type-1": 1
            },
            "ultraRares": {
                "asset-type-2": 1
            }
        },
        "upgrades": {
            "asset-type-0": "asset-type-1",
            "asset-type-1": "asset-type-2",
            "asset-type-2": "asset-type-3"
        }
    }
);
