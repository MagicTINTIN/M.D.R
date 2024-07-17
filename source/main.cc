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

#define NUMBER_OF_MODES 3
int globalMode = NUMBER_OF_MODES - 1;
int globalSpeed = 127;
int switchLightsVar = 1;
int switchLedLightsVar = 1;
std::vector<std::thread> globalActiveThreads = {};
unsigned char myDmx[530];

int lyreSpots[3] = {100, 113, 126};
int ledAddresses[4] = {7, 11, 17, 23};

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

void dateTimeThread(Controller *s)
{
    while (true)
    {
        s->displayCurrentDateTime();
        usleep(1000000);
    }
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

void startLight(int light, int value, int timespeed)
{
    startLight(light, 2.8 * value / log(7, timespeed));
}

void startLight(std::vector<int> lights, int value)
{
    for (int light : lights)
    {
        startLight(light, value);
    }
}

int switchLedLights(Controller *s, int val)
{
    if (val == 0)
        return 0;
    switchLedLightsVar++;
    switchLedLightsVar %= 2;
    s->setLight(XTOUCH_FLIP, switchLedLightsVar == 1 ? 127 : 0);
    for (int i = 0; i < 4; i++)
    {
        // printf("meh\n");
        startLight(ledAddresses[i] + (globalMode == 0 ? 0 : 2), switchLedLightsVar == 1 ? 255 : 0);
    }
    return 0;
}

int halogen(Controller *surface, int val)
{
    startLight(1, val * 2);
    startLight(2, val * 2);
    return 0;
}

int focusMH(Controller *surface, int val)
{
    startLight(lyreSpots[0] + 9, val);
    startLight(lyreSpots[1] + 9, val);
    startLight(lyreSpots[2] + 9, val);
    return 0;
}

int colorMH(Controller *surface, int val)
{
    startLight(lyreSpots[0] + 0, val);
    startLight(lyreSpots[1] + 0, val);
    startLight(lyreSpots[2] + 0, val);
    return 0;
}

int offJardinMH(Controller *surface, int val)
{
    if (val == 0)
        return 0;
    startLight(lyreSpots[0] + 3, 0);
    surface->setLight({XTOUCH_SELECT1, XTOUCH_SELECT2, XTOUCH_MUTE1, XTOUCH_MUTE2, XTOUCH_SOLO1, XTOUCH_SOLO2, XTOUCH_REC1, XTOUCH_REC2}, {127, 127, 0, 0, 0, 0, 0, 0});
    return 0;
}

int offCenterMH(Controller *surface, int val)
{
    if (val == 0)
        return 0;
    startLight(lyreSpots[1] + 3, 0);
    surface->setLight({XTOUCH_SELECT3, XTOUCH_SELECT4, XTOUCH_MUTE3, XTOUCH_MUTE4, XTOUCH_SOLO3, XTOUCH_SOLO4, XTOUCH_REC3, XTOUCH_REC4}, {127, 127, 0, 0, 0, 0, 0, 0});
    return 0;
}

int offCourMH(Controller *surface, int val)
{
    if (val == 0)
        return 0;
    startLight(lyreSpots[2] + 3, 0);
    surface->setLight({XTOUCH_SELECT5, XTOUCH_SELECT6, XTOUCH_MUTE5, XTOUCH_MUTE6, XTOUCH_SOLO5, XTOUCH_SOLO6, XTOUCH_REC5, XTOUCH_REC6}, {127, 127, 0, 0, 0, 0, 0, 0});
    return 0;
}

int midLowJardinMH(Controller *surface, int val)
{
    if (val == 0)
        return 0;
    startLight(lyreSpots[0] + 3, 32);
    surface->setLight({XTOUCH_SELECT1, XTOUCH_SELECT2, XTOUCH_MUTE1, XTOUCH_MUTE2, XTOUCH_SOLO1, XTOUCH_SOLO2, XTOUCH_REC1, XTOUCH_REC2}, {127, 127, 127, 127, 0, 0, 0, 0});
    return 0;
}

int midLowCenterMH(Controller *surface, int val)
{
    if (val == 0)
        return 0;
    startLight(lyreSpots[1] + 3, 32);
    surface->setLight({XTOUCH_SELECT3, XTOUCH_SELECT4, XTOUCH_MUTE3, XTOUCH_MUTE4, XTOUCH_SOLO3, XTOUCH_SOLO4, XTOUCH_REC3, XTOUCH_REC4}, {127, 127, 127, 127, 0, 0, 0, 0});
    return 0;
}

int midLowCourMH(Controller *surface, int val)
{
    if (val == 0)
        return 0;
    startLight(lyreSpots[2] + 3, 32);
    surface->setLight({XTOUCH_SELECT5, XTOUCH_SELECT6, XTOUCH_MUTE5, XTOUCH_MUTE6, XTOUCH_SOLO5, XTOUCH_SOLO6, XTOUCH_REC5, XTOUCH_REC6}, {127, 127, 127, 127, 0, 0, 0, 0});
    return 0;
}

int midHighJardinMH(Controller *surface, int val)
{
    if (val == 0)
        return 0;
    startLight(lyreSpots[0] + 3, 48);
    surface->setLight({XTOUCH_SELECT1, XTOUCH_SELECT2, XTOUCH_MUTE1, XTOUCH_MUTE2, XTOUCH_SOLO1, XTOUCH_SOLO2, XTOUCH_REC1, XTOUCH_REC2}, {127, 127, 127, 127, 127, 127, 0, 0});
    return 0;
}

int midHighCenterMH(Controller *surface, int val)
{
    if (val == 0)
        return 0;
    startLight(lyreSpots[1] + 3, 48);
    surface->setLight({XTOUCH_SELECT3, XTOUCH_SELECT4, XTOUCH_MUTE3, XTOUCH_MUTE4, XTOUCH_SOLO3, XTOUCH_SOLO4, XTOUCH_REC3, XTOUCH_REC4}, {127, 127, 127, 127, 127, 127, 0, 0});
    return 0;
}

int midHighCourMH(Controller *surface, int val)
{
    if (val == 0)
        return 0;
    startLight(lyreSpots[2] + 3, 48);
    surface->setLight({XTOUCH_SELECT5, XTOUCH_SELECT6, XTOUCH_MUTE5, XTOUCH_MUTE6, XTOUCH_SOLO5, XTOUCH_SOLO6, XTOUCH_REC5, XTOUCH_REC6}, {127, 127, 127, 127, 127, 127, 0, 0});
    return 0;
}

int fullJardinMH(Controller *surface, int val)
{
    if (val == 0)
        return 0;
    startLight(lyreSpots[0] + 3, 127);
    surface->setLight({XTOUCH_SELECT1, XTOUCH_SELECT2, XTOUCH_MUTE1, XTOUCH_MUTE2, XTOUCH_SOLO1, XTOUCH_SOLO2, XTOUCH_REC1, XTOUCH_REC2}, {127, 127, 127, 127, 127, 127, 127, 127});
    return 0;
}

int fullCenterMH(Controller *surface, int val)
{
    if (val == 0)
        return 0;
    startLight(lyreSpots[1] + 3, 127);
    surface->setLight({XTOUCH_SELECT3, XTOUCH_SELECT4, XTOUCH_MUTE3, XTOUCH_MUTE4, XTOUCH_SOLO3, XTOUCH_SOLO4, XTOUCH_REC3, XTOUCH_REC4}, {127, 127, 127, 127, 127, 127, 127, 127});
    return 0;
}

int fullCourMH(Controller *surface, int val)
{
    if (val == 0)
        return 0;
    startLight(lyreSpots[2] + 3, 127);
    surface->setLight({XTOUCH_SELECT5, XTOUCH_SELECT6, XTOUCH_MUTE5, XTOUCH_MUTE6, XTOUCH_SOLO5, XTOUCH_SOLO6, XTOUCH_REC5, XTOUCH_REC6}, {127, 127, 127, 127, 127, 127, 127, 127});
    return 0;
}

int panJardinMH(Controller *surface, int val)
{
    // startLight(lyreSpots[0] + 4, val * 2);
    startLight(lyreSpots[0] + 4, mapRange(val, 0, 127, 42, 127));
    return 0;
}

int panCenterMH(Controller *surface, int val)
{
    // startLight(lyreSpots[1] + 4, val * 2);
    startLight(lyreSpots[1] + 4, mapRange(val, 0, 127, 42, 127));
    return 0;
}

int panCourMH(Controller *surface, int val)
{
    // startLight(lyreSpots[2] + 4, val * 2);
    startLight(lyreSpots[2] + 4, mapRange(val, 0, 127, 42, 127));
    return 0;
}

int tiltJardinMH(Controller *surface, int val)
{
    startLight(lyreSpots[0] + 5, val * 2);
    return 0;
}

int tiltCenterMH(Controller *surface, int val)
{
    startLight(lyreSpots[1] + 5, val * 2);
    return 0;
}

int tiltCourMH(Controller *surface, int val)
{
    startLight(lyreSpots[2] + 5, val * 2);
    return 0;
}

void setModeLCDs(Controller *surface, int mode)
{
    surface->setLCDColor({3, 4}, XTOUCH_SCREEN_WHITE);
    surface->setLCDColor(7, XTOUCH_SCREEN_CYAN);
    surface->setLCDColor(8, XTOUCH_SCREEN_GREEN);
    if (mode == 0)
    {
        surface->setLCDColor({1, 2, 5, 6}, XTOUCH_SCREEN_RED);
    }
    else
    {
        surface->setLCDColor({1, 2, 5, 6}, XTOUCH_SCREEN_BLUE);
    }
    surface->setLCDText({1, 2}, {0, 0}, "LYRE 1");
    usleep(100);
    surface->setLCDText({3, 4}, {0, 0}, "LYRE 2");
    usleep(100);
    surface->setLCDText({5, 6}, {0, 0}, "LYRE 3");
    usleep(100);
    surface->setLCDText({7, 8}, {0, 0}, "LYRES");
    usleep(100);
    surface->setLCDText({1, 3, 5}, {1, 1, 1}, "PAN");
    usleep(100);
    surface->setLCDText({2, 4, 6}, {1, 1, 1}, "TILT");
    usleep(100);
    surface->setLCDText(7, 1, "FOCUS");
    usleep(100);
    surface->setLCDText(8, 1, "COLOR");
    usleep(100);

    surface->setLight({XTOUCH_SELECT1, XTOUCH_SELECT2, XTOUCH_MUTE1, XTOUCH_MUTE2, XTOUCH_SOLO1, XTOUCH_SOLO2, XTOUCH_REC1, XTOUCH_REC2}, {127, 127, 0, 0, 0, 0, 0, 0});
    usleep(100);
    surface->setLight({XTOUCH_SELECT3, XTOUCH_SELECT4, XTOUCH_MUTE3, XTOUCH_MUTE4, XTOUCH_SOLO3, XTOUCH_SOLO4, XTOUCH_REC3, XTOUCH_REC4}, {127, 127, 0, 0, 0, 0, 0, 0});
    usleep(100);
    surface->setLight({XTOUCH_SELECT5, XTOUCH_SELECT6, XTOUCH_MUTE5, XTOUCH_MUTE6, XTOUCH_SOLO5, XTOUCH_SOLO6, XTOUCH_REC5, XTOUCH_REC6}, {127, 127, 0, 0, 0, 0, 0, 0});
    usleep(100);
    surface->setFader({XTOUCH_FADER_1_CH,XTOUCH_FADER_3_CH,XTOUCH_FADER_5_CH},64);
    panJardinMH(surface, 64);
    panCenterMH(surface, 64);
    panCourMH(surface, 64);

    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SELECT1, &offJardinMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SELECT2, &offJardinMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SELECT3, &offCenterMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SELECT4, &offCenterMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SELECT5, &offCourMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SELECT6, &offCourMH});

    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_MUTE1, &midLowJardinMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_MUTE2, &midLowJardinMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_MUTE3, &midLowCenterMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_MUTE4, &midLowCenterMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_MUTE5, &midLowCourMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_MUTE6, &midLowCourMH});

    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SOLO1, &midHighJardinMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SOLO2, &midHighJardinMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SOLO3, &midHighCenterMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SOLO4, &midHighCenterMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SOLO5, &midHighCourMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_SOLO6, &midHighCourMH});

    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_REC1, &fullJardinMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_REC2, &fullJardinMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_REC3, &fullCenterMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_REC4, &fullCenterMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_REC5, &fullCourMH});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_REC6, &fullCourMH});

    surface->addFunctionToTrigger({TRIGGER_FADER_TYPE, XTOUCH_FADER_1_CH, &panJardinMH});
    surface->addFunctionToTrigger({TRIGGER_FADER_TYPE, XTOUCH_FADER_2_CH, &tiltJardinMH});
    surface->addFunctionToTrigger({TRIGGER_FADER_TYPE, XTOUCH_FADER_3_CH, &panCenterMH});
    surface->addFunctionToTrigger({TRIGGER_FADER_TYPE, XTOUCH_FADER_4_CH, &tiltCenterMH});
    surface->addFunctionToTrigger({TRIGGER_FADER_TYPE, XTOUCH_FADER_5_CH, &panCourMH});
    surface->addFunctionToTrigger({TRIGGER_FADER_TYPE, XTOUCH_FADER_6_CH, &tiltCourMH});

    surface->addFunctionToTrigger({TRIGGER_FADER_TYPE, XTOUCH_FADER_7_CH, &focusMH});
    surface->addFunctionToTrigger({TRIGGER_FADER_TYPE, XTOUCH_FADER_8_CH, &colorMH});

    surface->addFunctionToTrigger({TRIGGER_FADER_TYPE, XTOUCH_FADER_M_CH, &halogen});
    surface->addFunctionToTrigger({TRIGGER_BUTTON_TYPE, XTOUCH_FLIP, &switchLedLights});
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
    switchLedLightsVar = 0;

    if (globalMode == 0)
    {
        surface->setSegmentsChar({8, 7, 6, 5, 4}, {'d', 'r', 'e', 'a', 'm'});
        setModeLCDs(surface, 0);
        switchLedLights(surface, 127);
    }
    else if (globalMode == 1)
    {
        surface->setSegmentsChar({9, 8, 7, 6, 5, 4, 3}, {'r', 'e', 'a', 'l', 'i', 't', 'y'});
        setModeLCDs(surface, 1);
        switchLedLights(surface, 127);
    }
    else if (globalMode == 2)
    {
        globalActiveThreads.emplace_back(dateTimeThread, surface);
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

    globalActiveThreads.emplace_back(dateTimeThread, &surface);
    surface.startThreads();

    terminatePortMidi();
    std::cout << "Ending mdr\n";
    return 0;
}