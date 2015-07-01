function init() {
    var hash = location.hash;
    if (hash) {
        var config = JSON.parse(decodeURIComponent(location.hash.substring(1)));
        if (!config['showSecondHand']) {
            document.getElementById('showSecondHand').checked = false;
        }
    }
}
function save() {
    var secHand = document.getElementById('showSecondHand').checked;
    var config = {
        "showSecondHand": secHand ? 1 : 0
    };
    location.href = "pebblejs://close#" + encodeURIComponent(JSON.stringify(config));
}
