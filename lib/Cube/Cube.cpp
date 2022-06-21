#include "Cube.h"

#define NUM_LEDS 64
#define CTRL_IN  2   // default gpio pin

CRGB leds[NUM_LEDS];
CRGB cArray[8] = {CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Blue, CRGB::Pink, CRGB::Purple, CRGB::White};

//////////////////////////////////////////////////////////////////////////

Cube::Cube(CFastLED* f):fastLED(f)
{
    //NEOPIXEL
    fastLED->addLeds<WS2812B, CTRL_IN>(leds, NUM_LEDS);
    //fastLED->setBrightness(25);
    clearData();
}

void Cube::begin()
{
    fastLED->show();
}

bool Cube::inRange(int x, int y, int z)
{
    if (x >= 0 && x < SX && y >= 0 && y < SY && z >= 0 && z < SZ)
        return true;
    else
        return false;
}

void Cube::setVoxel(int x, int y, int z, const CRGB& c)
{
    if (!inRange(x,y,z))
        return;

    // 3d to 1d
    // x*4*4+y*4+z
    // x,y座標計算，從二為換算成一維
    int loc = x*SX*SY+z*SZ+y;
    //leds[loc] = c;

    //
    // mix color
    //
    CRGB mix;
    uint16_t value = leds[loc].r + c.r;
    mix.r =(value >= 255) ? 255 : value;
    value = leds[loc].g + c.g;
    mix.g =(value >= 255) ? 255 : value;
    value = leds[loc].b + c.b;
    mix.b =(value >= 255) ? 255 : value;
    leds[loc] = mix;
}

void Cube::clrVoxel(int x, int y, int z)
{
    if (!inRange(x,y,z))
        return;

    int loc = x*SX*SY+z*SZ+y;
    leds[loc] = CRGB::Black;
}

// Get the current status of a voxel
void Cube::getVoxel(int x, int y, int z, CRGB& c)
{
    if (!inRange(x,y,z))
        return;

    int loc = x*SX*SY+z*SZ+y;
    c = leds[loc];
}

void Cube::alterVoxel(int x, int y, int z, const CRGB& c)
{
  if (c.r != 0 || c.g != 0 || c.b != 0)
      setVoxel(x,y,z,c);
  else
      clrVoxel(x,y,z);
}

void Cube::shift(uint8_t axis, int direction)
{
    int i, x ,y;
    int ii, iii;
    int state;
    CRGB c;

    for (i = 0; i < SZ; i++)
    {
        if (direction == -1)
            ii = i;
        else
            ii = (3-i);

        for (x = 0; x < SX; x++)
        {
            for (y = 0; y < SY; y++)
            {
                if (direction == -1)
                    iii = ii+1;
                else
                    iii = ii-1;

                if (axis == AXIS_Z)
                {
                    getVoxel(x,y,iii,c);
                    alterVoxel(x,y,ii,c);
                }
                
                if (axis == AXIS_Y)
                {
                    getVoxel(x,iii,y,c);
                    alterVoxel(x,ii,y,c);
                }
                
                if (axis == AXIS_X)
                {
                    getVoxel(iii,y,x,c);
                    alterVoxel(ii,x,y,c);
                }
            }
        }
    }

    if (direction == -1)
        i = 3;
    else
        i = 0;

    for (x = 0; x < SX; x++)
    {
        for (y = 0; y < SY; y++)
        {
          if (axis == AXIS_Z)
              clrVoxel(x,y,i);
          else if (axis == AXIS_Y)
              clrVoxel(x,i,y);
          else if (axis == AXIS_X)
              clrVoxel(i,y,x);
        }
    }
}

void Cube::setPlaneX(uint8_t index, byte* data, int len, const CRGB& c)
{
    if(index >= SX)
        return;

    int x = index;
    for(int y = 0; y < SY; y++)
    {
        byte b = data[y];
        for(int z = 0; z < SZ; z++)
        {
            setVoxel(x, y, z, ((b>>z)&0x01) ? c : CRGB::Black);
        }
    }
    fastLED->show();
}

