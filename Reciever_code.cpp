#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

//================ WIFI =================
const char* ssid = "Your_wifi_name";
const char* password = "password";

//================ AWS ==================
const char* aws_endpoint = "aws_endpoint";

//================ SERVO ================
#define SERVO_PIN 26

Servo myServo;

WiFiClientSecure net;
PubSubClient client(net);

// Track valve state
bool isOpen = false;

// Rotation timing (adjust if needed)
int moveTime = 800;   // 0.8 sec

//================ ROOT CA ==============
const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
PASTE YOUR RootCA CERTIFICATE HERE
-----END CERTIFICATE-----
)EOF";

//================ DEVICE CERT ==========
const char* device_cert = R"KEY(
PASTE YOUR DEVICE CERTIFICATE HERE
)KEY";

//================ PRIVATE KEY ==========
const char* private_key = R"KEY(
PASTE YOUR PRIVATE KEY HERE
)KEY";

//=======================================
// OPEN VALVE (corrected direction)
//=======================================
void openServo() {

  if (!isOpen) {

    Serial.println("Opening Valve...");

    myServo.write(0);        // Correct OPEN direction
    delay(moveTime);

    myServo.write(90);       // Stop servo

    Serial.println("OPEN complete");

    isOpen = true;
  }
}

//=======================================
// CLOSE VALVE (corrected direction)
//=======================================
void closeServo() {

  if (isOpen) {

    Serial.println("Closing Valve...");

    myServo.write(180);      // Correct CLOSE direction
    delay(moveTime);

    myServo.write(90);       // Stop servo

    Serial.println("CLOSE complete");

    isOpen = false;
  }
}

//=======================================
// MQTT CALLBACK
//=======================================
void callback(char* topic, byte* payload, unsigned int length) {

  String msg = "";

  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Received: ");
  Serial.println(msg);

  if (msg.indexOf("OPEN") >= 0) {
    openServo();
  }

  else if (msg.indexOf("CLOSE") >= 0) {
    closeServo();
  }
}

//=======================================
// WIFI CONNECT
//=======================================
void connectWiFi() {

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
}

//=======================================
// AWS CONNECT
//=======================================
void connectAWS() {

  net.setCACert(root_ca);
  net.setCertificate(device_cert);
  net.setPrivateKey(private_key);

  client.setServer(aws_endpoint, 8883);
  client.setCallback(callback);

  while (!client.connected()) {

    Serial.print("Connecting to AWS...");

    if (client.connect("MotorESP")) {

      Serial.println("Connected!");
      client.subscribe("motor/control");
      Serial.println("Subscribed to motor/control");

    } else {

      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

//=======================================
// SETUP
//=======================================
void setup() {

  Serial.begin(115200);

  myServo.attach(SERVO_PIN);

  // Stop initially
  myServo.write(90);

  connectWiFi();
  connectAWS();
}

//=======================================
// LOOP
//=======================================
void loop() {

  if (!client.connected()) {
    connectAWS();
  }

  client.loop();
}
