// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "meteoStanica1";
const char* password = "semestralka2023";
const int redPin = 13;
 const int greenPin = 12; 
 const int bluePin= 14;
 const int freq = 5000;
 const int redChannel = 0;
 const int greenChannel= 1;
 const int blueChannel = 2;
 const int resolution = 8;
 const String nazovStanice = "MeteoStanica1";

#define DHTPIN 18     // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    Serial.println("no output");
    return "--";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("no output");
    return "--";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>MeteoInfo</title>

   <style> 
    #humidity,#temperature, .koliesko {
      display: inline-block;
      vertical-align: middle;
    }
   
    .lds-dual-ring {  
      width: auto;
      height: auto;
    }
    .lds-dual-ring:after {
      content: " ";
      display: block;
      width: 10px;
      height: 10px;
      margin: 0px;
      border-radius: 80%;
      border: 4px solid #ff0000;
      border-color: #ff0000 transparent #ff0000 transparent;
      animation: lds-dual-ring 1.2s linear infinite;
    }
    @keyframes lds-dual-ring {
      0% {
        transform: rotate(0deg);
      }
      100% {
        transform: rotate(360deg);
      }
    }
   </style>
)rawliteral";
   const char index_html2[] PROGMEM = R"rawliteral(
   <script>
    function updateValues() {
      fetch('/data.json')
        .then(response => response.json())
        .then(data => {
          document.getElementById('temperature').innerHTML = data.temperature +"°C";
          document.getElementById('humidity').innerHTML = data.humidity+"%";
        });
    }
    setInterval(updateValues, 5000);
  </script>
   <script>
    let seconds=0;
    function odpocitavac() {
    seconds = (seconds + 1) % 6;
    let kolieska = document.querySelectorAll('.koliesko');
    for(let koliesko of kolieska){
      if (seconds == 4) {
        koliesko.style.display = 'inline-block';
      } else if (seconds == 0) {
        koliesko.style.display = 'none';
      }
    }
    let dots = '.'.repeat(seconds);
    document.getElementById('update').innerHTML = dots;
   }
  setInterval(odpocitavac, 1000);      
    
    
  </script>
</head>
<body>
    <h2>Teplota</h2>
    <div id="temperature">Nacitavam...</div>
    <div class="koliesko lds-dual-ring"></div>
    <h2>Vlhkost</h2>
    <div id="humidity">Nacitavam...</div>
    <div class="koliesko lds-dual-ring"></div>
    <h1 id="update">.....</h1>    
</body>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
ledcSetup(redChannel, freq, resolution);
ledcSetup(greenChannel,freq, resolution);
ledcSetup(blueChannel, freq, resolution);

  dht.begin();
  
  // Connect to Wi-Fi
   WiFi.softAP(ssid, password);

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
 server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = String(index_html) + String(index_html2);
    request->send(200, "text/html", html);
});

 
  server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{\"name\":\"";
    json += nazovStanice;
    json += "\",\"temperature\":";
    json += readDHTTemperature();
    json += ",\"humidity\":";
    json += readDHTHumidity();
    json += "}";
    request->send(200, "application/json", json);
});

server.on("/ledZmena1", HTTP_GET, [](AsyncWebServerRequest *request){
  if(request->hasParam("r") && request->hasParam("g") && request->hasParam("b") && request->hasParam("a")) {
    int r = request->getParam("r")->value().toInt();
    int g = request->getParam("g")->value().toInt();
    int b = request->getParam("b")->value().toInt();
    float a = request->getParam("a")->value().toFloat();
    // Handle the input values here
    zmenLedku(r,g,b,a);
  }
  request->send(200);
});
  // Start server
  server.begin();
}
void zmenLedku(int r,int g, int b, float alfa) {
analogWrite(redPin, r*alfa);
analogWrite(greenPin, g*alfa);
analogWrite(bluePin, b*alfa);

}
void loop(){
  
}
