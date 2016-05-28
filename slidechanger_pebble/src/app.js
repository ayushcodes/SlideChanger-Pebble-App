var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
xhr.setRequestHeader("Content-type", "application/json; charset=utf-8");
xhr.timeout = 2000; // Set timeout to 2 seconds
  xhr.send();
};


// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
  var dict = e.payload;

    /*
    if (dict.AppKeyRequestData == 1)
    var url = "http://192.168.3.155:8080/next";
  else
    var url = "http://192.168.3.155:8080/prev";
    
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
    });
    */
    var websocket = new WebSocket("ws://192.168.3.155:8080", "echo-protocol");
websocket.onopen = function (event) {
websocket.send(""+dict.AppKeyRequestData);
};
// Tell the server this is client 1 (swap for client 2 of course)

  }                     
);
