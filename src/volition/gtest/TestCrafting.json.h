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
            "common": {
                "fields": {
                    "name": {
                        "type": "STRING",
                        "value": "Common",
                        "mutable": false
                    }
                }
            },
            "rare": {
                "fields": {
                    "name": {
                        "type": "STRING",
                        "value": "Rare",
                        "mutable": false
                    }
                }
            },
            "ultrarare": {
                "fields": {
                    "name": {
                        "type": "STRING",
                        "value": "UltraRare",
                        "mutable": false
                    }
                }
            },
            "pack": {
                "fields": {
                    "name": {
                        "type": "STRING",
                        "value": "Pack",
                        "mutable": false
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
                "friendlyName": "",
                "description": "",
                "assetArgs": {
                    "asset0": {
                        "qualifier": {
                            "op": "EQUAL",
                            "left": {
                                "op": "INDEX",
                                "paramID": "",
                                "value": "@"
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
                    "asset1": {
                        "qualifier": {
                            "op": "EQUAL",
                            "left": {
                                "op": "INDEX",
                                "paramID": "",
                                "value": "@"
                            },
                            "right": {
                                "op": "CONST",
                                "const": {
                                    "type": "STRING",
                                    "value": "common"
                                }
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
                "friendlyName": "",
                "description": "",
                "assetArgs": {
                    "asset0": {
                        "qualifier": {
                            "op": "EQUAL",
                            "left": {
                                "op": "INDEX",
                                "paramID": "",
                                "value": "@"
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
                    "asset1": {
                        "qualifier": {
                            "op": "EQUAL",
                            "left": {
                                "op": "INDEX",
                                "paramID": "",
                                "value": "@"
                            },
                            "right": {
                                "op": "CONST",
                                "const": {
                                    "type": "STRING",
                                    "value": "rare"
                                }
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
                "friendlyName": "",
                "description": "",
                "assetArgs": {
                    "pack": {
                        "qualifier": {
                            "op": "EQUAL",
                            "left": {
                                "op": "INDEX",
                                "paramID": "",
                                "value": "@"
                            },
                            "right": {
                                "op": "CONST",
                                "const": {
                                    "type": "STRING",
                                    "value": "pack"
                                }
                            }
                        }
                    }
                },
                "constArgs": {},
                "constraints": [],
                "lua": "function main ( caller, assetArgs, constArgs )\r\n    print ( 'openPack:', caller, assetArgs.pack.assetID )\r\n    randomAward ( caller, 'commons', assetArgs.pack.assetID, 3 )\r\n    randomAward ( caller, 'rares', assetArgs.pack.assetID, 2 )\r\n    randomAward ( caller, 'ultraRares', assetArgs.pack.assetID, 1 )\r\n    revokeAsset ( assetArgs.pack.assetID )\r\nend"
            }
        },
        "rewards": {
            "boosterBox": {
                "friendlyName": "",
                "description": "",
                "quantity" : 3,
                "lua": "function main ( caller, count, quantity )\r\n    print ( 'boosterBox', caller, count, quantity )\r\n    awardAsset ( caller, 'pack', 12 )\r\nend"
            }
        },
        "sets": {
            "commons": {
                "common": 1
            },
            "rares": {
                "rare": 1
            },
            "ultraRares": {
                "ultrarare": 1
            }
        },
        "upgrades": {
            "common": "rare",
            "rare": "ultraRare",
            "ultraRare": "pack"
        },
        "version": {
            "release": "test",
            "major": 0,
            "minor": 0,
            "revision": 0
        }
    }
);
