#ifndef REACTIVEAT_H
#define REACTIVEAT_H

#include <Arduino.h>
#include <ArduinoQueue.h>

#define DEFAULT_TIMEOUT 10000
#define MAX_QUEUE_ELEMS 20

typedef std::function<void(String response)> ATCallback;
typedef std::function<void(String number)> OnCallCallback;
typedef std::function<void(String number, String content)> OnSmsCallback;
typedef std::function<void(bool sucess)> ResultCallback;

struct ATCommand
{
    String command;
    ATCallback callbackFn;
};

class ReactiveAT
{
private:
    Stream *_stream;
    ArduinoQueue<ATCommand> *callbackQueue = new ArduinoQueue<ATCommand>(MAX_QUEUE_ELEMS);
    OnCallCallback onCallFunc = nullptr;
    OnSmsCallback onSmsFunc = nullptr;

    bool isSuccessful(String response);
    void waitForStream();
    String readResponse();
    void responseReacived();
    void handleResponse(String response);
    void sendData(String data);

public:
    ReactiveAT(void);
    ReactiveAT(Stream *stream);

    bool moduleAvailable();

    String sendAT(String command);
    void sendATAsync(String command, ATCallback ATCallback);

    String sendATRaw(String command);
    void sendATRawAsync(String command, ATCallback ATCallback);

    bool sendSms(String number, String body);
    void sendSmsAsync(String number, String body, ResultCallback result);

    void registerOnSms(OnSmsCallback smsCallback);
    void registerOnCall(OnCallCallback callCallback);

    void poll();
};

#endif