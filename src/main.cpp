#include <Arduino.h>
#include <ReactiveAT.h>

// Init the library on hardware serial 2
// You can use any hardware or software serial
ReactiveAT reactiveAT(&Serial2);

void onCall(String number) // On call callback
{
    Serial.printf("Call from [%s]\n", number.c_str());
}

void onSms(String number, String content) // On SMS callback
{
    Serial.printf("SMS from [%s]: %s\n", number.c_str(), content.c_str());
}

void setup()
{
    Serial.begin(115200);
    Serial2.begin(9600);
    sleep(1);
    Serial.println("Incomming call and sms demo");

    while (!reactiveAT.moduleAvailable())
    {
        Serial.println("Waiting for modem");
        sleep(1);
    }

    reactiveAT.registerOnCall(onCall);
    reactiveAT.registerOnSms(onSms);
}

void loop()
{
    reactiveAT.poll();
}