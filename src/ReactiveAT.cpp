#include <Arduino.h>
#include <ReactiveAT.h>

ReactiveAT::ReactiveAT(void)
{
}

ReactiveAT::ReactiveAT(Stream *stream)
{
    this->_stream = stream;
}

bool ReactiveAT::moduleAvailable()
{
    auto res = this->sendATRaw("AT");

    return res.indexOf("OK") > -1;
}

String ReactiveAT::sendATRaw(String command)
{
    command += "\r\n";

    this->_stream->print(command);

    this->waitForStream();

    return this->readResponse();
}

void ReactiveAT::sendATRawAsync(String command, ATCallback atCallback)
{
    ATCommand atCommand;

    int equalsIndex = command.indexOf("=");
    if (equalsIndex > -1)
        atCommand.command = command.substring(0, equalsIndex - 1);
    else
        atCommand.command = command;

    int atIndex = command.indexOf("AT+");
    if (atIndex > -1)
        atCommand.command = atCommand.command.substring(3);

    atCommand.callbackFn = atCallback;

    if (!callbackQueue->isFull())
    {
        this->callbackQueue->enqueue(atCommand);

        command += "\r\n";

        this->_stream->print(command);
    }
}

String ReactiveAT::sendAT(String command)
{
    command = "AT+" + command;
    return this->sendATRaw(command);
}

void ReactiveAT::sendATAsync(String command, ATCallback atCallback)
{
    command = "AT+" + command;
    this->sendATRawAsync(command, atCallback);
}

void ReactiveAT::sendData(String data)
{
    this->_stream->print(data + "\r\n");
    this->_stream->print((char)26);
}

void ReactiveAT::waitForStream()
{
    auto startTime = millis();
    while (!this->_stream->available() && !(startTime + DEFAULT_TIMEOUT < millis()))
    {
        delay(10);
    }
}

String ReactiveAT::readResponse()
{
    String response = "";

    while (this->_stream->available())
    {
        response += (char)this->_stream->read();
    }

    return response;
}

void ReactiveAT::responseReacived()
{
    auto response = this->readResponse();

    if (!this->callbackQueue->isEmpty())
    {
        auto atCommand = this->callbackQueue->getHead();
        int tIndex = atCommand.command.indexOf("T");
        if (tIndex > -1)
        {
            return;
        }

        if (response.indexOf(atCommand.command) > -1)
        {
            this->callbackQueue->dequeue();
            atCommand.callbackFn(response);
            return;
        }
    }

    handleResponse(response);
}

void ReactiveAT::handleResponse(String response)
{
    if (response.indexOf("+CLIP:") > -1) // Incomming call
    {
        int quote1 = response.indexOf("\"", 10) + 1;
        int quote2 = response.indexOf("\"", quote1);

        String callingNumber = response.substring(quote1, quote2);

        if (this->onCallFunc != nullptr)
        {
            this->onCallFunc(callingNumber);
        }

        this->sendATAsync("CHUP", [](String r) {}); // hang up
    }
    else if (response.indexOf("+CMTI: \"SM\"") > -1) // got sms now prepare read
    {
        auto smsId = response.substring(14);
        String getSmsCommand = "CMGR=" + smsId;
        sendATAsync(getSmsCommand, [](String res) {});
    }
    else if (response.indexOf("+CMGR: \"REC UNREAD\"") > -1) // read sms
    {
        int comma1 = response.indexOf(",", 10) + 2;
        int comma2 = response.indexOf(",", comma1) - 1;

        String smsNumber = response.substring(comma1, comma2);

        int br1 = response.indexOf('\n', 2) + 1;
        int br2 = response.indexOf('\n', br1);

        String smsBody = response.substring(br1, br2);

        if (this->onSmsFunc != nullptr)
        {
            this->onSmsFunc(smsNumber, smsBody);
        }
    }
}

void ReactiveAT::registerOnCall(OnCallCallback callCalback)
{
    this->sendATRaw("AT+CLIP=1");
    this->onCallFunc = callCalback;
}

void ReactiveAT::registerOnSms(OnSmsCallback smsCallback)
{
    this->sendATRaw("AT+CMGF=1");
    this->onSmsFunc = smsCallback;
}

void ReactiveAT::poll()
{
    if (this->_stream->available())
    {
        this->responseReacived();
    }
}

bool ReactiveAT::sendSms(String number, String body)
{
    String setSmsMode = "CMGF=1";
    String smsCommand = "CMGS=\"" + number + "\"";

    this->sendAT(setSmsMode);
    auto res = this->sendAT(smsCommand + "\r\n");
    this->sendData(body);

    return this->isSuccessful(res);
}

void ReactiveAT::sendSmsAsync(String number, String body, ResultCallback resultCallback)
{
    String setSmsMode = "CMGF=1";
    String smsCommand = "CMGS=\"" + number + "\"";

    this->sendATAsync(setSmsMode,
                      [this, body, smsCommand, resultCallback](String response)
                      {
                          this->sendATAsync(smsCommand,
                                            [this, body, resultCallback](String response)
                                            {
                                                this->sendData(body);
                                                resultCallback(this->isSuccessful(response));
                                            });
                      });
}

bool ReactiveAT::isSuccessful(String response)
{
    return response.indexOf("ERR") < 0;
}