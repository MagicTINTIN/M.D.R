#include <iostream>
#include <cstring>
#include <string>
#include <cmath>
#include <thread>
#include <unistd.h>
#include "portmidi.h"
#include "midi/communication.hh"
#include "midi/controller.hh"
#include "midi/xtouch.hh"
#include "midi/animations.hh"

int globalSpeed = 127;

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
    int previousValue = -1;
    while (1)
    {
        if (globalSpeed > 0)
            surface->animTimeFunctionVector(XTOUCH_ROWS[2], &getAnimSpeed, 0);
        else
            usleep(10000);

        if (globalSpeed != previousValue)
        {
            previousValue = globalSpeed;
            surface->setLight(XTOUCH_DOWN, (globalSpeed > 2) ? 127 : 0);
        }
    }
}

void randomAnim(Controller *surface)
{
    surface->animRandomOfVector({70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90}, -1, 500, 0);
}

int main()
{
    std::cout << "Starting mdr\n";
    std::string surfaceName = "X-Touch X-TOUCH_INT";

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
    surface.addValueToUpdate({TRIGGER_FADER_TYPE, XTOUCH_FADER_1_CH, &globalSpeed});
    surface.addFunctionToTrigger({TRIGGER_FADER_TYPE, XTOUCH_FADER_1_CH, &updateOtherFaders});

    surface.setSegmentsChar({9, 8, 7, 6, 5, 4, 3}, {'b', 'o', 'n', 'j', 'o', 'u', 'r'});
    // surface.analyser(176, 176, 48, 55, 0, 255, 20);
    // initAnim(surface);
    // surface.animLCDRainbow(100, 8, true);
    // surface.setLCDColor(XTOUCH_CHANNELS, XTOUCH_SCREEN_YELLOW);
    // surface.setLCDText(1, 0, "Nice !");
    // surface.setLCDFullLineText(1,"C'est marrant !");
    // surface.setLCDText(7, 1, "Coucou");
    // surface.setLCDText(8, 0, "Yo!", 1);
    surface.setLCDFullLineText(0, "Commandes moteurs principaux");
    surface.setLCDColor({1, 8}, XTOUCH_SCREEN_YELLOW);
    surface.setLCDColor({3, 4, 5, 6}, XTOUCH_SCREEN_WHITE);
    surface.setFader({XTOUCH_FADER_2_CH, XTOUCH_FADER_3_CH, XTOUCH_FADER_4_CH, XTOUCH_FADER_5_CH, XTOUCH_FADER_6_CH, XTOUCH_FADER_7_CH}, 0);
    surface.setFader(XTOUCH_FADER_1_CH, 127);
    surface.setFader(XTOUCH_FADER_8_CH, 69);

    std::thread ylAnimThread(yellowLineAnim, &surface);
    std::thread rndAnimThread(randomAnim, &surface);

    surface.startThreads();

    terminatePortMidi();
    std::cout << "Ending mdr\n";
    return 0;
}