#include <WebServer.h>

  #include <Adafruit_BusIO_Register.h>
  #include <Adafruit_GenericDevice.h>
  #include <Adafruit_I2CDevice.h>
  #include <Adafruit_I2CRegister.h>
  #include <Adafruit_SPIDevice.h>
  
  #include <Wire.h>
  #include <WiFi.h>
  
  #include <PubSubClient.h>
  #include <Adafruit_VL53L0X.h>
  //WiFi credentials
  const char* ssid = "Samanvai";
  const char* pass = "12345678";
  
  //ThingSpeak Channnel
  unsigned long myChannelNumber = 3435480;
  
  //ThingSpeak MQTT Broker
  const char* mqttServer = "mqtt3.thingspeak.com";
  const int mqttPort = 1883;
  
  //ThingSpeak MQTT Device Credentials
  const char* mqttClientID = "LDwBJh83LAkdDwQlPSEBKQI";
  const char* mqttUsername = "LDwBJh83LAkdDwQlPSEBKQI";
  const char* mqttPassword = "K6k104+0Ks0Rz7yBzM96F6pU";
  
  #define RED_PIN 18
  #define GREEN_PIN 19
  #define BLUE_PIN 23
  
  #define VACANT 0
  #define APPROACHING 1
  #define OCCUPIED 2
  
  WiFiClient espClient;
  WebServer server(80);
  PubSubClient mqttClient(espClient);
  Adafruit_VL53L0X lox =
  Adafruit_VL53L0X();
  
  //ThingSpeak MQTT Publish Topic
  String publishTopic = "channels/" + String(myChannelNumber) + "/publish";
  
  int distance = 0;
  int status = 0;
  bool slot[4] =
{
    false,   // Slot 1 Empty
    true,    // Slot 2 Occupied
    false,   // Slot 3 Empty
    true     // Slot 4 Occupied
};
  VL53L0X_RangingMeasurementData_t measure;
  
  void connectWiFi()
  {
      if(WiFi.status()==WL_CONNECTED)
          return;
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid,pass);
      while(WiFi.status()!=WL_CONNECTED)
      {
          delay(1000);
          Serial.print(".");
      }
      Serial.println("WiFi Connected");
  }
  
  void connectMQTT()
  {
      while(!mqttClient.connected())
      {
          if(mqttClient.connect(
              mqttClientID,
              mqttUsername,
              mqttPassword))
          {
              Serial.println("MQTT Connected");
          }
          else
          {
              Serial.print("MQTT Failed. State = ");
              Serial.println(mqttClient.state());
              Serial.println("Retrying...");
              delay(5000);
          }
      }
  }
  
int getNextSlot();
//HTML web server 
String getHTML()
{
    String parkingStatus;

    if(status == VACANT)
        parkingStatus = "VACANT";
    else if(status == APPROACHING)
        parkingStatus = "APPROACHING";
    else
        parkingStatus = "OCCUPIED";

    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>

<meta http-equiv="refresh" content="2">

<title>Smart Parking System</title>

<style>

body{
    font-family: Arial;
    background:#f2f2f2;
    text-align:center;
}

.card{
    width:450px;
    margin:auto;
    margin-top:20px;
    background:white;
    padding:20px;
    border-radius:15px;
    box-shadow:0px 0px 10px gray;
}

h1{
    color:#0b5394;
}

.slot{
    padding:12px;
    margin:10px;
    border-radius:10px;
    font-size:20px;
    font-weight:bold;
}

.empty{
    background:#7CFC00;
}

.occupied{
    background:#ff4d4d;
    color:white;
}

</style>

</head>

<body>

<div class='card'>

<h1>SMART PARKING SYSTEM</h1>

)rawliteral";

    html += "<h2>Distance : " + String(distance) + " cm</h2>";
    html += "<h2>Status : " + parkingStatus + "</h2>";

    html += "<hr>";
    html += "<h2>Parking Slots</h2>";

    for(int i=0;i<4;i++)
    {
        html += "<div class='slot ";

        if(slot[i])
            html += "occupied'>";
        else
            html += "empty'>";

        html += "Slot ";
        html += String(i+1);

        if(slot[i])
            html += " : OCCUPIED";
        else
            html += " : EMPTY";

        html += "</div>";
    }

    int nextSlot = getNextSlot();

    html += "<hr>";

    if(nextSlot == -1)
    {
        html += "<h2 style='color:red;'>No Parking Available</h2>";
    }
    else
    {
        html += "<h2 style='color:green;'>Recommended Slot : ";
        html += String(nextSlot);
        html += "</h2>";
    }

    html += R"rawliteral(

