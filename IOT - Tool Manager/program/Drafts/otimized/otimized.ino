#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

#define WIFI_SSID "Vivo-Internet-BF17"
#define WIFI_PASSWORD "78814222"

#define DATABASE_URL "https://toolmanager-b1304-default-rtdb.firebaseio.com/"

WiFiClientSecure ssl;
DefaultNetwork network;
AsyncClientClass client(ssl, getNetwork(network));

FirebaseApp app;
RealtimeDatabase Database;
AsyncResult result;
NoAuth noAuth;

int builtinLed = 2;
const int portsLength = 15;
//                         A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
int pins[portsLength] = {4, 16, 17, 5, 18, 19, 21, 22, 23, 32, 33, 25, 26, 27, 14};
char cabinet[5] = "01";

void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}

void open(int index)
{
    digitalWrite(pins[index], LOW);
    delay(5000);
    int status = Database.set<int>(client,cabinet, 0);
    digitalWrite(pins[index], HIGH);
}

void getOpened(int* arr, int value) 
{
  for (int i = 0; i< 15; i++){
    int status = (value >> i) & (1);
    arr[i] = status;
  }
}

void setup()
{

    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    ssl.setInsecure();

    // Initialize the authentication handler.
    initializeApp(client, app, getAuth(noAuth));

    // Binding the authentication handler with your Database class object.
    app.getApp<RealtimeDatabase>(Database);

    // Set your database URL
    Database.url(DATABASE_URL);

    // In sync functions, we have to set the operating result for the client that works with the function.
    client.setAsyncResult(result);

    Serial.print("Testando portas (");
    Serial.print(portsLength);
    Serial.println(" conectadas)");

    for (int i = 0; i < portsLength; i++)
    {
        int crrPin = pins[i];
        Serial.print("    Testando porta: ");
        Serial.print(crrPin);
        pinMode(crrPin, OUTPUT);
        digitalWrite(crrPin, LOW);
        delay(500);
        digitalWrite(crrPin, HIGH);
        Serial.println(" - OK");
    }

    Serial.println("Trancando todos as gavetas");

    int status = Database.set<int>(client, cabinet, 0);

    if (status == 0)
        Serial.println(" - OK");
    else
        printError(client.lastError().code(), client.lastError().message());

    Serial.println("Configuração concluida");
    pinMode(builtinLed, OUTPUT);
    digitalWrite(builtinLed, HIGH);
}

void loop()
{

    Serial.println("");
    Serial.print("Status of ");
    Serial.print(cabinet);
    Serial.print(": ");
    int opened[portsLength];
    int status = Database.get<int>(client, cabinet);
    getOpened(opened, status);
    if (status){
      Serial.println(status);
    }
    for(int i = 0; i < portsLength; i++){
      int crr = opened[i];
      if(crr > 0){
        open(i);
      }
      Serial.print(" ");
      Serial.print(opened[i]);
    }
    if (client.lastError().code() != 0)
        printError(client.lastError().code(), client.lastError().message());

    // if (status)
    // {
    //     open(drawer, pins[port]);
    // }
}
