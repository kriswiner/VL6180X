VL6180X
=======

Proximity sensor using ir laser range finding and ambient light sensing for gesture recognition

Simple sketch for Arduino and Teensy 3.1 for ST Microelectronics' VL6180X proximity range finder and light sensor. The sensor works by measuring the time for emitted ir signals from a laser to return to an ir sensor, providing absolute range from ~ 0 mm to 100 mm or more depending on the ambient lighting. This is most useful for gesture recognition and using touchless gestures to choose different microcontroller functions. This is a basic sketch that just intializes the sensor, reads the identification data, configures the sensor for ranging and then gets continuous range data. There is a lot more development required to perfect gesture recognition but I expect to do so in the near future and will post an updated sketch with this capability here.

The sketch has been upgraded with the capability to get continuous-mode absolute range in mm, continuous-mode absolute ambient light intensity in lux, or both with the interleave mode. For ranges beyond about 150 mm the sensor reports various errors since there is not enough ir light returned to get a range reading without increasing the photon collection time, which is set to 100 ms but can be changed in the initialization part of the sketch. The sensor continues reporting range once the range is reduced. There is very little jitter in the range reported, about 1 or 2 mm. This is a pretty accurate and stable range sensor! I haven't looked at the absolute accuracy yet and I need to add a calibration component to the sketch. The current settings work well for a basic demonstration of the capabilities of this sensor.

And the interleave mode should allow some simple gesture recognition programming, which is next on the list of things to do...
