
#include <Arduino.h>
#include "Cube.h"
#include "IRremote.h"

Cube cube(&FastLED);

//plane ping-pong effects
uint8_t showCount = 0;
int8_t iCount = 0;
bool pingPongStart = false;
bool initPingPong = false;
uint8_t mask[4];

// cubeJump effects
uint8_t xPos;
uint8_t yPos;
uint8_t zPos;
bool loading;
uint8_t cubeSize = 0;
bool cubeExpanding = true;

// moveing Cube effects
MovingCube mc1(&cube, CRGB::Red);
MovingCube mc2(&cube, CRGB::Green);
MovingCube mc3(&cube, CRGB::Blue);

// rain effects

// Pixel UpDown effects
int iUpDown = 0;
int jUpDown = 0;
int kUpDown = 0;
int upDownLoc = 0;
bool up = false;
bool fillArray[16];
uint16_t mapArray[64];


enum {
    EFFECT_AUTO = 0,
    EFFECT_PLANE_PINGPONG,
    EFFECT_CUBE_JUMP,
    EFFECT_RAIN,
    EFFECT_MOVINE_CUBE,
    EFFECT_PIXEL_UPDOWN,
    EFFECT_MAX,
    EFFECT_UNKNOWN
};

//unsigned long cmdTime = 0;
int8_t currentEffects = -1;
uint8_t effects = EFFECT_AUTO;
uint8_t effectsAuto = 0;
unsigned long changeTime = 0;
uint8_t brightness = 25; // 0 ~ 256
bool start = true;
long long speedup = 0;
#define SPEED 10000;
unsigned long escapeTime = 0;
uint8_t repetTime = 0;
long long delayEffect = 0;
int ishowEffect = 0;
long long autoChangeTime = 0;

// common variable
CRGB cc = CRGB::Green;// current color
CRGB colorArr[8] = { CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Blue, CRGB::Pink, CRGB::Purple, CRGB::White };

int RECV_PIN = 3;


void showEffectAuto();
void cubeJump();
void showPlanePingpong();
void showRain();
void showMovingCube();
void showPixelUpDown();


void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    while (!Serial);

    cube.setBrightness(brightness);
    cube.begin();

    memset(mask, 0x0f, 4);
    IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
}

void loop()
{
    // put your main code here, to run repeatedly:
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.decodedRawData == 3125149440) {  // BUTTON 1
            effectsAuto = EFFECT_PLANE_PINGPONG;
            Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
        }
        else if (IrReceiver.decodedIRData.decodedRawData == 3108437760) {  // BUTTON 2
            effectsAuto = EFFECT_CUBE_JUMP;
            Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
        }
        else if (IrReceiver.decodedIRData.decodedRawData == 3091726080) {  // BUTTON 3
            effectsAuto = EFFECT_RAIN;
            Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
        }
        else if (IrReceiver.decodedIRData.decodedRawData == 3141861120) {  // BUTTON 4
            effectsAuto = EFFECT_MOVINE_CUBE;
            Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
        }
        else if (IrReceiver.decodedIRData.decodedRawData == 3208707840) {  // BUTTON 5
            effectsAuto = EFFECT_PIXEL_UPDOWN;
            Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
        }
        else if (IrReceiver.decodedIRData.decodedRawData == 3810328320) {  // BUTTON OK
            effectsAuto = EFFECT_AUTO;
            Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
        }
        IrReceiver.resume();
    }
    showEffectAuto();

}

//
// effect function
//

void showEffectAuto()
{
    switch (effectsAuto) {
    case EFFECT_PLANE_PINGPONG:
        Serial.println(effectsAuto);
        showPlanePingpong();
        autoChangeTime = 20000000;
        break;
    case EFFECT_CUBE_JUMP:
        Serial.println(effectsAuto);
        cubeJump();
        break;
    case EFFECT_RAIN:
        Serial.println(effectsAuto);
        showRain();
        break;
    case EFFECT_MOVINE_CUBE:
        Serial.println(effectsAuto);
        showMovingCube();
        break;
    case EFFECT_PIXEL_UPDOWN:
        Serial.println(effectsAuto);
        showPixelUpDown();
        autoChangeTime = 60000000;
        break;
    }


    /*// change effects per 20 seconds.
    if ((micros() - changeTime) > autoChangeTime)
    {
        changeTime = micros();
        if (++effectsAuto >= EFFECT_MAX)
            effectsAuto = EFFECT_PLANE_PINGPONG;

        // initial
        if (effectsAuto == EFFECT_PIXEL_UPDOWN)
        {
            iUpDown = 0;
            delayEffect = 0;
        }

        cube.clearData();
        effects = EFFECT_AUTO;
    }*/
}

