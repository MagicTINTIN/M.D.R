#pragma once

#include <vector>

// group channels

const int XTOUCH_BUTTONS_CH = 144;
const int XTOUCH_ENCODERS_CH = 176;

// values

const int XTOUCH_STATUS_OFF = 0;
const int XTOUCH_STATUS_BLINK = 1;
const int XTOUCH_STATUS_ON = 127;

// buttons
const int XTOUCH_NB_OF_BUTTONS = 102;

// channels buttons
const int XTOUCH_REC1 = 0;
const int XTOUCH_REC2 = 1;
const int XTOUCH_REC3 = 2;
const int XTOUCH_REC4 = 3;
const int XTOUCH_REC5 = 4;
const int XTOUCH_REC6 = 5;
const int XTOUCH_REC7 = 6;
const int XTOUCH_REC8 = 7;
const int XTOUCH_SOLO1 = 8;
const int XTOUCH_SOLO2 = 9;
const int XTOUCH_SOLO3 = 10;
const int XTOUCH_SOLO4 = 11;
const int XTOUCH_SOLO5 = 12;
const int XTOUCH_SOLO6 = 13;
const int XTOUCH_SOLO7 = 14;
const int XTOUCH_SOLO8 = 15;
const int XTOUCH_MUTE1 = 16;
const int XTOUCH_MUTE2 = 17;
const int XTOUCH_MUTE3 = 18;
const int XTOUCH_MUTE4 = 19;
const int XTOUCH_MUTE5 = 20;
const int XTOUCH_MUTE6 = 21;
const int XTOUCH_MUTE7 = 22;
const int XTOUCH_MUTE8 = 23;
const int XTOUCH_SELECT1 = 24;
const int XTOUCH_SELECT2 = 25;
const int XTOUCH_SELECT3 = 26;
const int XTOUCH_SELECT4 = 27;
const int XTOUCH_SELECT5 = 28;
const int XTOUCH_SELECT6 = 29;
const int XTOUCH_SELECT7 = 30;
const int XTOUCH_SELECT8 = 31;
// encoder assign buttons
const int XTOUCH_TRACK = 40;
const int XTOUCH_SEND = 41;
const int XTOUCH_PAN = 42;
const int XTOUCH_PLUGIN = 43;
const int XTOUCH_EQ = 44;
const int XTOUCH_INST = 45;
// faders navigation
const int XTOUCH_PREVFB = 46;
const int XTOUCH_NEXTFB = 47;
const int XTOUCH_PREVCH = 48;
const int XTOUCH_NEXTCH = 49;
// view control buttons
const int XTOUCH_FLIP = 50;
const int XTOUCH_GLOBAL = 51;
const int XTOUCH_DISPLAY = 52;
const int XTOUCH_SMTPEBEATS = 53;
// function buttons
const int XTOUCH_F1 = 54;
const int XTOUCH_F2 = 55;
const int XTOUCH_F3 = 56;
const int XTOUCH_F4 = 57;
const int XTOUCH_F5 = 58;
const int XTOUCH_F6 = 59;
const int XTOUCH_F7 = 60;
const int XTOUCH_F8 = 61;
// software controls
const int XTOUCH_MIDITRACKS = 62;
const int XTOUCH_INPUTS = 63;
const int XTOUCH_AUDIOTRACKS = 64;
const int XTOUCH_AUDIOINST = 65;
const int XTOUCH_AUX = 66;
const int XTOUCH_BUSES = 67;
const int XTOUCH_OUTPUTS = 68;
const int XTOUCH_USER = 69;
// modify
const int XTOUCH_SHIFT = 70;
const int XTOUCH_OPTION = 71;
const int XTOUCH_CONTROL = 72;
const int XTOUCH_ALT = 73;
// automation
const int XTOUCH_READ = 74;
const int XTOUCH_WRITE = 75;
const int XTOUCH_TRIM = 76;
const int XTOUCH_TOUCH = 77;
const int XTOUCH_LATCH = 78;
const int XTOUCH_GROUP = 79;
// utility
const int XTOUCH_SAVE = 80;
const int XTOUCH_UNDO = 81;
const int XTOUCH_CANCEL = 82;
const int XTOUCH_ENTER = 83;
// transport
const int XTOUCH_MARKER = 84;
const int XTOUCH_NUDGE = 85;
const int XTOUCH_CYCLE = 86;
const int XTOUCH_DROP = 87;
const int XTOUCH_REPLACE = 88;
const int XTOUCH_CLICK = 89;
const int XTOUCH_SOLO = 90;
// play pause options
const int XTOUCH_REWIND = 91;
const int XTOUCH_FORWARD = 92;
const int XTOUCH_STOP = 93;
const int XTOUCH_PLAY = 94;
const int XTOUCH_REC = 95;
// directionnal arrows
const int XTOUCH_UP = 96;
const int XTOUCH_DOWN = 97;
const int XTOUCH_LEFT = 98;
const int XTOUCH_RIGHT = 99;
const int XTOUCH_ZOOM = 100;
// other
const int XTOUCH_SCRUB = 101;

const std::vector<int> XTOUCH_RED = {
    //REC
    0, 1, 2, 3, 4, 5, 6, 7,
    //MUTE
    16, 17, 18, 19, 20, 21, 22, 23,
    // write and save
    75, 80,
    // drop and replace
    87, 88,
    // main rec and scrub
    95, 101
};

