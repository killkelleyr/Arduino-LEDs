/*
Fast and smooth random colour patterns for a LPD8066 addressable RGB LED strip.
By happyinmotion (jez.weston@yahoo.com)

Simplex noise code taken from Stephen Carmody's Java implementation at:
http://stephencarmody.wikispaces.com/Simplex+Noise

Perlin Noise code copyright 2007 Mike Edwards:
http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1191768812

// Timing results on a 96 LED strip controlled by an Arduino Nano v3:
// Perlin noise - 10 ms per LED per colour
// Simplex noise - 1 ms per LED per colour
// Cubic interpolation - 0.02 ms per LED per colour
//
// Smooth motion needs a refresh every 50 ms or so. For a 96 RGB LED strip, I used 6 simplex nodes with interpolation between to get it all smoothly flowing.
*/


// LPD8806-based code from Adafruit for RGB LED Modules in a strip
/*****************************************************************************/
// you can also use hardware SPI, for ultra fast writes by leaving out the
// data and clock pin arguments. This will 'fix' the pins to the following:
// on Arduino 168/328 thats data = 11, and clock = pin 13.
#include "LPD8806.h"
#include "SPI.h"


// Strip variables:
const int LEDs_in_strip = 50;
const int LEDs_for_simplex = 6;
// Extra fake LED at the end, to avoid fencepost problem.
// It is used by simplex node and interpolation code.
float LED_array_red[LEDs_in_strip+1];
float LED_array_green[LEDs_in_strip+1];
float LED_array_blue[LEDs_in_strip+1];
float LED_array_hue[LEDs_in_strip+1];
float LED_array_brightness[LEDs_in_strip+1];

int node_spacing = LEDs_in_strip / LEDs_for_simplex;
LPD8806 strip = LPD8806(LEDs_in_strip);

// Perlin noise global variables:
float x1,y1,x2,y2;
// Set up Perlin globals:
//persistence affects the degree to which the "finer" noise is seen
float persistence = 0.25;
//octaves are the number of "layers" of noise that get computed
int octaves = 3;
// Simplex noise global variables:
int i, j, k, A[] = {0, 0, 0};
float u, v, w, s;
static float onethird = 0.333333333;
static float onesixth = 0.166666667;
int T[] = {0x15, 0x38, 0x32, 0x2c, 0x0d, 0x13, 0x07, 0x2a};

// Simplex noise parameters:
// Useable values for time increment range from 0.005 (barely perceptible) to 0.2 (irritatingly flickery)
// 0.02 seems ideal for relaxed screensaver
float timeinc = 0.02;
// Useable values for space increment range from 0.8 (LEDS doing different things to their neighbours), to 0.02 (roughly one feature present in 15 LEDs).
// 0.05 seems ideal for relaxed screensaver
float spaceinc = 0.1;
// Simplex noise variables:
// So that subsequent calls to SimplexNoisePattern produce similar outputs, this needs to be outside the scope of loop()
float yoffset = 0.0;
float saturation = 1.0;


void setup()
{
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();

  Serial.begin(9600); 
  Serial.println("Setup done ");
}


void loop() {
  int repeats = 200;

  // Perlin noise for five red LEDs
  timeinc = 0.1;
  for ( int i=0; i<repeats; i++ ) {
    PerlinNoisePattern( 5 );
    yoffset += timeinc; 
  }

  AllOff();
  delay(500);

  // Simplex noise for ten full-colour LEDs
  // No interpolation.
  timeinc = 0.02;
  for ( int i=0; i<repeats; i++ ) {
    SimplexNoisePattern( spaceinc, timeinc, yoffset, 10);
    yoffset += timeinc; 
  }
  AllOff();
  delay(500);

  // Simplex noise for whole strip of 96 LEDs.
  // (Well, it's simplex noise for 6 LEDs and cubic interpolation between those nodes.)
  for ( int i=0; i<repeats; i++ ) {
    SimplexNoisePatternInterpolated( spaceinc, timeinc, yoffset);
    yoffset += timeinc; 
  }
  AllOff();
  delay(500);
}



void PerlinNoisePattern(int numLEDs) {
  float xoffset = 1.0;
  for (int j=0; j < numLEDs; j++) {
    xoffset += spaceinc;
    
    // Caluclate Perlin Noise (range -1 to 1), expand and bias that range:
    // Takes around 10 ms per call, so only doing this for the red LEDs, not full colour
    int r = int(PerlinNoise2(xoffset,yoffset,persistence,octaves)*196+32);        
    // Clip output to the brightness level of 0-127 that the LED strip accepts.
    // Doing this with constrain() is slooow. Doing this explicitly adds no time at all. Conclusion: constrain() sucks.
    if ( r>127 ) { r=127; }
    else if ( r<0 ) { r=0; }

    strip.setPixelColor(j, r, 0, 0);
  }
 
  strip.show();
}