</div>

</body>
</html>

)rawliteral";

    return html;
}

int getNextSlot()
{
    for(int i=0;i<4;i++)
    {
        if(slot[i]==false)
        {
            return i+1;
        }
    }

    return -1;
}

  void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    delay(1000);
    Wire.begin(21,22);
    pinMode(RED_PIN,OUTPUT);
    pinMode(GREEN_PIN,OUTPUT);
    pinMode(BLUE_PIN,OUTPUT);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    if(!lox.begin())
    {
      Serial.println("Sensor Failed");
      while(1);
    }
    //CONNECT TO WIFI
    connectWiFi();
    //PRINTING IP ADDRESS
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());
    //MQTT SETUP
    mqttClient.setServer(mqttServer,mqttPort);
    mqttClient.setBufferSize(256);
    mqttClient.setKeepAlive(60);
    //SERVER 
      server.on("/", []()
  {
      server.send(200, "text/html", getHTML());
  });

  server.begin();
  Serial.println("Web Server Started");
  }
  
  void loop() {
    // put your main code here, to run repeatedly:
    connectWiFi();
    if(!mqttClient.connected())
    {
      connectMQTT();
    }
    mqttClient.loop();
    server.handleClient();
    lox.rangingTest(&measure,false);
    if(measure.RangeStatus !=4)
    {
      Serial.print("Raw Distance (mm): ");
      Serial.println(measure.RangeMilliMeter);
      distance = measure.RangeMilliMeter/10;
      if(distance>12)
      {
        status=VACANT;
        slot[0]=false;
        digitalWrite(GREEN_PIN,HIGH);
        digitalWrite(RED_PIN,LOW);
        digitalWrite(BLUE_PIN,LOW);
      }
      else if(distance>6 && distance<=12)
      {
        status=APPROACHING;
        slot[0]=false;
        digitalWrite(GREEN_PIN,HIGH);
        digitalWrite(RED_PIN,HIGH);
        digitalWrite(BLUE_PIN,LOW);
      }
      else 
      {
        status=OCCUPIED;
        slot[0]=true;
        digitalWrite(GREEN_PIN,LOW);
        digitalWrite(RED_PIN,HIGH);
        digitalWrite(BLUE_PIN,LOW);
      }
// DISPLAY DISTANCE
Serial.print("Distance : ");
Serial.print(distance);
Serial.println(" cm");

// DISPLAY STATUS (Number)
Serial.print("Status Code : ");
Serial.println(status);

// DISPLAY STATUS (Text)
Serial.print("Parking Status : ");

if(status == VACANT)
{
    Serial.println("VACANT");
}
else if(status == APPROACHING)
{
    Serial.println("APPROACHING");
}
else
{
    Serial.println("OCCUPIED");
}

// PUBLISHING
String payload = "field1=" + String(distance) + "&field2=" + String(status);
      Serial.println(payload);
      if (mqttClient.publish(publishTopic.c_str(), payload.c_str()))
      {
         Serial.println("Published Successfully");
      }
      else
      {
         Serial.println("Publish Failed");
      }
      delay(2000);
     }
     else
     {
       Serial.println("Out of Range");
     }
  }