void Cube::setPlaneY(uint8_t index, byte* data, int len, const CRGB& c)
{
    if(index >= SY)
        return;

    int y = index;
    for(int x = 0; x < SX; x++)
    {
        byte b = data[x];
        for(int z = 0; z < SZ; z++)
        {
            setVoxel(x, y, z, ((b>>z)&0x01) ? c : CRGB::Black);
        }
    }
    fastLED->show();
}

void Cube::setPlaneZ(uint8_t index, byte* data, int len, const CRGB& c)
{
    if(index >= SZ)
        return;

    int z = index;
    for(int x = 0; x < SX; x++)
    {
        byte b = data[x];
        for(int y = 0; y < SY; y++)
        {
            setVoxel(x, y, z, ((b>>y)&0x01) ? c : CRGB::Black);
        }
    }
    fastLED->show();
}


void Cube::clrPlaneX(uint8_t index)
{
    if(index >= SX)
        return;

    int x = index;
    for(int y = 0; y < SY; y++)
    {
        for(int z = 0; z < SZ; z++)
        {
            setVoxel(x, y, z, CRGB::Black);
        }
    }
    fastLED->show();
}

void Cube::clrPlaneY(uint8_t index)
{
    if(index >= SY)
        return;

    int y = index;
    for(int x = 0; x < SX; x++)
    {
        for(int z = 0; z < SZ; z++)
        {
            setVoxel(x, y, z, CRGB::Black);
        }
    }
    fastLED->show();
}

void Cube::clrPlaneZ(uint8_t index)
{
    if(index >= SZ)
        return;

    int z = index;
    for(int x = 0; x < SX; x++)
    {
        for(int y = 0; y < SY; y++)
        {
            setVoxel(x, y, z, CRGB::Black);
        }
    }
    fastLED->show();
}

void Cube::setSquare(uint8_t dir, uint8_t dev, uint8_t x, uint8_t y, uint8_t len, const CRGB& c)
{
    // base on plane(max size:8)
    for (uint8_t i = 0; i < len; i++)
    {
        if(dir == DIRX)
        {
            setVoxel(dev, x, y + i, c);
            setVoxel(dev, x + i, y, c);

            setVoxel(dev, x + len - 1, y + i, c);
            setVoxel(dev, x + i, y + len - 1, c);
        }
        else if(dir == DIRY)
        {
            setVoxel(x, dev, y + i, c);
            setVoxel(x + i, dev, y, c);

            setVoxel(x + len - 1, dev, y + i, c);
            setVoxel(x + i, dev, y + len - 1, c);
        }
        else if(dir == DIRZ)
        {
            setVoxel(x, y + i, dev, c);
            setVoxel(x + i, y, dev, c);

            setVoxel(x + len - 1, y + i, dev, c);
            setVoxel(x + i, y + len - 1, dev, c);
        }
    }
}

void Cube::clrSquare(uint8_t dir, uint8_t dev, uint8_t x, uint8_t y, uint8_t len)
{
    // base on plane(max size:8)
    for (uint8_t i = 0; i < len; i++)
    {
        if(dir == DIRX)
        {
            clrVoxel(dev, x, y + i);
            clrVoxel(dev, x + i, y);

            clrVoxel(dev, x + len - 1, y + i);
            clrVoxel(dev, x + i, y + len - 1);
        }
        else if(dir == DIRY)
        {
            clrVoxel(x, dev, y + i);
            clrVoxel(x + i, dev, y);

            clrVoxel(x + len - 1, dev, y + i);
            clrVoxel(x + i, dev, y + len - 1);
        }
        else if(dir == DIRZ)
        {
            clrVoxel(x, y + i, dev);
            clrVoxel(x + i, y, dev);

            clrVoxel(x + len - 1, y + i, dev);
            clrVoxel(x + i, y + len - 1, dev);
        }
    }
}

