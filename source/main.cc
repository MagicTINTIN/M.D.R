#include <iostream>
#include <cstring>
#include <string>
#include <cmath>
#include <thread>
#include <chrono>
#include <csignal>
#include <vector>
#include <random>
#include <stop_token>
#include <unistd.h>
#include <pthread.h>
#include "portmidi.h"
#include "midi/communication.hh"
#include "midi/controller.hh"
#include "midi/xtouch.hh"
#include "midi/animations.hh"
#include "dmx/usbPro.hh"

#define NUMBER_OF_MODES 4
int globalMode = NUMBER_OF_MODES - 1;
int globalSpeed = 127;
int switchLightsVar = 1;
std::vector<std::thread> globalActiveThreads = {};
unsigned char myDmx[530];

int mapRange(int x, int inMin, int inMax, int outMin, int outMax)
{
    return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

float mapRange(float x, float inMin, float inMax, float outMin, float outMax)
{
    return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

double log(double base, double x)
{
    return std::log(x) / std::log(base);
}

int getAnimSpeed()
{
    // return 2000 / std::log2(std::pow(globalSpeed,10)+1)+1;
    // return 1000 / (globalSpeed+1);
    // return 2000 / (std::pow(2 * globalSpeed, -2));
    return 500 / (log(2, 1 + std::pow(globalSpeed / 2, 4) / 100) + 1);
}

int switchLights(Controller *s, int val)
{
    // myDmx[1] = val == 127 ? 255 : 0;
    if (val == 0)
        return 0;
    switchLightsVar++;
    switchLightsVar %= 2;
    if (switchLightsVar == 1)
        return 0;
    memset(myDmx, 0, 530);
    return 0;
}

void resetDMX()
{
    memset(myDmx, 0, 530);
}

void startLight(int light, int value)
{
    if ((switchLightsVar == 0 && value > 0) || light < 1 || light > 512 || value < 0 || value > 255)
        return;
    myDmx[light] = value;
}

void finTransitionSoleil(int timeSpeed)
{
    for (int i = 0; i < 256; i++)
    {
        startLight(1, 255-i);
        usleep(timeSpeed * 10);
    }
}

void transitionSoleil(int timeSpeed)
{
    for (int i = 0; i < 256; i++)
    {
        startLight(2, i);
        usleep(timeSpeed * 10);
    }
    for (int i = 0; i < 256; i++)
    {
        startLight(2, 255-i);
        startLight(1, i);
        usleep(timeSpeed * 10);
    }

    if (globalSpeed > 0)
        finTransitionSoleil(timeSpeed);
}

void soleilFluctuant()
{
    while (true)
    {
        usleep(80000);
        startLight(1, 170 + std::rand() % 10);
    }
}

void soleilFort()
{
    while (true)
    {
        usleep(40000);
        startLight(1, 200 + std::rand() % 55);
    }
}

int updateOtherFaders(Controller *s, int val)
{
    // s->setSoundPeak(XTOUCH_CHANNELS, {mapRange(val, 127, 0, 8, 0),
    //                                   mapRange(val, 127, 0, 6, 0),
    //                                   mapRange(val, 127, 0, 4, 0),
    //                                   mapRange(val, 127, 0, 2, 0),
    //                                   mapRange(val, 127, 0, 2, 0),
    //                                   mapRange(val, 127, 0, 4, 0),
    //                                   mapRange(val, 127, 0, 6, 0),
    //                                   mapRange(val, 127, 0, 8, 0)});
    s->setFader({XTOUCH_FADER_2_CH,
                 XTOUCH_FADER_3_CH,
                 XTOUCH_FADER_4_CH,
                 XTOUCH_FADER_5_CH,
                 XTOUCH_FADER_6_CH,
                 XTOUCH_FADER_7_CH},
                {
                    mapRange(val, 127, 0, 0, 4),
                    mapRange(val, 127, 0, 0, 26),
                    mapRange(val, 127, 0, 0, 64),
                    mapRange(val, 127, 0, 0, 101),
                    mapRange(val, 127, 0, 0, 123),
                    mapRange(val, 127, 0, 0, 127),
                });
    return 0;
}

void yellowLineAnim(Controller *surface)
{
    int i = 0;
    int previousValue = -1;
    while (1)
    {
        if (globalSpeed > 0) {
            // //if (i % 100)
            // transitionSoleil(getAnimSpeed());
            surface->animTimeFunctionVector(XTOUCH_ROWS[2], &getAnimSpeed, 0);
            transitionSoleil(getAnimSpeed());
        }
        else
            usleep(10000);

        if (globalSpeed != previousValue)
        {
            previousValue = globalSpeed;
            surface->setLight(XTOUCH_DOWN, (globalSpeed > 2) ? 127 : 0);
            //startLight(1, (globalSpeed > 0) ? 0 : 255);
        }
        i++;
    }
}

void randomAnim(Controller *surface)
{
    surface->animRandomOfVector({70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90}, -1, 500, 0);
}

int getFaderAlertSin(int x, int &amp, int &period, int &ampOffset, int const &timeOffset)
{
    return amp * std::sin(2 * M_PI * (x + timeOffset * 3) / period) + ampOffset;
}

void redAlarm(Controller *surface)
{
    int timer = 0;
    int rndOffset = 30 + std::rand() % 67;
    int rndAmp = 10 + std::rand() % 15;
    int rndPeriod = 5 + std::rand() % 60;
    int period = 60;
    int memory = 81;
    int faders[memory] = {0};
    int topdanger = 0;
    int topDangerPeriod = 20;
    int dispersion = 5;
    while (1)
    {
        if (timer % period == 0)
        {
            surface->setLCDColor({2, 4, 6, 8}, XTOUCH_SCREEN_BLACK);
            surface->setLCDColor({1, 3, 5, 7}, XTOUCH_SCREEN_RED);

            rndPeriod = 20 + std::rand() % 100;
            startLight(7,255);
        }
        if (timer % period == period / 4)
        {
            surface->allLightsRed(XTOUCH_STATUS_ON);
        }
        if (timer % period == period / 2)
        {
            surface->setLCDColor({1, 3, 5, 7}, XTOUCH_SCREEN_BLACK);
            surface->setLCDColor({2, 4, 6, 8}, XTOUCH_SCREEN_RED);

            startLight(7,0);
        }
        if (timer % period == 3 * period / 4)
        {
            surface->allLightsRed(XTOUCH_STATUS_OFF);
        }
        if (timer % 9 == 0)
        {
            surface->setLight({61, 69, 83, 90}, XTOUCH_STATUS_ON);
            if (timer % 120 < 40)
            {
                surface->setLight({8, 11, 12, 15}, XTOUCH_STATUS_ON);
            }
        }
        if (timer % 9 == 4)
            surface->setLight({61, 69, 83, 90, 8, 11, 12, 15}, XTOUCH_STATUS_OFF);

        if (timer % 600 == 0)
        {
            rndOffset = 30 + std::rand() % 34;
            rndAmp = 10 + std::rand() % 15;
        }
        if (timer % 80 < 20)
            surface->setRing({1, 4, 5, 8}, {XTOUCH_RING_MODE_DISPERSION_WEXT, XTOUCH_RING_MODE_NORMAL, XTOUCH_RING_MODE_NORMAL, XTOUCH_RING_MODE_DISPERSION_WEXT}, {dispersion, 0, 0, dispersion});
        else if (timer % 80 < 40)
            surface->setRing({2, 1, 8, 7}, {XTOUCH_RING_MODE_DISPERSION_WEXT, XTOUCH_RING_MODE_NORMAL, XTOUCH_RING_MODE_NORMAL, XTOUCH_RING_MODE_DISPERSION_WEXT}, {dispersion, 0, 0, dispersion});
        else if (timer % 80 < 60)
            surface->setRing({3, 2, 7, 6}, {XTOUCH_RING_MODE_DISPERSION_WEXT, XTOUCH_RING_MODE_NORMAL, XTOUCH_RING_MODE_NORMAL, XTOUCH_RING_MODE_DISPERSION_WEXT}, {dispersion, 0, 0, dispersion});
        else
            surface->setRing({4, 3, 6, 5}, {XTOUCH_RING_MODE_DISPERSION_WEXT, XTOUCH_RING_MODE_NORMAL, XTOUCH_RING_MODE_NORMAL, XTOUCH_RING_MODE_DISPERSION_WEXT}, {dispersion, 0, 0, dispersion});
        if (timer % 20 == 0)
        {
            dispersion = 5;
        }
        if (timer % 4 == 0)
            dispersion--;
        for (size_t i = 0; i < 8; i++)
        {
            faders[i + 1] = faders[i];
        }
        faders[timer % memory] = getFaderAlertSin(timer, rndAmp, rndPeriod, rndOffset, 0);

        if (timer % 4 == 0)
        {
            topdanger++;
            if ((topdanger % topDangerPeriod) < XTOUCH_ROWS[0].size() * 2)
            {
                surface->setLight(XTOUCH_ROWS[0][topdanger % topDangerPeriod], XTOUCH_STATUS_ON);
            }
            else
            {
                surface->setLight(XTOUCH_ROWS[0][(topdanger - XTOUCH_ROWS[0].size() * 2) % topDangerPeriod], XTOUCH_STATUS_OFF);
            }
        }

        surface->setFader(XTOUCH_FADERS, {faders[(timer) % memory],
                                          faders[(timer + memory / 9) % memory],
                                          faders[(timer + 2 * memory / 9) % memory],
                                          faders[(timer + 3 * memory / 9) % memory],
                                          faders[(timer + 4 * memory / 9) % memory],
                                          faders[(timer + 5 * memory / 9) % memory],
                                          faders[(timer + 6 * memory / 9) % memory],
                                          faders[(timer + 7 * memory / 9) % memory],
                                          faders[(timer + 8 * memory / 9) % memory]});

        usleep(1000000 / period);
        timer++;
        // timer %= period;
    }
}

void greenChaser(Controller *surface)
{
    while (1)
    {
        usleep(2000000);
        surface->animFilledVector(XTOUCH_BTN_GREEN, 60, 0);
        surface->setLight({0, 7}, {127, 127});
        usleep(100000);
        surface->setLight({0, 7}, {0, 0});
    }
}



int modesChanger(Controller *surface, int val)
{
    if (val == 0)
        return 0;

    globalMode++;
    globalMode %= NUMBER_OF_MODES;
    std::cout << "cleaning threads...\n";
    for (auto &thread : globalActiveThreads)
    {
        pthread_cancel(thread.native_handle());
    }
    surface->resetFunctionsToTrigger();
    surface->resetValuesToUpdate();
    surface->globalReset();
    resetDMX();
    std::cout << "=> MODE : " << globalMode << std::endl;
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SCRUB, &modesChanger});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_INST, &switchLights});

    if (globalMode == 0)
    {
        surface->addValueToUpdate({TRIGGER_FADER_TYPE, XTOUCH_FADER_1_CH, &globalSpeed});
        surface->addFunctionToTrigger({TRIGGER_FADER_TYPE, XTOUCH_FADER_1_CH, &updateOtherFaders});
        surface->setLCDFullLineText(0, "Commandes moteurs principaux");
        surface->setLCDColor({1, 8}, XTOUCH_SCREEN_YELLOW);
        surface->setLCDColor({3, 4, 5, 6}, XTOUCH_SCREEN_WHITE);
        surface->setFader({XTOUCH_FADER_2_CH, XTOUCH_FADER_3_CH, XTOUCH_FADER_4_CH, XTOUCH_FADER_5_CH, XTOUCH_FADER_6_CH, XTOUCH_FADER_7_CH}, 0);
        surface->setFader(XTOUCH_FADER_1_CH, 127);
        surface->setFader(XTOUCH_FADER_8_CH, 69);

        globalActiveThreads.emplace_back(yellowLineAnim, surface);
        globalActiveThreads.emplace_back(randomAnim, surface);
    }
    else if (globalMode == 1)
    {
        surface->setLCDColor({1, 8}, XTOUCH_SCREEN_GREEN);
        surface->setLCDColor({4, 5}, XTOUCH_SCREEN_BLUE);
        surface->setLight(XTOUCH_ZOOM, XTOUCH_STATUS_BLINK);
        surface->setFader(XTOUCH_FADERS, {90, 74, 65, 60, 55, 60, 65, 74, 90});
        globalActiveThreads.emplace_back(greenChaser, surface);
        globalActiveThreads.emplace_back(soleilFluctuant);
    }
    else if (globalMode == 2)
    {
        surface->setLCDText({1, 3, 5, 7}, {0, 0, 0, 0}, "ALERT!");
        surface->setLCDText({2, 4, 6, 8}, {0, 0, 0, 0}, "DANGER");
        globalActiveThreads.emplace_back(redAlarm, surface);
        globalActiveThreads.emplace_back(soleilFort);
    }

    return 0;
}

