#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);

#define Button D0
#define Lamp D2

const char *ssid = "vuiDoan0504";
const char *pass = "12121212";

// config địa chỉ ip esp cố định
IPAddress local_IP(192, 168, 43, 120);

IPAddress gateway(192, 168, 43, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

// biến lưu trữ
String ChuoiSendWebJson = "";
float nhietdo;
float doam;
int TTTB1, TTTB2;
long last = 0;

void ConnectWiFi();
void SendWebPage();
void Send_Update();
void dataJson(String sensor1, String sensor2, String sensor3, String sensor4);
void chuongtrinhcambien();
void NUT();
void duytri();
void button_onClick();
void xuLyOnOff(String button);

#include "web.h"

void setup() 
{
  Serial.begin(9600);
  while(!Serial);
  
  //Khai báo chân esp
  pinMode(Button, INPUT);
  pinMode(Lamp, OUTPUT);

  Serial.println();
  delay(3000);

  ConnectWiFi();

  server.on("/", [] {
    SendWebPage();  
  });

  server.on("/Update", [] {
    Send_Update();  
  });

  server.on("/Button", [] {
    button_onClick(); 
  });

  // Turn on server
  server.begin();
  Serial.println("Server Start");

  last = millis();
  
}

void loop() 
{
  
  server.handleClient(); // duy tri server
  NUT();
  if(millis() - last >= 1000)
  {
    chuongtrinhcambien();
    last = millis(); 
  }
  
}

void SendWebPage()
{
  server.send(200, "text/html", webpage);
}

void Send_Update()
{
  //send du lieu JSON
  dataJson(String(nhietdo), String(doam), String(TTTB1), String(TTTB2));
  server.send(200, "text/html", String(ChuoiSendWebJson));
}

void dataJson(String sensor1, String sensor2, String sensor3, String sensor4)
{
  ChuoiSendWebJson = "{\"ND\":\"" + String(sensor1) + "\"," +
                     "\"DA\":\"" + String(sensor2) + "\"," +
                     "\"TB1\":\"" + String(sensor3) + "\"," +
                     "\"TB2\":\"" + String(sensor4) + "\"}";
}

void chuongtrinhcambien()
{
  nhietdo = nhietdo + 1.2;
  doam = doam + 0.6;
  if(nhietdo >= 151)
  {
    nhietdo = 0;
  }

  if(doam >= 101)
  {
    doam = 0;
  }
}

void NUT()
{
  if(digitalRead(Button) == 0)
  {
    Serial.println("Button da duoc nhan!!!");
    delay(300);
    while(1)
    {
      //Kiem tra tha tay chua
      duytri(); 
      if(digitalRead(Button) == 1)
      {
        //thuc hien lenh
        if(TTTB1 == 1)
        {
          //OFF
          digitalWrite(Lamp, LOW);
          Serial.println("Lamp OFF");
          TTTB1 = 0;
        }
        else if (TTTB1 == 0)
        {
          //ON
          digitalWrite(Lamp, HIGH);
          Serial.println("Lamp ON");
          TTTB1 = 1;
        }
        //Thoat vong lap while
        last = millis();
        break;
      }
    }
  }
}

void duytri()
{
  server.handleClient();
  if(millis() - last >= 1000)
  {
    chuongtrinhcambien();
    last = millis(); 
  }
}

void button_onClick() 
{
  String button = "";
  button = server.arg("Button");
  xuLyOnOff(String(button));
  server.send(200, "text/html", webpage);
}

void xuLyOnOff(String button) 
{
  if(button.indexOf("A0B") >= 0)
  {
    digitalWrite(Lamp, LOW);
    TTTB1 = 0;
  } 
  else if (button.indexOf("A1B") >= 0)
  {
    digitalWrite(Lamp, HIGH);
    TTTB1 = 1;
  }
}

void ConnectWiFi()
{
  WiFi.disconnect();
  delay(1000);
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
  }
  delay(1000);
  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected, IP: address: ");
  Serial.println(WiFi.localIP());
}