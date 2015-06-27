var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude;

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Conditions
      var weatherId = json.weather[0].id;      
      console.log('Weather ID is ' + weatherId);
      
      // Assemble dictionary using our keys
      var dictionary = {
        'KEY_WEATHER_ID': weatherId
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log('Weather info sent to Pebble successfully!');
        },
        function(e) {
          console.log('Error sending weather info to Pebble!');
        }
      );
    }      
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');

    // Get the initial weather
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    getWeather();
  }                     
);

var html = (function() {/*
<html>
	<head>
		<style type="text/css">
html {
	font-family: 'Trebuchet MS', sans-serif;
	background-color: #222222;
	color: #AAAAAA;
	text-transform: uppercase;
	-webkit-user-select: none;
}
h1, h2, h3, h4, h5, h6 {
	color: #FF4A00;
	font-size: 1em;
}
.button {
	background-color: transparent;
	border: 2px solid #FF4A00;
	border-radius: 100px;
	color: white;
	display: inline-block;
	font-size: 0.8em;
	margin: 1em 0;
	padding: 0.5em 1em;
	text-decoration: none;
	text-transform: uppercase;
}
.button:active {
	border-color: #7F2500;
}
.save-box {
	float: right;
	margin: 1em 0 0 0;
}
table {
	border: 2px solid #FF4A00;
	border-radius: 3px;
}
		</style>
		<script type="text/javascript">
function save() {
	var secHand = document.getElementById('showsSecondHand').checked;
  var config = {
    "showsSecondHand": secHand
  };
  location.href = "pebblejs://close#" + encodeURIComponent(JSON.stringify(config));
}
		</script>
	</head>
	<body>
		<div class="save-box">
			<a href="#" class="button" onclick="save()">Save</a>
		</div>
		<table>
			<h2>Display Settings</h2>
			<tr>
				<td><input type="checkbox" id="showsSecondHand" checked></td>
				<td><label for="showsSecondHand">Show Second Hand</label></td>
			</tr>
		</table>
	</body>
</html>
*/}).toString().match(/\/\*([^]*)\*\//)[1];

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('data:text/html;charset=UTF-8,' + encodeURIComponent(html));
});

Pebble.addEventListener('webviewclosed', function(e) {
  var config = JSON.parse(decodeURIComponent(e.response));
  Pebble.sendAppMessage(config,
    function(e) {
      console.log('Successfully delivered message with transactionId='
        + e.data.transactionId);
    },
    function(e) {
      console.log('Unable to deliver message with transactionId='
        + e.data.transactionId
        + ' Error is: ' + e.error.message);
    });
  console.log('Configuration window returned: ', JSON.stringify(config));
});
