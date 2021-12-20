// Frits Duindam

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> //LCD i2c lib


#ifndef STASSID
#define STASSID "Network"
#define STAPSK  "&v5BCx70R"

#endif

void wifiSetup();
char ReageerOpBericht();
void controllLCD(float t, int h);

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
int loper = 0;
char buffer[100];

char ch;

const String IDname = "LCDWemos";

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.5.1";
const uint16_t port = 3000;

// geeft geen temperatuur mee dus 0
float t = 0;
int h = 0;

ESP8266WiFiMulti WiFiMulti;
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  
  Serial.begin(9600);
  wifiSetup(); 
  Serial.print("connecting to ");
  Serial.print(host);         
  Serial.print(':');
  Serial.println(port);       // de port van de pi
  //Serial.print(unique_id); // is het wemos device in gebruik
  // Use WiFiClient class to create TCP connections

  //start up LCD
  lcd.begin();  
  lcd.backlight();
  lcd.print("INFO");
  lcd.setCursor(0,1);
  lcd.print("Screen");
  
}


void loop() {

  WiFiClient client;

  Serial.print("sending data... ");

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
    delay(5000);
    return;
  }
  
  client.print(json_data(0 ,0 ,IDname));
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
      ch = static_cast<char>(client.read());
      Serial.print(ch); // om te zien wat de waarde is
        if(loper < 42)
        {
          buffer[loper] = ch;
          loper++;
          buffer[loper] = '\0';
        }
      delay(10);    
      
  }
  loper=0;
      //Serial.print(buffer); // voor debugging
      DynamicJsonDocument doc(1000);
      DeserializationError error = deserializeJson(doc, buffer);
        if(error)
        {
          Serial.println("  Invalid Json Object!  ");
       
        }
      controllLCD(doc["temp"],doc["humi"]);
  
  Serial.println("closing connection");
  client.stop();

  Serial.println("wait 5 sec...");
  
  delay(10000);
}



//* functions ---------------------------------------------------------------------------------------------------


void controllLCD(float t, int h)
{
  lcd.clear();// clear previous values from screen
  lcd.print("Temperature");
  lcd.setCursor(12,0);
  lcd.print("Humidity");
  lcd.setCursor(0,1);  
  lcd.print(t);
  lcd.setCursor(6,1);
  lcd.print("C");
  lcd.setCursor(12,1);
  lcd.print(h);
  lcd.setCursor(15,1);
  lcd.print("%");
  delay(200);
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
