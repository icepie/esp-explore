#include <Esp.h>

class Led
{
public:
    Led();
    Led(int userLedPin);
    ~Led();
    void on();
    void off();
    int getStatus();
    int getLedPin();
    void setLedPin(int userLedPin);

private:
    int ledPin = LED_BUILTIN;
};

Led::Led()
{
    Serial.println("Led Object Created.");
    pinMode(ledPin, OUTPUT);
}

Led::Led(int userLedPin)
{
    Serial.println("Led Object Created.");
    ledPin = userLedPin;
    pinMode(ledPin, OUTPUT);
}

Led::~Led()
{
    Serial.println("Led Object Deleted.");
}

void Led::on()
{
    digitalWrite(ledPin, HIGH);
}

void Led::off()
{
    digitalWrite(ledPin, LOW);
}

int Led::getStatus()
{
    return digitalRead(ledPin);
}

int Led::getLedPin()
{
    return ledPin;
}

void Led::setLedPin(int userLedPin)
{
    ledPin = userLedPin;
    pinMode(ledPin, OUTPUT);
}