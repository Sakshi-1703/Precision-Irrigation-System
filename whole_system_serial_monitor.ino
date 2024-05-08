#include <iostream>
#include <cstdlib>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
int dry = 1100; // Define the value representing dry conditions (maximum analog reading)
int wet = 2940; // Define the value representing wet conditions (minimum analog reading)
const int sensorPin = 34; // Analog pin for the soil moisture sensor, adjust as needed
int moistureThreshold = 85; // Moisture threshold in percentage
int solenoidPin = 25; // This is the output pin on the Arduino we are using
int moisture; // The variable to control


Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

// Function to predict irrigation toggling based on given inputs
int predict_irrigation(float soil_moisture, float volume, float temperature, float threshold_values[]) {
    if (soil_moisture < threshold_values[0] && volume < threshold_values[1]) {
        return 1;  // Toggle irrigation system
    } else if (soil_moisture < threshold_values[0] && volume >= threshold_values[1] && temperature < threshold_values[2]) {
        return 1;  // Toggle irrigation system
    } else {
        return 0;  // Do not toggle irrigation system
    }
}

// Function to generate random data for demonstration
void generate_random_data(float data[], int num_samples, float lower_bound, float upper_bound) {
    for (int i = 0; i < num_samples; ++i) {
        data[i] = (float)rand() / RAND_MAX * (upper_bound - lower_bound) + lower_bound;
    }
}

void controlWaterValve(int moisturePercentage) {
  if (moisturePercentage < moistureThreshold) { // If moisture is below threshold
    digitalWrite(solenoidPin, HIGH); // Open the solenoid water valve
  } else {
    digitalWrite(solenoidPin, LOW); // Keep the solenoid water valve closed
  }
}

// Setup function (runs once)
void setup() {
    // Initialize serial communication
    Serial.begin(9600);
    Serial.println("Initializing...");
    pinMode(sensorPin, INPUT); // Setting sensor pin to be input
    pinMode(solenoidPin, OUTPUT); // Sets the pin as an output
    if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
}

// Loop function (runs repeatedly)
void loop() {
    // Example threshold values (you can adjust these according to your requirements)
    float threshold_values[] = {60.0, 12.0, 28.0};  // Example: Soil moisture threshold, volume threshold, temperature threshold
    
    // Example usage
    float soil_moisture = 55.0;
    float volume = 10.0;
    float temperature = 25.0;
    
    int prediction = predict_irrigation(soil_moisture, volume, temperature, threshold_values);
    
    // Handle alerts
    if (prediction == 1) {
        Serial.println("Toggle irrigation system");
    } else {
        Serial.println("Do not toggle irrigation system");
    }

    delay(1000); // Delay for 1 second before repeating

  //BME280 code
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.println();
    delay(2000);

    //soil moisture
    int sensorValue = analogRead(sensorPin); // Read analog value from the sensor
    int moisturePercentage = map(sensorValue, dry, wet, 0, 100); // Map to moisture percentage
    moisture = moisturePercentage; // Update global variable

    controlWaterValve(moisturePercentage); // Control the water valve based on moisture level
  }

}