void SimplexNoisePattern( float spaceinc, float timeinc, float yoffset, int numLEDs) {
    // Calculate simplex noise for LEDs that are nodes:
    // Store raw values from simplex function (-0.347 to 0.347)
    float xoffset = 0.0;
    for (int i=0; i<numLEDs; i++) {
      xoffset += spaceinc;
      LED_array_red[i] = SimplexNoise(xoffset,yoffset,0);
      LED_array_green[i] = SimplexNoise(xoffset,yoffset,1);
      LED_array_blue[i] = SimplexNoise(xoffset,yoffset,2);
    }
  
    // Convert values from raw noise to scaled r,g,b and feed to strip.
    // Raw noise is -0.347 to +0.347 or thereabouts.
    for (int i=0; i<numLEDs; i++) {
      int r = int(LED_array_red[i]*403 + 16);
      int g = int(LED_array_green[i]*403 + 16);
      int b = int(LED_array_blue[i]*403 + 16);
      
      if ( r>127 ) { r=127; }
      else if ( r<0 ) { r=0; }  // Adds no time at all. Conclusion: constrain() sucks.
  
      if ( g>127 ) { g=127; }
      else if ( g<0 ) { g=0; }
  
      if ( b>127 ) { b=127; }
      else if ( b<0 ) { b=0; }  
  
      strip.setPixelColor(i, r, g, b);
    }

    // Update strip  
    strip.show();
}



void SimplexNoisePatternInterpolated( float spaceinc, float timeinc, float yoffset) {
    // Calculate simplex noise for LEDs that are nodes:
    // Store raw values from simplex function (-0.347 to 0.347)
    float xoffset = 0.0;
    for (int i=0; i<=LEDs_in_strip; i=i+node_spacing) {
      xoffset += spaceinc;
      LED_array_red[i] = SimplexNoise(xoffset,yoffset,0);
      LED_array_green[i] = SimplexNoise(xoffset,yoffset,1);
      LED_array_blue[i] = SimplexNoise(xoffset,yoffset,2);
    }

    // Interpolate values for LEDs between nodes
    for (int i=0; i<LEDs_in_strip; i++) {
      int position_between_nodes = i % node_spacing;
      int last_node, next_node;

      // If at node, skip
      if ( position_between_nodes == 0 ) {
        // At node for simplex noise, do nothing but update which nodes we are between
        last_node = i;
        next_node = last_node + node_spacing;
      }
      // Else between two nodes, so identify those nodes
      else {
        // And interpolate between the values at those nodes for red, green, and blue
        float t = float(position_between_nodes) / float(node_spacing);
        float t_squaredx3 = 3*t*t;
        float t_cubedx2 = 2*t*t*t;
        LED_array_red[i] = LED_array_red[last_node] * ( t_cubedx2 - t_squaredx3 + 1.0 ) + LED_array_red[next_node] * ( -t_cubedx2 + t_squaredx3 );
        LED_array_green[i] = LED_array_green[last_node] * ( t_cubedx2 - t_squaredx3 + 1.0 ) + LED_array_green[next_node] * ( -t_cubedx2 + t_squaredx3 );
        LED_array_blue[i] = LED_array_blue[last_node] * ( t_cubedx2 - t_squaredx3 + 1.0 ) + LED_array_blue[next_node] * ( -t_cubedx2 + t_squaredx3 );
      }
    }
  
    // Convert values from raw noise to scaled r,g,b and feed to strip
    for (int i=0; i<LEDs_in_strip; i++) {
      int r = int(LED_array_red[i]*403 + 16);
      int g = int(LED_array_green[i]*403 + 16);
      int b = int(LED_array_blue[i]*403 + 16);
      
      if ( r>127 ) { r=127; }
      else if ( r<0 ) { r=0; }  // Adds no time at all. Conclusion: constrain() sucks.
  
      if ( g>127 ) { g=127; }
      else if ( g<0 ) { g=0; }
  
      if ( b>127 ) { b=127; }
      else if ( b<0 ) { b=0; }  
  
      strip.setPixelColor(i, r, g, b);
    }

    // Update strip  
    strip.show();
}



