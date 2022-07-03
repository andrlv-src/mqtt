#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "secrets.h"

extern "C" {
#include <pr.h>
}

#define RX_PIN            16
#define TX_PIN            17
#define MQTT_BUFF_LEN     32
#define RX_BUFF_LEN       32
#define TOPIC_PLACE       "gb_iot/1148_LAV/place"
#define TOPIC_TIMEOUT     "gb_iot/1148_LAV/timeout"
#define TOPIC_TEMPERATURE "gb_iot/1148_LAV/temperature"
#define TOPIC_PRESSURE    "gb_iot/1148_LAV/pressure"
#define TOPIC_TEST        "gb_iot/1148_LAV/test"
#define TOPIC_GENERAL     "gb_iot/#"


WiFiClientSecure publisher;
WiFiClientSecure subscriber;

PubSubClient mqtt_pub;
PubSubClient mqtt_sub;

const char* ssid     = SSID;
const char* password = PASS;
const char* host ="mqtt.cloud.yandex.net";
const uint16_t port = 8883;
const char* rootCABuff = rootCA;
const char* pub_cert   = dev_cert;
const char* pub_key    = dev_key;
const char* sub_cert   = reg_cert;
const char* sub_key    = reg_key;

char rx_buff[RX_BUFF_LEN] = {0};
char mqtt_buff[MQTT_BUFF_LEN] = {0};
pr__data_t pr_data;

void mqtt_setup();
void mqtt_connect();
void sub_rx_cb(char* topic, byte* payload, unsigned int length);

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
    WiFi.begin(SSID, PASS);

    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\n\rConnected to WiFi");
    mqtt_setup();
}

void loop()
{
    if (Serial2.available() > 0) {
        size_t bytes = Serial2.readBytes(rx_buff, RX_BUFF_LEN);
        pr__parse_answer(rx_buff, &pr_data);

        char buff[RX_BUFF_LEN];

        sprintf(buff, "%d.%d C°", pr_data.temperature / 10, (pr_data.temperature % 10));
        mqtt_pub.publish(TOPIC_TEMPERATURE, buff);

        sprintf(buff, "%d mm Hg", (int) (pr_data.pressure / 133.3224));
        mqtt_pub.publish(TOPIC_PRESSURE, buff);
    }

    if (!mqtt_pub.connected() && !mqtt_sub.connected())
        mqtt_connect();

    mqtt_sub.loop();
    delay(1000);
}

void mqtt_setup()
{
    // Setting up publish client.
    publisher.setCACert(rootCABuff);
    publisher.setCertificate(pub_cert);
    publisher.setPrivateKey(pub_key);

    mqtt_pub.setClient(publisher);
    mqtt_pub.setServer(host, port);

    // Setting up subscriber client.
    subscriber.setCACert(rootCABuff);
    subscriber.setCertificate(sub_cert);
    subscriber.setPrivateKey(sub_key);

    mqtt_sub.setClient(subscriber);
    mqtt_sub.setServer(host, port);
    mqtt_sub.setCallback(sub_rx_cb);

    mqtt_connect();

    // Start messages.
    mqtt_pub.publish(TOPIC_PLACE, "living room");
    mqtt_pub.publish(TOPIC_TIMEOUT, "5000 ms");
}

void mqtt_connect()
{
    while(!mqtt_pub.connected()) {
        Serial.println("Publisher connection to mqtt...");
        if(mqtt_pub.connect("Pub")) {
            Serial.println("Mqtt publisher connected OK.");
        } else {
            Serial.println("Error, mqtt publisher connection fault.");
            Serial.println("Reconnecting...");
        }
    }

    if (!publisher.connected())
        Serial.println("Publisher client Error.");
    else
        Serial.println("Publisher OK.");


    while(!mqtt_sub.connected()) {
        Serial.println("Subscriber connection to mqtt...");
        if(mqtt_sub.connect("Sub")) {
            Serial.println("Mqtt subscriber connected OK.");
            mqtt_sub.subscribe(TOPIC_GENERAL);
        } else {
            Serial.println("Error, mqtt subscriber connection fault.");
            Serial.println("Reconnecting...");
        }
    }
    if (!subscriber.connected())
        Serial.println("Subscriber client Error.");
    else
        Serial.println("Subscriber OK.");
}

void sub_rx_cb(char* topic, byte* payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
        Serial.print((char) payload[i]);
    Serial.println();
}
