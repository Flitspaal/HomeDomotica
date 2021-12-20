#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <DHT.h>
#include <ArduinoJson.h>


#ifndef STASSID
#define STASSID "Network"
#define STAPSK  "&v5BCx70R"

#define DHTPIN 5 // pin D1
#define DHTTYPE DHT22

#endif

void wifiSetup();
char ReageerOpBericht();
int readHumi();
float readTemp();

String json_data(float temp, int humi, String ID) {
  const char* data = "{\"temp\":\"furniture\",\"humi\":status_code}"; // Create JSON skeleton   
  StaticJsonDocument<96> json_object;     // Create JSON object
  json_object["ID"] = ID;        // Modify value in JSON object based on key name
  json_object["temp"] = temp;        // Modify value in JSON object based on key name
  json_object["humi"] = humi;      // Modify value in JSOB object based on key name
  char send_data[100];              
  serializeJson(json_object, send_data); // Convert JSON Object to a character string. 

  return send_data;
}

int i = 0;
const String IDname = "SensorWemos";

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.5.1";
const uint16_t port = 3000;

DHT dht(DHTPIN, DHTTYPE);

ESP8266WiFiMulti WiFiMulti;

void setup() {
  
  Serial.begin(9600);
  wifiSetup(); 
  dht.begin();
  Serial.print("connecting to ");
  Serial.print(host);         
  Serial.print(':');
  Serial.println(port);       // de port van de pi
  //Serial.print(unique_id); // is het wemos device in gebruik
  // Use WiFiClient class to create TCP connections

}
// testing git


void loop() {

  WiFiClient client;

  Serial.println("sending data... ");

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
    delay(5000);
    return;
  }
  
  float t = readTemp();
  int h = readHumi();
  client.print(json_data(t ,h ,IDname));
  delay(100);
  unsigned long timeout = millis();
  while (client.available() == 0) // contoleerd connectie
  {
      if (millis() - timeout > 20000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        delay(10000); // wacht 10 sec en probeer het overnieuw
        
        return;
      }
    }
  
  while (client.available()) // leest het ingekomen bericht
  {
      char ch = static_cast<char>(client.read());
      //Serial.print(ch); // om te zien wat de waarde is
      //delay(100);
      //ReageerOpBericht(ch); // doet een actie met ingelezen waarde
      
      
  }
  
  Serial.println("closing connection");
  client.stop();

  Serial.print("wait 5 sec...");
  delay(10000);
}



//* functions ---------------------------------------------------------------------------------------------------

float readTemp()
{
  float t = dht.readTemperature();
  if(isnan(t))
  {
    Serial.println("Failed to read from DHT temp sensor ");
    return 80;
  }
  return t;
}

int readHumi()
{
  int h = dht.readHumidity();
  if(isnan(h))
  {
    Serial.println("Failed to read from DHT humi sensor ");
    return 80;
  }
  return h;
}

void wifiSetup()
{


  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}