void Cube::setCube(uint8_t x, uint8_t y, uint8_t z, uint8_t len, const CRGB& c)
{
    // empty cube
//    CRGB c = cArray[random(0, 6)];
    for (uint8_t i = 0; i < len; i++)
    {
        setVoxel(x, y + i, z, c);
        setVoxel(x + i, y, z, c);
        setVoxel(x, y, z + i, c);
        setVoxel(x + len - 1, y + i, z + len - 1, c);
        setVoxel(x + i, y + len - 1, z + len - 1, c);
        setVoxel(x + len - 1, y + len - 1, z + i, c);
        setVoxel(x + len - 1, y + i, z, c);
        setVoxel(x, y + i, z + len - 1, c);
        setVoxel(x + i, y + len - 1, z, c);
        setVoxel(x + i, y, z + len - 1, c);
        setVoxel(x + len - 1, y, z + i, c);
        setVoxel(x, y + len - 1, z + i, c);
    }
}

void Cube::setCube(const Point3D& head, const Point3D& offset, const CRGB& c)
{
    // full cube
    uint8_t len = 2;
    for(uint8_t x = head.x; x < head.x+len; x ++)
    {
        for(uint8_t y = head.y; y < head.y+len; y ++)
        {
            for(uint8_t z = head.z; z < head.z+len; z ++)
            {
                setVoxel(x+offset.x, y+offset.y, z+offset.z, c);
            }
        }
    }
}

void Cube::show()
{
    fastLED->show();
}

void Cube::clearData()
{
    for(int i = 0; i < NUM_LEDS; i++)
        leds[i] = CRGB::Black;
}

void Cube::setBrightness(uint8_t pwm)
{
    fastLED->setBrightness(pwm);
    fastLED->show();
}

//////////////
void MovingCube::moving()
{
    Point3D head(0,0,0);

    Point3D offset(offsetX,offsetY,offsetZ);
    cube->setCube(head, offset, c);
    //cube->show();

    if(actionEnd)
    {
        pos = rand()%3;
        actionEnd = false;
    }

    switch(pos) {
    case 0:{
        if(!moveTailX)
        {
            if(++offsetX >= 2)
            {
                moveTailX = true;
                actionEnd = true;
            }
        }
        else
        {
            if(--offsetX <= 0)
            {
                moveTailX = false;
                actionEnd = true;
            }
        }
    }break;
    case 1:{
        if(!moveTailY)
        {
            if(++offsetY >= 2)
            {
                moveTailY = true;
                actionEnd = true;
            }
        }
        else
        {
            if(--offsetY <= 0)
            {
                moveTailY = false;
                actionEnd = true;
            }
        }
    }break;
    case 2:{
        if(!moveTailZ)
        {
            if(++offsetZ >= 2)
            {
                moveTailZ = true;
                actionEnd = true;
            }
        }
        else
        {
            if(--offsetZ <= 0)
            {
                moveTailZ = false;
                actionEnd = true;
            }
        }
    }break;
    }
}

void MovingCube::handShark(uint8_t dir)
{
//    for(int z = 0; z < SZ; z ++)
    {
//        for(int x = 0; x < SX; x ++)
        {
            for(int y = 0; y < SY; y ++)
            {
                if(dir == 0)
                    cube->setVoxel(gX,y,gZ,c);
                else if(dir == 1)
                    cube->setVoxel(y,gX,gZ,c);
                else if(dir == 2)
                    cube->setVoxel(gZ,y,gX,c);
            }

            cube->show();

            if(!ack)
              gX++;
            else
              gX--;
        }

        if(gX >= 4 || gX < 0)
        {
            ack = !ack;
            if(!ackZ)
              gZ++;
            else
              gZ--;

            if(gZ >= 3 || gZ <= 0)
            {
                ackZ = !ackZ;
                c = cArray[random(0, 7)];
            }
        }
    }
}
