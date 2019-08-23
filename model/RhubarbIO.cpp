/*
The MIT License (MIT)

Copyright (c) 2017 Lancaster University.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

/**
  * Class definition for Genuino Zero IO.
  * Represents a collection of all I/O pins on the device.
  */

#include "CodalConfig.h"
#include "RhubarbIO.h"

#include "sam.h"

using namespace codal;

/**
  * Constructor.
  *
  * Create a representation of all given I/O pins on the edge connector
  *
  * Accepts a sequence of unique ID's used to distinguish events raised
  * by MicroBitPin instances on the default EventModel.
  */
RhubarbIO::RhubarbIO() :
    a0 (ID_PIN_A0, PIN_PA16, PIN_CAPABILITY_AD),
    a1 (ID_PIN_A1, PIN_PA17, PIN_CAPABILITY_AD),
    a2 (ID_PIN_A2, PIN_PA18, PIN_CAPABILITY_AD),
    a3 (ID_PIN_A3, PIN_PA08, PIN_CAPABILITY_AD),
    a4 (ID_PIN_A4, PIN_PA09, PIN_CAPABILITY_AD),
    a5 (ID_PIN_A5, PIN_PA10, PIN_CAPABILITY_AD),
    a6 (ID_PIN_A6, PIN_PA11, PIN_CAPABILITY_AD),
    led (ID_PIN_LED, PIN_PA22, PIN_CAPABILITY_DIGITAL),
    buttonA(ID_PIN_BUTTONA, PIN_PA23, PIN_CAPABILITY_DIGITAL),
    buttonB(ID_PIN_BUTTONB, PIN_PA03, PIN_CAPABILITY_DIGITAL),
    neopixel(ID_PIN_NEOPIXEL, PIN_PA15, PIN_CAPABILITY_DIGITAL)    
{
}