/*****************************************************************************/
// Simplex noise code:
// From an original algorythm by Ken Perlin.
// Returns a value in the range of about [-0.347 .. 0.347]
float SimplexNoise(float x, float y, float z) {
  // Skew input space to relative coordinate in simplex cell
  s = (x + y + z) * onethird;
  i = fastfloor(x+s);
  j = fastfloor(y+s);
  k = fastfloor(z+s);
   
  // Unskew cell origin back to (x, y , z) space
  s = (i + j + k) * onesixth;
  u = x - i + s;
  v = y - j + s;
  w = z - k + s;;
   
  A[0] = A[1] = A[2] = 0;
   
  // For 3D case, the simplex shape is a slightly irregular tetrahedron.
  // Determine which simplex we're in
  int hi = u >= w ? u >= v ? 0 : 1 : v >= w ? 1 : 2;
  int lo = u < w ? u < v ? 0 : 1 : v < w ? 1 : 2;
   
  return k_fn(hi) + k_fn(3 - hi - lo) + k_fn(lo) + k_fn(0);
}


int fastfloor(float n) {
  return n > 0 ? (int) n : (int) n - 1;
}


float k_fn(int a) {
  s = (A[0] + A[1] + A[2]) * onesixth;
  float x = u - A[0] + s;
  float y = v - A[1] + s;
  float z = w - A[2] + s;
  float t = 0.6f - x * x - y * y - z * z;
  int h = shuffle(i + A[0], j + A[1], k + A[2]);
  A[a]++;
  if (t < 0) return 0;
  int b5 = h >> 5 & 1;
  int b4 = h >> 4 & 1;
  int b3 = h >> 3 & 1;
  int b2 = h >> 2 & 1;
  int b = h & 3;
  float p = b == 1 ? x : b == 2 ? y : z;
  float q = b == 1 ? y : b == 2 ? z : x;
  float r = b == 1 ? z : b == 2 ? x : y;
  p = b5 == b3 ? -p : p;
  q = b5 == b4 ? -q: q;
  r = b5 != (b4^b3) ? -r : r;
  t *= t;
  return 8 * t * t * (p + (b == 0 ? q + r : b2 == 0 ? q : r));
}


int shuffle(int i, int j, int k) {
  return b(i, j, k, 0) + b(j, k, i, 1) + b(k, i, j, 2) + b(i, j, k, 3) + b(j, k, i, 4) + b(k, i, j, 5) + b(i, j, k, 6) + b(j, k, i, 7);
}


int b(int i, int j, int k, int B) {
  return T[b(i, B) << 2 | b(j, B) << 1 | b(k, B)];
}


int b(int N, int B) {
  return N >> B & 1;
}


/*****************************************************************************/
// Perlin noise code:
// using the algorithm from http://freespace.banned.net/hugo.elias/models/m_perlin.html
// thanks to hugo elias
float Noise2(float x, float y)
{
  long noise;
  noise = x + y * 57;
  noise = (noise << 13) ^ noise;
  return ( 1.0 - ( long(noise * (noise * noise * 15731L + 789221L) + 1376312589L) & 0x7fffffff) / 1073741824.0);
}

float SmoothNoise2(float x, float y)
{
  float corners, sides, center;
  corners = ( Noise2(x-1, y-1)+Noise2(x+1, y-1)+Noise2(x-1, y+1)+Noise2(x+1, y+1) ) / 16;
  sides   = ( Noise2(x-1, y)  +Noise2(x+1, y)  +Noise2(x, y-1)  +Noise2(x, y+1) ) /  8;
  center  =  Noise2(x, y) / 4;
  return (corners + sides + center);
}

float InterpolatedNoise2(float x, float y)
{
  float v1,v2,v3,v4,i1,i2,fractionX,fractionY;
  long longX,longY;

  longX = long(x);
  fractionX = x - longX;

  longY = long(y);
  fractionY = y - longY;

  v1 = SmoothNoise2(longX, longY);
  v2 = SmoothNoise2(longX + 1, longY);
  v3 = SmoothNoise2(longX, longY + 1);
  v4 = SmoothNoise2(longX + 1, longY + 1);

  i1 = Interpolate(v1 , v2 , fractionX);
  i2 = Interpolate(v3 , v4 , fractionX);

  return(Interpolate(i1 , i2 , fractionY));
}

float Interpolate(float a, float b, float x)
{
  //cosine interpolations
  return(CosineInterpolate(a, b, x));
}

float LinearInterpolate(float a, float b, float x)
{
  return(a*(1-x) + b*x);
}

float CosineInterpolate(float a, float b, float x)
{
  float ft = x * 3.1415927;
  float f = (1 - cos(ft)) * .5;

  return(a*(1-f) + b*f);
}

float PerlinNoise2(float x, float y, float persistance, int octaves)
{
  float frequency, amplitude;
  float total = 0.0;

  for (int i = 0; i <= octaves - 1; i++)
  {
    frequency = pow(2,i);
    amplitude = pow(persistence,i);

    total = total + InterpolatedNoise2(x * frequency, y * frequency) * amplitude;
  }
  return(total);
}



void AllOff() {
  // Reset LED strip
  strip.begin();
  strip.show();
}
