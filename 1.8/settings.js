function init() {
    if (isAndroid()) {
        document.getElementById('save-button').style.display = "none";
    } else {
        document.getElementById('toolbar').style.display = "none";
    }
    
    var zoomInTimeout = document.querySelector('#zoomInTimeout');
    zoomInTimeout.addEventListener('value-change', function() {
        document.querySelector('#zoomInTimeoutLabel').textContent = zoomInTimeout.value;
    });
    zoomInTimeout.addEventListener('immediate-value-change', function() {
        document.querySelector('#zoomInTimeoutLabel').textContent = zoomInTimeout.immediateValue;
    });
    
    load();
}
function isAndroid() {
    return navigator.userAgent.indexOf('Android') >= 0;
}
function load() {
    var hash = location.hash;
    if (hash) {
        var config = JSON.parse(decodeURIComponent(location.hash.substring(1)));
        if (!config['showSecondHand']) {
            document.getElementById('showSecondHand').checked = false;
        }
        document.querySelector('#zoomInTimeout').value = config['zoomInTimeout'] / 1000.0;
        document.querySelector('#temperatureScale').selected = config['temperatureScale'];
    }
}
function save() {
    var secHand = document.getElementById('showSecondHand').checked;
    var tempScale = document.getElementById('temperatureScale').selected;
    var zoomInTimeout = parseFloat(document.querySelector('#zoomInTimeout').value) * 1000;
    
    var config = {
        "showSecondHand":   secHand ? 1 : 0,
        "temperatureScale": tempScale,
        "zoomInTimeout":    zoomInTimeout
    };
    location.href = "pebblejs://close#" + encodeURIComponent(JSON.stringify(config));
}
function cancel() {
    location.href = "pebblejs://close";
}
document.addEventListener('WebComponentsReady', function () {
    init();
});