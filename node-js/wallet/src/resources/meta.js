const meta = {
    templates: {},
}

meta.templates [ 'card' ] = '<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" baseProfile="basic" width="750" height="1050" viewBox="0 0 750 1050" preserveAspectRatio="none"><rect x="0" y="0" width="750" height="1050" fill="gray" stroke="blue" stroke-width="37.5" /><image x="25" y="100" height="700" width="700" xlink:href="{{ image }}" /><text x="375" y="870" font-size="75" text-anchor="middle" fill="white">{{ displayName }}</text><svg x="75" y="900" width="600" height="125" viewBox="0 0 107 52" preserveAspectRatio="none"><rect x="0" y="0" width="107" height="52" fill="white" />{{{ barcode }}}</svg></svg>';
meta.templates [ 'pack' ] = '<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" baseProfile="basic" width="750" height="1050" viewBox="0 0 750 1050" preserveAspectRatio="none"><rect x="0" y="0" width="750" height="1050" fill="gray" stroke="blue" stroke-width="37.5" /><text x="375" y="560" font-size="150" text-anchor="middle" fill="white">PACK</text><text x="375" y="870" font-size="75" text-anchor="middle" fill="white">{{ displayName }}</text><svg x="75" y="900" width="600" height="125" viewBox="0 0 107 52" preserveAspectRatio="none"><rect x="0" y="0" width="107" height="52" fill="white" />{{{ barcode }}}</svg></svg>';

export { meta };