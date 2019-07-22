const meta = JSON.parse ( `{
    "templates": {
    },
    "definitions": {
        "pack": {
            "template": "pack",
            "displayName": {
                "EN": "Booster Pack",
                "ES": "El Booster Pack",
                "FR": "Le Booster Pack"
            }
        },
        "common": {
            "template": "card",
            "displayName": {
                "EN": "Common",
                "ES": "El Common",
                "FR": "Le Common"
            },
            "image": {
                "RGB": "https://i.imgur.com/VMPKVAN.jpg"
            }
        },
        "rare": {
            "template": "card",
            "displayName": {
                "EN": "Rare",
                "ES": "El Rare",
                "FR": "Le Rare"
            },
            "image": {
                "RGB": "https://i.imgur.com/BtKggd4.jpg"
            }
        },
        "ultraRare": {
            "template": "card",
            "displayName": {
                "EN": "Ultra-Rare",
                "ES": "El Ultra-Rare",
                "FR": "Le Ultra-Rare"
            },
            "image": {
                "RGB": "https://i.imgur.com/2aiJ3cq.jpg"
            }
        }
    }
}` );

meta.templates [ 'card' ] = '<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" baseProfile="basic" width="750" height="1050" viewBox="0 0 750 1050" preserveAspectRatio="none"><rect x="0" y="0" width="750" height="1050" fill="gray" stroke="blue" stroke-width="37.5" /><image x="25" y="100" height="700" width="700" xlink:href="{{ image }}" /><text x="375" y="870" font-size="75" text-anchor="middle" fill="white">{{ displayName }}</text><svg x="75" y="900" width="600" height="125" viewBox="0 0 107 52" preserveAspectRatio="none"><rect x="0" y="0" width="107" height="52" fill="white" />{{{ barcode }}}</svg></svg>';
meta.templates [ 'pack' ] = '<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" baseProfile="basic" width="750" height="1050" viewBox="0 0 750 1050" preserveAspectRatio="none"><rect x="0" y="0" width="750" height="1050" fill="gray" stroke="blue" stroke-width="37.5" /><text x="375" y="560" font-size="150" text-anchor="middle" fill="white">PACK</text><text x="375" y="870" font-size="75" text-anchor="middle" fill="white">{{ displayName }}</text><svg x="75" y="900" width="600" height="125" viewBox="0 0 107 52" preserveAspectRatio="none"><rect x="0" y="0" width="107" height="52" fill="white" />{{{ barcode }}}</svg></svg>';

export { meta };