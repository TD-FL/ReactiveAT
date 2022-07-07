# ReactiveAT

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/ddododvic/library/ReactiveAt.svg)](https://registry.platformio.org/libraries/ddododvic/ReactiveAt)
[![GitHub issues](https://img.shields.io/github/issues/TD-FL/ReactiveAT.svg)](https://github.com/TD-FL/ReactiveAT/issues)
[![License](https://img.shields.io/badge/licence-Apache%202.0-blue)](https://github.com/TD-FL/ReactiveAT/blob/master/LICENSE)


Reactive Non Blocking GSM Modem Library for Arduino Framework.

## Suported Modems

- SIM800 series
- SIM900 series

More coming soon...

## Features

- Send and Receive SMS
- Receive phone calls
- Send Raw AT commands

## Example

```c++
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
```