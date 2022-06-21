
#ifndef CUBE_H_
#define CUBE_H_

//#if(ARDUINO >= 100)
// #include <Arduino.h>
//#else
// #include <WProgram.h>
//#endif

#include <FastLED.h>


#define SX 4
#define SY 4
#define SZ 4

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

typedef struct {
  float x;
  float y;
  float z;
} point;

struct Point3D {
    uint8_t x;
    uint8_t y;
    uint8_t z;
    Point3D(){}
    Point3D(uint8_t x1, uint8_t y1, uint8_t z1) : x(x1), y(y1), z(z1){}
};
enum {DIRX = 0, DIRY, DIRZ};


class Cube {
  public:
  Cube(CFastLED* f);

  void begin();
  void setVoxel(int x, int y, int z, const CRGB& c);
  void clrVoxel(int x, int y, int z);
  void alterVoxel(int x, int y, int z, const CRGB& c);
  void getVoxel(int x, int y, int z, CRGB& c);
  void shift(uint8_t axis, int direction);
  bool inRange(int x, int y, int z);
  void setPlaneX(uint8_t index, byte* data, int len, const CRGB& c);
  void setPlaneY(uint8_t index, byte* data, int len, const CRGB& c);
  void setPlaneZ(uint8_t index, byte* data, int len, const CRGB& c);
  void clrPlaneX(uint8_t index);
  void clrPlaneY(uint8_t index);
  void clrPlaneZ(uint8_t index);

  void setSquare(uint8_t dir, uint8_t dev, uint8_t x, uint8_t y, uint8_t len, const CRGB& c);
  void clrSquare(uint8_t dir, uint8_t dev, uint8_t x, uint8_t y, uint8_t len);
  void setCube(uint8_t x, uint8_t y, uint8_t z, uint8_t len, const CRGB& c);
  void setCube(const Point3D& head, const Point3D& offset, const CRGB& c);

  void setBrightness(uint8_t pwm);
  void show();
  void clearData();

  private:
  CFastLED* fastLED;
};

class MovingCube
{
  Cube* cube = NULL;
  uint8_t offsetX = 0;
  uint8_t offsetY = 0;
  uint8_t offsetZ = 0;
  bool moveTailX = false;
  bool moveTailY = false;
  bool moveTailZ = false;
  bool actionEnd = true;
  CRGB c;
  uint8_t pos = 0;

  //////////
  int gX = 0;
  int gZ = 0;
  bool ack = false;
  bool ackZ = false;

public:
     MovingCube(Cube* c1, CRGB color) :c(color) {
        cube = c1;
     }
     void moving();
     void handShark(uint8_t dir);
};

#endif /* CUBE_H_ */