/////////////////////////////
void cubeJump()
{
    if (!start)
        return;
    // speed not enough
    if ((micros() - escapeTime) < (150000 + speedup))
        return;

    if (loading)
    {
        cube.clearData();
        xPos = random(0, 2) * 3;
        yPos = random(0, 2) * 3;
        zPos = random(0, 2) * 3;
        cubeSize = 4;
        cubeExpanding = false;
        loading = false;
    }

    {
        CRGB c = cc;
        cube.clearData();
        if (xPos == 0 && yPos == 0 && zPos == 0) {
            cube.setCube(xPos, yPos, zPos, cubeSize, c);
        }
        else if (xPos == 3 && yPos == 3 && zPos == 3) {
            cube.setCube(xPos + 1 - cubeSize, yPos + 1 - cubeSize, zPos + 1 - cubeSize, cubeSize, c);
        }
        else if (xPos == 3 && yPos == 0 && zPos == 0) {
            cube.setCube(xPos + 1 - cubeSize, yPos, zPos, cubeSize, c);
        }
        else if (xPos == 0 && yPos == 3 && zPos == 0) {
            cube.setCube(xPos, yPos + 1 - cubeSize, zPos, cubeSize, c);
        }
        else if (xPos == 0 && yPos == 0 && zPos == 3) {
            cube.setCube(xPos, yPos, zPos + 1 - cubeSize, cubeSize, c);
        }
        else if (xPos == 3 && yPos == 3 && zPos == 0) {
            cube.setCube(xPos + 1 - cubeSize, yPos + 1 - cubeSize, zPos, cubeSize, c);
        }
        else if (xPos == 0 && yPos == 3 && zPos == 3) {
            cube.setCube(xPos, yPos + 1 - cubeSize, zPos + 1 - cubeSize, cubeSize, c);
        }
        else if (xPos == 3 && yPos == 0 && zPos == 3) {
            cube.setCube(xPos + 1 - cubeSize, yPos, zPos + 1 - cubeSize, cubeSize, c);
        }
        if (cubeExpanding) {
            cubeSize++;
            if (cubeSize == 4) {
                cc = colorArr[random(0, 6)];
                cubeExpanding = false;
                xPos = random(0, 2) * 3;
                yPos = random(0, 2) * 3;
                zPos = random(0, 2) * 3;
            }
        }
        else {
            cubeSize--;
            if (cubeSize == 1) {
                cc = colorArr[random(0, 6)];
                cubeExpanding = true;
            }
        }
    }

    cube.show();
    escapeTime = micros();
}

void showPlanePingpong()
{
    if (!start)
        return;
    if (repetTime >= 2)
    {
        initPingPong = false;
        pingPongStart = false;
        repetTime = 0;
        return;
    }

    // speed not enough
    if ((micros() - escapeTime) < (150000 + speedup))
        return;

    if (!initPingPong)
    {
        initPingPong = true;
        repetTime = 0;
    }

    cube.clearData();
    if ((showCount + 1) % 4 == 0)
        cc = colorArr[random(0, 6)];

    CRGB c = cc;
    if (showCount <= 7)
    {
        if (showCount == 0)
            iCount = 0;

        cube.setPlaneX(iCount, mask, 4, c);
        if (showCount < 3)
            ++iCount;
        else
            --iCount;
    }
    else if (showCount > 7 && showCount <= 15)
    {
        if (showCount == 8)
            iCount = 0;
        cube.setPlaneY(iCount, mask, 4, c);
        if (showCount < 11)
            ++iCount;
        else
            --iCount;
    }
    else if (showCount > 15 && showCount <= 23)
    {
        if (showCount == 16)
            iCount = 0;
        cube.setPlaneZ(iCount, mask, 4, c);
        if (showCount < 19)
            ++iCount;
        else
            --iCount;
    }
    //cube.writeScreen();

    if (++showCount >= 24)
    {
        showCount = 0;
        repetTime++;
    }

    escapeTime = micros();
}

void showRain()
{
    if (!start)
        return;
    // speed not enough
    if ((micros() - escapeTime) < (350000 + speedup))
        return;

    int rnd_x;
    int rnd_z;
    int rnd_num;

    rnd_num = rand() % 4;
    for (int i = 0; i < rnd_num;i++)
    {
        rnd_x = rand() % 4;
        rnd_z = rand() % 4;
        CRGB c = colorArr[random(0, 6)];

        cube.setVoxel(rnd_x, 3, rnd_z, c);
    }

    cube.show();
    cube.shift(AXIS_Y, -1); // down to top

    escapeTime = micros();
}

void showMovingCube()
{
    if (!start)
        return;
    // speed not enough
    if ((micros() - escapeTime) < (350000 + speedup))
        return;

    cube.clearData();
    mc1.moving();
    mc2.moving();
    mc3.moving();
    cube.show();

    escapeTime = micros();
}

