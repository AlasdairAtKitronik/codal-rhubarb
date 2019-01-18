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

#include "CodalConfig.h"
#include "CircuitPlayground.h"
#include "CodalDmesg.h"
#include "Timer.h"
#include "neopixel.h"

using namespace codal;

#ifdef DEVICE_DBG
RawSerial *SERIAL_DEBUG;
#endif

void cplay_dmesg_flush();
CircuitPlayground* cplay_device_instance = NULL;

/**
  * Constructor.
  *
  * Create a representation of a GenuinoZero device, which includes member variables
  * that represent various device drivers used to control aspects of the micro:bit.
  */
CircuitPlayground::CircuitPlayground() :
    tcTimer(TC4, TC4_IRQn),
    timer(tcTimer),
    messageBus(),
    io(),
    buttonA(io.buttonA, DEVICE_ID_BUTTON_A, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_HIGH, PullMode::Down),
    buttonB(io.buttonB, DEVICE_ID_BUTTON_B, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_HIGH, PullMode::Down),
    buttonC(io.buttonC, DEVICE_ID_BUTTON_C, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW, PullMode::Up),
    buttonAB(DEVICE_ID_BUTTON_A, DEVICE_ID_BUTTON_B, DEVICE_ID_BUTTON_AB),
    i2c(io.sda, io.scl),
    flashSPI(io.flashMOSI, io.flashMISO, io.flashSCLK),
    coordinateSpace(SIMPLE_CARTESIAN, false, COORDINATE_SPACE_ROTATED_0),
    accelerometer(i2c, io.int1, coordinateSpace),
    thermometer(io.temperature, DEVICE_ID_THERMOMETER, 20, 10000, 3380, 10000, 273.5),
    lightSensor(io.light, DEVICE_ID_LIGHT_SENSOR)

    // sws(io.a7),
    // jacdac(sws),
    // protocol(jacdac)
{

    cplay_device_instance = this;
    // Clear our status
    status = 0;

    codal_dmesg_set_flush_fn(cplay_dmesg_flush);

    // Bring up fiber scheduler.
    scheduler_init(messageBus);

    system_timer_calibrate_cycles();

    messageBus.listen(DEVICE_ID_MESSAGE_BUS_LISTENER, DEVICE_EVT_ANY, this, &CircuitPlayground::onListenerRegisteredEvent);

    for(int i = 0; i < DEVICE_COMPONENT_COUNT; i++)
    {
        if(CodalComponent::components[i])
            CodalComponent::components[i]->init();
    }


    //
    // Device specific configuraiton
    //

    // Seed our random number generator
    this->seedRandom(thermometer.getValue() * lightSensor.getValue());

    // light sensor is very stable, so reflect this in the tuning parameters of the driver.
    lightSensor.setSensitivity(912);
    lightSensor.setPeriod(50);

    // clear neopixels
    uint8_t neopixelOff[30];
    memset(neopixelOff, 0x00, sizeof(neopixelOff));
    // for whatever reason to first send doesn't quite work
    neopixel_send_buffer(io.neopixel, neopixelOff, sizeof(neopixelOff));
    target_wait(1);
    neopixel_send_buffer(io.neopixel, neopixelOff, sizeof(neopixelOff));

    flashSPI.setFrequency(4000000);
    flashSPI.setMode(0);
}

/**
  * A listener to perform actions as a result of Message Bus reflection.
  *
  * In some cases we want to perform lazy instantiation of components, such as
  * the compass and the accelerometer, where we only want to add them to the idle
  * fiber when someone has the intention of using these components.
  */
void CircuitPlayground::onListenerRegisteredEvent(Event evt)
{
    switch(evt.value)
    {
        case DEVICE_ID_BUTTON_AB:
            // A user has registered to receive events from the buttonAB multibutton.
            // Disable click events from being generated by ButtonA and ButtonB, and defer the
            // control of this to the multibutton handler.
            //
            // This way, buttons look independent unless a buttonAB is requested, at which
            // point button A+B clicks can be correclty handled without breaking
            // causal ordering.
            buttonA.setEventConfiguration(DEVICE_BUTTON_SIMPLE_EVENTS);
            buttonB.setEventConfiguration(DEVICE_BUTTON_SIMPLE_EVENTS);
            buttonAB.setEventConfiguration(DEVICE_BUTTON_ALL_EVENTS);
            break;

        case DEVICE_ID_ACCELEROMETER:
        case DEVICE_ID_GESTURE:
            // A listener has been registered for the accelerometer.
            // The accelerometer uses lazy instantiation, we just need to read the data once to start it running.
            accelerometer.updateSample();
            break;

        case DEVICE_ID_THERMOMETER:
            // A listener has been registered for the thermometer.
            // The thermometer uses lazy instantiation, we just need to read the data once to start it running.
            thermometer.updateSample();
            break;

        case DEVICE_ID_LIGHT_SENSOR:
            // A listener has been registered for the light sensor.
            // The light sensor uses lazy instantiation, we just need to read the data once to start it running.
            lightSensor.updateSample();
            break;
    }
}

/**
 * A periodic callback invoked by the fiber scheduler idle thread.
* We use this for any low priority, backgrounf housekeeping.
*
*/
void CircuitPlayground::idleCallback()
{
    codal_dmesg_flush();
}

void cplay_dmesg_flush()
{
#if CONFIG_ENABLED(DMESG_SERIAL_DEBUG)
#if DEVICE_DMESG_BUFFER_SIZE > 0

    if (codalLogStore.ptr > 0 && cplay_device_instance)
    {
        for (uint32_t i=0; i<codalLogStore.ptr; i++)
            ((CircuitPlayground *)cplay_device_instance)->serial.putc(codalLogStore.buffer[i]);

        codalLogStore.ptr = 0;
    }
#endif
#endif
}

