
/* 1.Connectez le fil de connexion du pin SDA de l'écran OLED 
à la broche SDA de l'ESP8266 AzDelivery (GPIO4/D2).

2.Connectez le fil de connexion du pin SCL de l'écran OLED 
à la broche SCL de l'ESP8266 AzDelivery (GPIO5/D1).

3.Si des résistances de pull-up sont nécessaires, connectez-les entre
 les broches SDA et VCC (3.3V) et entre les broches SCL et VCC sur l'écran OLED. 
 La valeur de résistance recommandée est généralement de 4.7kΩ.

4.Connectez le fil de connexion GND (Ground) de l'écran OLED à la broche GND de l'ESP8266 AzDelivery.

5. Connectez le fil de connexion VCC (alimentation) de l'écran OLED à 
la broche 3.3V de l'ESP8266 AzDelivery.*/

#include <PubSubClient.h> /* libarie à choisir pour Message MQTT*/
#include <ESP8266WiFi.h>/* libairie pour wifi ESP8266*/


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///s'assurer d'avoir 0x3C pour l'adresse de l'Oled
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Update these with values suitable for your network.
// parametre wifi 

const char* ssid = "PlanetCampus - Prive 000359"; 
const char* password = "2D9S3B9g2h32Y";
const char* mqtt_server = "test.mosquitto.org"; /* broker MQTT*/

char tab[200];

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("statut :");
    Serial.print(WiFi.status());
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    tab[i] = (char)payload[i];
  }
  Serial.println(tab);
  display.clearDisplay();
  display.display();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 20);
  display.print("Message: ");
   display.setCursor(0, 40);
  display.write(tab);
  display.display();
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
   // tab[i] = ' ';
   memset(tab, ' ',sizeof(tab));
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Binome_4")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("binome4");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
