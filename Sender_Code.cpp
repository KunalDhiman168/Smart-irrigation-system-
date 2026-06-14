#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>

// =========================
// WiFi Credentials
// =========================
const char* ssid = "your_wifi";
const char* password = "password";

// =========================
// AWS Endpoint
// =========================
const char* aws_endpoint = "aws_endpoint";

// =========================
// Root CA
// =========================
const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
YOUR_RootCA_CERTIFICATE_HERE
-----END CERTIFICATE-----
)EOF";

// =========================
// Device Certificate
// =========================
const char* device_cert = R"KEY(
-----BEGIN CERTIFICATE-----
YOUR_DEVICE_CERTIFICATE_HERE
-----END CERTIFICATE-----
)KEY";

// =========================
// Private Key
// =========================
const char* private_key = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
YOUR_PRIVATE_KEY_HERE
-----END RSA PRIVATE KEY-----
)KEY";

// =========================
// MQTT Topics
// =========================
const char* publish_topic = "soil/data";
const char* subscribe_topic = "motor/control";

// =========================
// Objects
// =========================
WiFiClientSecure net;
PubSubClient client(net);

// =========================
// MQTT Callback
// =========================
void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("\nMessage arrived on topic: ");
  Serial.println(topic);

  String incoming = "";

  for (unsigned int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }

  Serial.print("Received: ");
  Serial.println(incoming);

  if (incoming.indexOf("OPEN") >= 0) {
    Serial.println("Motor ON");
    // digitalWrite(RELAY_PIN, HIGH);   // Add later
  }
  else if (incoming.indexOf("CLOSE") >= 0) {
    Serial.println("Motor OFF");
    // digitalWrite(RELAY_PIN, LOW);    // Add later
  }
}

// =========================
// AWS Connect
// =========================
void connectAWS() {

  net.setCACert(root_ca);
  net.setCertificate(device_cert);
  net.setPrivateKey(private_key);

  client.setServer(aws_endpoint, 8883);

  while (!client.connected()) {
    Serial.print("Connecting to AWS...");

    if (client.connect("ESP32Client")) {
      Serial.println("Connected!");

      if (client.subscribe(subscribe_topic)) {
        Serial.println("Subscribed to motor/control");
      } else {
        Serial.println("Subscription failed");
      }

    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

// =========================
// Setup
// =========================
void setup() {

  Serial.begin(115200);
  delay(1000);

  WiFi.setSleep(false);

  // Optional relay pin for future motor
  // pinMode(RELAY_PIN, OUTPUT);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");

  // =========================
  // Internet Check
  // =========================
  WiFiClient test;
  Serial.println("Checking internet...");

  if (test.connect("google.com", 80)) {
    Serial.println("Internet OK");
    test.stop();
  } else {
    Serial.println("Internet FAILED");
  }

  // =========================
  // Time Sync
  // =========================
  Serial.println("Syncing time...");

  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");

  time_t now = time(nullptr);
  int retry = 0;

  while (now < 100000 && retry < 20) {
    delay(1000);
    Serial.print(".");
    now = time(nullptr);
    retry++;
  }

  if (now < 100000) {
    Serial.println("\nTime sync FAILED");
  } else {
    Serial.println("\nTime synced!");
  }

  // =========================
  // MQTT Callback
  // =========================
  client.setCallback(messageHandler);

  // =========================
  // AWS Connect
  // =========================
  connectAWS();
}

// =========================
// Main Loop
// =========================
void loop() {

  if (!client.connected()) {
    connectAWS();
  }

  client.loop();

  // Read moisture sensor from GPIO34
  int moisture = analogRead(34);

  // Create JSON payload
  String payload = "{\"moisture\": " + String(moisture) + "}";

  // Publish to AWS
  client.publish(publish_topic, payload.c_str());

  // Serial output
  Serial.println(payload);

  delay(3000);
}
