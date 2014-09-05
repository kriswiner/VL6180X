VL6180X
=======

Proximity sensor using ir laser range finding

Simple sketch for Arduino and Teensy 3.1 for ST Microelectronics' VL6180X proximity range finder and light sensor. The sensor works by measuring the time for emitted ir signals from a laser to return to an ir sensor, providing absolute range from ~ 0 mm to 100 mm or more depending on the ambient lighting. This is most useful for gesture recognition and using touchless gestures to choose different microcontroller functions. This is a basic sketch that just intializes the sensor, reads the identification data, configures the sensor for ranging and then gets continuous range data. There is a lot more development required to perfect esture recognition but I expect to do so in the near future and will post an updated sketch with this capability here.
