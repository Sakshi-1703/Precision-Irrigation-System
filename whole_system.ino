#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"

// Replace with your network credentials
const char* ssid = "vivo V27";
const char* password = "11111111";

Adafruit_BME280 bme;
int dry = 2940; // Define the value representing dry conditions (maximum analog reading)
int wet = 1100; // Define the value representing wet conditions (minimum analog reading)
const int sensorPin = 34; // Analog pin for the soil moisture sensor, adjust as needed

float temperature;
float humidity;

AsyncWebServer server(80);
AsyncEventSource events("/events");

unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;  // send readings timer

float tempThreshold = 24.0; // Temperature threshold in degrees Celsius
float humidityThreshold = 70.0; // Humidity threshold in percentage
int moistureThreshold = 85; // Moisture threshold in percentage

const int solenoidPin = 12; // Pin connected to the solenoid water valve

#define SENSOR  27//water flow swnsor

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

void getBME280Readings(){
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
}

void controlWaterValve(int moisturePercentage) {
  if (moisturePercentage < moistureThreshold) { // If moisture is below 80%
    digitalWrite(solenoidPin, HIGH); // Open the solenoid water valve
  } else {
    digitalWrite(solenoidPin, LOW); // Keep the solenoid water valve closed
  }
}

String processor(const String& var){
  getBME280Readings();
  String result = "";
  
  if(var == "TEMPERATURE"){
    result = String(temperature);
    // Check if temperature exceeds or decreases below the threshold
    if (temperature > tempThreshold) {
      result += " <span style='color: red;'>(Exceeds threshold)</span>";
    } else if (temperature < tempThreshold) {
      result += " <span style='color: blue;'>(Below threshold)</span>";
    }
  }
  else if(var == "HUMIDITY"){
    result = String(humidity);
    // Check if humidity exceeds or decreases below the threshold
    if (humidity > humidityThreshold) {
      result += " <span style='color: red;'>(Exceeds threshold)</span>";
    } else if (humidity < humidityThreshold) {
      result += " <span style='color: blue;'>(Below threshold)</span>";
    }
  } else if (var == "MOISTURE") {
    int sensorValue = analogRead(sensorPin); // Read analog value from the sensor
    int moisturePercentage = map(sensorValue, dry, wet, 0, 100); // Map to moisture percentage
    controlWaterValve(moisturePercentage); // Control the water valve based on moisture level
    result = String(moisturePercentage) + "%"; // Prepare response
  } //else if (var == "FLOWRATE") {
    //result = String(flowRate);
  //} else if (var == "TOTALVOLUME") {
    //result = String(totalMilliLitres);
  //}
  
  return result;
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>BME280 Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p {  font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #4B1D3F; color: white; font-size: 1.7rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,140,.5); }
    .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); }
    .reading { font-size: 2.8rem; }
    .card.temperature { color: #0e7c7b; }
    .card.humidity { color: #17bebb; }
    .card.moisture { color: #e17d68; }
    .card.total_volume { color: #e17e68; }
  </style>
</head>
<body>
  <div class="topnav">
    <h3>BME280 WEB SERVER</h3>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card temperature">
        <h4><i class="fas fa-thermometer-half"></i> TEMPERATURE</h4><p><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
      </div>
      <div class="card humidity">
        <h4><i class="fas fa-tint"></i> HUMIDITY</h4><p><span class="reading"><span id="hum">%HUMIDITY%</span> &percnt;</span></p>
      </div>
      <div class="card moisture">
        <h4><i class="fas fa-tint"></i> MOISTURE</h4><p><span class="reading"><span id="moist">%MOISTURE%</span>&percnt;</span></p>
      </div>
      <div class="card flowrate">
        <h4><i class="fas fa-tint"></i> FLOW RATE</h4><p><span class="reading"><span id="flow">%FLOWRATE%</span> L/min</span></p>
      </div>
      <div class="card totalvolume">
        <h4><i class="fas fa-tint"></i> TOTAL VOLUME</h4><p><span class="reading"><span id="volume">%TOTALVOLUME%</span> mL</span></p>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('temperature', function(e) {
  console.log("temperature", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);
 
 source.addEventListener('humidity', function(e) {
  console.log("humidity", e.data);
  document.getElementById("hum").innerHTML = e.data;
 }, false);
 
 source.addEventListener('moisture', function(e) {
  console.log("moisture", e.data);
  document.getElementById("moist").innerHTML = e.data;
 }, false);
 source.addEventListener('flowrate', function(e) {
  console.log("flowrate", e.data);
  document.getElementById("flow").innerHTML = e.data;
 }, false);

 source.addEventListener('totalvolume', function(e) {
  console.log("totalvolume", e.data);
  document.getElementById("volume").innerHTML = e.data;
 }, false);
}
</script>
</body>
</html>)rawliteral";

void setup() {
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  bool status;

  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not detect a BME280 sensor, Fix wiring Connections!");
    while (1);
  }
  
  // Setup solenoid water valve pin
  pinMode(solenoidPin, OUTPUT);
  
  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
}

void loop() {
  // Read BME280 sensor data
  getBME280Readings();
  
  // Read soil moisture sensor data
  int sensorValue = analogRead(sensorPin); // Read analog value from the sensor
  int moisturePercentage = map(sensorValue, dry, wet, 0, 100); // Map to moisture percentage
  
  // Read flow sensor data
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    pulse1Sec = pulseCount;
    pulseCount = 0;

    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
  }

  // Print all sensor readings to serial monitor
  Serial.println("BME280 Sensor Readings:");
  Serial.printf("Temperature = %.2f ºC \n", temperature);
  Serial.printf("Humidity = %.2f %% \n", humidity);
  Serial.println();
  
  Serial.println("Soil Moisture Sensor Reading:");
  Serial.printf("Moisture = %d%%\n", moisturePercentage);
  Serial.println();
  
  Serial.println("Flow Sensor Readings:");
  Serial.printf("Flow Rate = %.2f L/min \n", flowRate);
  Serial.printf("Total Volume = %lu mL \n", totalMilliLitres);
  Serial.println();

  // Send sensor readings to the Web Server
  events.send(String(temperature).c_str(), "temperature", millis());
  events.send(String(humidity).c_str(), "humidity", millis());
  events.send(String(moisturePercentage).c_str(), "moisture", millis());
  events.send(String(flowRate).c_str(), "flowrate", millis());
  events.send(String(totalMilliLitres).c_str(), "totalvolume", millis());

  // Control water valve based on moisture level
  controlWaterValve(moisturePercentage);

  delay(1000); // Delay for stability
}