const std::vector<int> XTOUCH_BLUE = {
    // zoom
    100
};

const std::vector<int> XTOUCH_GREEN = {
    // select
    24, 25, 26, 27, 28, 29, 30, 31,
    // read group undo
    74, 79, 81,
    // marker nudge cycle clic and play
    84, 85, 86, 89
};

const std::vector<int> XTOUCH_YELLOW = {
    // solo
    8, 9, 10, 11, 12, 13, 14, 15,
    // encoder assign
    40, 41, 42, 43, 44, 45,
    // fb and channels
    46, 47, 48, 49,
    // nav
    50, 51,
    // function
    54, 55, 56, 57, 58, 59, 60, 61,
    // tracks control
    62, 63, 64, 65, 66, 67, 68, 69,
    // modify, automation, utility
    70, 71, 72, 73, 76, 77, 78, 82, 83,
    // transport and pause
    90, 91, 92, 93,
    // move
    96, 97, 98, 99
};

const std::vector<std::vector<int>> XTOUCH_COLUMNS = {
    // channels
    {0, 8, 16, 24},
    {1, 9, 17, 25},
    {2, 10, 18, 26},
    {3, 11, 19, 27},
    {4, 12, 20, 28},
    {5, 13, 21, 29},
    {6, 14, 22, 30},
    {7, 15, 23, 31},
    // main
    {40, 50, 51},
    // f1 column
    {42, 62, 54, 70, 72, 84, 91, 46, 48, 98},
    // f2 column
    {44, 63, 55, 71, 73, 85, 92, 47, 49, 96, 100, 97},
    // f3 column
    {41, 64, 56, 74, 77, 86, 99},
    // f4 column
    {43, 65, 57, 75, 78, 87, 93},
    // f5 column
    {45, 66, 58},
    // f6 column
    {67, 59, 76, 79, 88, 94},
    // f7 column
    {68, 60, 80, 82, 89},
    // f8 column
    {69, 61, 81, 83, 90, 95, 101},
};

const std::vector<std::vector<int>> XTOUCH_ROWS = {
    // encoder assign
    {40, 42, 44, 41, 43, 45},
    {0, 1, 2, 3, 4, 5, 6, 7, 51, 62, 63, 64, 65, 66, 67, 68, 69},
    {8, 9, 10, 11, 12, 13, 14, 15},
    {16, 17, 18, 19, 20, 21, 22, 23, 54, 55, 56, 57, 58, 59, 60, 61},
    {24, 25, 26, 27, 28, 29, 30, 31, 50},
    {70, 71, 74, 75, 76, 80, 81},
    {72, 73, 77, 78, 79, 82, 83},
    {84, 85, 86, 87, 88, 89, 90},
    {91, 92, 93, 94, 95},
    {46, 47, 101},
    {48, 49},
    {96},
    {98, 100, 99},
    {97}
};

// faders press button allways 0
const int XTOUCH_FADER_PRESS_1 = 104;
const int XTOUCH_FADER_PRESS_2 = 105;
const int XTOUCH_FADER_PRESS_3 = 106;
const int XTOUCH_FADER_PRESS_4 = 107;
const int XTOUCH_FADER_PRESS_5 = 108;
const int XTOUCH_FADER_PRESS_6 = 109;
const int XTOUCH_FADER_PRESS_7 = 110;
const int XTOUCH_FADER_PRESS_8 = 111;
const int XTOUCH_FADER_PRESS_M = 112;

// button is a weird value 
const int XTOUCH_FADER_1_CH = 224;
const int XTOUCH_FADER_2_CH = 225;
const int XTOUCH_FADER_3_CH = 226;
const int XTOUCH_FADER_4_CH = 227;
const int XTOUCH_FADER_5_CH = 228;
const int XTOUCH_FADER_6_CH = 229;
const int XTOUCH_FADER_7_CH = 230;
const int XTOUCH_FADER_8_CH = 231;
const int XTOUCH_FADER_M_CH = 232;

const std::vector<int> XTOUCH_FADERS = {
    224,225,226,227,228,229,230,231,232
};

// encoders
const int XTOUCH_ENCODER1 = 16;
const int XTOUCH_ENCODER2 = 17;
const int XTOUCH_ENCODER3 = 18;
const int XTOUCH_ENCODER4 = 19;
const int XTOUCH_ENCODER5 = 20;
const int XTOUCH_ENCODER6 = 21;
const int XTOUCH_ENCODER7 = 22;
const int XTOUCH_ENCODER8 = 23;

const std::vector<int> XTOUCH_ENCODERS = {
    16,17,18,19,20,21,22,23
};

const int XTOUCH_ENCODER_PUSH1 = 32;
const int XTOUCH_ENCODER_PUSH2 = 33;
const int XTOUCH_ENCODER_PUSH3 = 34;
const int XTOUCH_ENCODER_PUSH4 = 35;
const int XTOUCH_ENCODER_PUSH5 = 36;
const int XTOUCH_ENCODER_PUSH6 = 37;
const int XTOUCH_ENCODER_PUSH7 = 38;
const int XTOUCH_ENCODER_PUSH8 = 39;

const std::vector<int> XTOUCH_ENCODER_PUSHS = {
    32,33,34,35,36,37,38,39
};

const int XTOUCH_JOG_WHEEL = 60;