void showPixelUpDown()
{
    if (!start)
        return;

    // speed not enough
    if ((micros() - escapeTime) < (delayEffect + speedup))
        return;

    // Main loop here for i iterations
    int r = 0; // x
    int c = 0; // z
    switch (iUpDown) {
    case 0: {
        cube.clearData();
        delayEffect = 0;
        for (int i = 0;i < 16;i++)
            fillArray[i] = random(2) ? true : false;

        uint8_t y;
        for (int j = 0;j < 16;j++)
        {
            // randon fill top and bottom layer
            r = j / 4;
            c = j - (r * 4);
            CRGB color = colorArr[random(0, 7)];
            y = fillArray[j] ? 3 : 0;
            cube.setVoxel(r, y, c, color);
        }  // end j
        cube.show();
        ++iUpDown;
    }break;
    case 1: {
        // main loop
        // random shift one led between top and bottom plane
        upDownLoc = random(16);
        if (fillArray[upDownLoc] == 0)
            ishowEffect = 1;
        else //3
            ishowEffect = 2;
        delayEffect = 0;
        ++iUpDown;
        ++jUpDown;
    }break;
    case 2: {
        delayEffect = 40000;
        r = upDownLoc / 4;
        c = upDownLoc - (r * 4);
        if (!fillArray[upDownLoc])
        {
            cube.getVoxel(r, ishowEffect - 1, c, cc);
            cube.clrVoxel(r, ishowEffect - 1, c);
            cube.setVoxel(r, ishowEffect, c, cc);
            if (++ishowEffect >= 4)
            {
                fillArray[upDownLoc] = !fillArray[upDownLoc];
                ishowEffect = 0;
                iUpDown = 1;
                ++jUpDown;
            }
        }
        else
        {
            cube.getVoxel(r, ishowEffect + 1, c, cc);
            cube.clrVoxel(r, ishowEffect + 1, c);
            cube.setVoxel(r, ishowEffect, c, cc);
            if (--ishowEffect < 0)
            {
                fillArray[upDownLoc] = !fillArray[upDownLoc];
                ishowEffect = 0;
                iUpDown = 1;
                ++jUpDown;
            }
        }
        cube.show();
        if (jUpDown >= 16) //32 pixel up or dwon times
        {
            delayEffect = 0;
            jUpDown = 0;
            ++iUpDown;
        }
    }break;
    case 3: { // type 2
        cube.clearData();
        for (int i = 0;i < 16;i++)
            mapArray[i] = i;

        uint8_t t1, t2, val;
        for (int i = 0;i < 250;i++)
        {
            t1 = random(16);
            t2 = random(16);
            val = mapArray[t1];
            mapArray[t1] = mapArray[t2];
            mapArray[t2] = val;
        }
        uint8_t y = up ? 3 : 0;
        for (int j = 0;j < 16;j++)
        {
            //fill top or bottom layer
            r = j / 4;
            c = j - (r * 4);
            CRGB color = colorArr[random(0, 7)];
            cube.setVoxel(r, y, c, color);
        }  // end j
        cube.show();
        ++iUpDown;
    }break;
    case 4: {
        // main loop
        // random shift one led from top or bottom plane
        upDownLoc = mapArray[jUpDown++];
        if (!up)
            ishowEffect = 1;
        else
            ishowEffect = 2;
        delayEffect = 0;
        ++iUpDown;
    }break;
    case 5: {
        delayEffect = 40000;
        r = upDownLoc / 4;
        c = upDownLoc - (r * 4);
        if (!up)
        {
            cube.getVoxel(r, ishowEffect - 1, c, cc);
            cube.clrVoxel(r, ishowEffect - 1, c);
            cube.setVoxel(r, ishowEffect, c, cc);
            if (++ishowEffect >= 4)
            {
                ishowEffect = 0;
                iUpDown = 4;
                ++jUpDown;
            }
        }
        else
        {
            cube.getVoxel(r, ishowEffect + 1, c, cc);
            cube.clrVoxel(r, ishowEffect + 1, c);
            cube.setVoxel(r, ishowEffect, c, cc);
            if (--ishowEffect < 0)
            {
                ishowEffect = 0;
                iUpDown = 4;
                ++jUpDown;
            }
        }
        cube.show();
        if (jUpDown >= 16)
        {
            up = !up;
            delayEffect = 0;
            iUpDown = 3;
            jUpDown = 0;
            if (++kUpDown >= 2)// 4 update times
            {
                iUpDown = 6;
                kUpDown = 0;
            }
        }
    }break;
    case 6: { // type 3
        if (!up)
        {
            cube.clearData();
            cube.show();
        }
        for (int i = 0;i < 64;i++)
            mapArray[i] = i;

        uint16_t t1, t2, val;
        for (int i = 0;i < 5000;i++)
        {
            t1 = random(64);
            t2 = random(64);
            val = mapArray[t1];
            mapArray[t1] = mapArray[t2];
            mapArray[t2] = val;
        }
        ++iUpDown;
    }break;
    case 7: {
        // main loop
        upDownLoc = mapArray[jUpDown++];

        uint8_t x = upDownLoc / 16;
        uint8_t y = (upDownLoc % 16) / 4;
        uint8_t z = (upDownLoc % 16) % 4;
        if (!up)
        {
            CRGB color = colorArr[random(0, 7)];
            cube.setVoxel(x, y, z, color);
        }
        else
        {
            cube.clrVoxel(x, y, z);
        }
        cube.show();

        delayEffect = 40000;
        if (jUpDown >= 64)
        {
            up = !up;
            delayEffect = 0;
            jUpDown = 0;
            iUpDown = 6;
            if (++kUpDown >= 2) //4 update times
            {
                kUpDown = 0;
                iUpDown = 0;
            }
        }
    }break;
    }

    escapeTime = micros();
}
