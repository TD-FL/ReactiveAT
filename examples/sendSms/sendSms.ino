#include <Arduino.h>
#include <ReactiveAT.h>

// Init the library on hardware serial 2
// You can use any hardware or software serial
ReactiveAT reactiveAT(&Serial2);

void setup()
{
    Serial.begin(115200);
    Serial2.begin(9600);
    sleep(1);
    Serial.println("Send SMS demo start");

    while (!reactiveAT.moduleAvailable())
    {
        Serial.println("Waiting for modem");
        sleep(1);
    }

    reactiveAT.sendSmsAsync("NUMBER", "TEST RACTIVEAT", [](bool res)
        {
            if (res)
            {
                Serial.println("Successfuly sent SMS");
            }
            else
            {
                Serial.println("Sending failed");
            }
        }
    );
}

void loop()
{
    reactiveAT.poll();
}