int main()
{
    std::cout << "Starting mdr\n";
    std::string surfaceName = "X-Touch X-TOUCH_INT";

    memset(myDmx, 0, 530);
    std::thread DMXUpdate(&startDMX, myDmx);

    if (initializePortMidi() != pmNoError)
    {
        return 1;
    }
    PmDeviceID dInputId = Pm_GetDefaultInputDeviceID();
    PmDeviceID dOutputId = Pm_GetDefaultOutputDeviceID();
    getAllDeviceInfo();

    if (!findController(dInputId, dOutputId, surfaceName))
    {
        std::cout << "Could not find X-Touch device among MIDI controllers\n";
        return 2;
    }
    Controller surface(surfaceName, dInputId, dOutputId);

    // adding values to be updated
    surface.addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SCRUB, &modesChanger});
    surface.globalReset();

    // surface.setSegmentsChar({9, 8, 7, 6, 5, 4, 3}, {'b', 'o', 'n', 'j', 'o', 'u', 'r'});
    // surface.analyser(176, 176, 48, 55, 0, 255, 20);
    // initAnim(surface);
    // surface.animLCDRainbow(100, 8, true);
    // surface.setLCDColor(XTOUCH_CHANNELS, XTOUCH_SCREEN_YELLOW);
    // surface.setLCDText(1, 0, "Nice !");
    // surface.setLCDFullLineText(1,"C'est marrant !");
    // surface.setLCDText(7, 1, "Coucou");
    // surface.setLCDText(8, 0, "Yo!", 1);

    surface.startThreads();

    terminatePortMidi();
    std::cout << "Ending mdr\n";
    return 0;
}