#include <peripherals.h>
#include <Arduino.h>
#include <SimpleRotary.h>
#include <AsyncMqttClient.h>
#include <Adafruit_NeoPixel.h>
#include <Debug.h>

extern AsyncMqttClient mqttClient;
extern Neopixelstick pixels;
extern RpiState state;
extern bool bPendingAliveRequest;
extern uint nUnansweredAliveRequests;

/////////////////////////////////////////////////////////////////////////////////////
// Raspberry Pi control functions
/////////////////////////////////////////////////////////////////////////////////////

unsigned long lastAliveRequest;
unsigned long startShutdown;

void startup()
{
    DEBUG_P("startup");
    // To start the raspberry pi, the power has to be turned on
    digitalWrite(PIN_POWER, HIGH);

    lastAliveRequest = millis();
    state = rpiStartup;
}

void shutdown()
{
    DEBUG_P("shutdown");
    // send a shutdown request to the raspberry pi
    mqttClient.publish("volumio", 0, false, "shutdown");
    
    startShutdown = millis();
    state = rpiShutdown;
}

void isRpiAlive()
{
    // check periodically, if rpi is alive
    unsigned long currentMillis = millis();
    if((currentMillis - lastAliveRequest) > isAliveInterval)
    {
        // check if a pending alive request kept unanswered
        if(bPendingAliveRequest)
        {
            nUnansweredAliveRequests++;
            if(nUnansweredAliveRequests > 500)
            {
                shutdown();
            }
        }
        // send a new alive request
        mqttClient.publish("volumio", 0, false, "isRpiAlive");
        // Only if RPi state is rpiUp, an answer is expected
        bPendingAliveRequest = (state == rpiUp) ? true : false;

        lastAliveRequest = currentMillis;
    }
}

void isRpiDown()
{
    // check if raspberry pi had enough time to shutdown gracefully
    unsigned long currentMillis = millis();
    if((currentMillis - startShutdown) > shutdownTime)
    {
        // Power off the raspberry pi
        digitalWrite(PIN_POWER, LOW);

        state = rpiDown;
    }
}

void handleState(RpiState actualState)
{
    switch(actualState)
    {
        case rpiDown:
        default:
            // nothing to do
        break;
        case rpiStartup:
        case rpiUp:
            // send periodically isAlive requests
            isRpiAlive();
        break;
        case rpiShutdown:
            isRpiDown();
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// Switch Encoder functions
/////////////////////////////////////////////////////////////////////////////////////

unsigned long buttonPressStart;

void buttonPressed()
{
    DEBUG_P("Pressed");
    buttonPressStart = millis();
    pixels.startFadeIn();
}

void buttonReleased()
{
    DEBUG_P("Released");
    if((millis() - buttonPressStart) > longpress)
    {
        // long press
        if(state == rpiDown)
        {
            startup();
        }
        else if((state == rpiStartup)||(state == rpiUp))
        {
            shutdown();
        }
    }
    else
    {
        // short press
        if(state == rpiDown)
        {
            startup();
        }
        else if(state == rpiUp)
        {
            mqttClient.publish("volumio", 0, false, "togglePlayPause");
        }
    }
}

void turnClockwise()
{
    DEBUG_P("CW");
    mqttClient.publish("volumio", 0, false, "VolumeUp");
    pixels.startFadeIn();
}

void turnCounterclockwise()
{
    DEBUG_P("CCW");
    mqttClient.publish("volumio", 0, false, "VolumeDown");
    pixels.startFadeIn();
}


/////////////////////////////////////////////////////////////////////////////////////
// class Neopixelstick
/////////////////////////////////////////////////////////////////////////////////////

Neopixelstick::Neopixelstick(uint16_t anzahlPixel, uint16_t pinNummer)
{
    m_previousMillis = 0;
    m_startLightOn = 0;
    m_lightOnTime = 5000;
    m_brightness = 0;
    m_bfadeIn = false;
    m_bfadeOut = false;
    m_numPixels = anzahlPixel;
    m_pPixels = new Adafruit_NeoPixel(anzahlPixel, pinNummer, NEO_GRB + NEO_KHZ800);
}

Neopixelstick::~Neopixelstick()
{
    delete m_pPixels;
}

void Neopixelstick::setup()
{
    m_pPixels->begin();
}

void Neopixelstick::startFadeIn()
{
    m_bfadeIn = true;
}

void Neopixelstick::check(unsigned long millis)
{
    if(millis - m_previousMillis >= 10)
    {
        m_previousMillis = millis;
        // light is fading in
        if(m_bfadeIn)
        {
            m_bfadeOut = false;
            if(m_brightness < 255)
            {
                m_brightness++;
                setBrightness(m_brightness);
            }
            else
            {
                m_bfadeIn = false;
                m_startLightOn = millis;
            }
        }
        // light on
        if((m_startLightOn > 0) && (millis - m_startLightOn > m_lightOnTime))
        {
            m_bfadeOut = true;  
        }
        // light is fading out
        if(m_bfadeOut)
        {
            if(m_brightness > 0)
            {
                m_brightness--;
                setBrightness(m_brightness);
            }
            else
            {
                m_bfadeOut = false;
                m_startLightOn = 0;
            }
        }
    }
}

void Neopixelstick::setBrightness(uint8_t brightness)
{
    for(int i=0; i<m_numPixels; i++)
    {
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      m_pPixels->setPixelColor(i, m_pPixels->Color(brightness,brightness,brightness));
      m_pPixels->show(); // This sends the updated pixel color to the hardware.
    } 
}