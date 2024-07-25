For the creation I used an Arduino Mega 2560 board because it has many output pins and is better suited to driving this type of display.
This panel measures 192 by 96. millimetrs and it already has all the drivers on board to drive the LEDs.

I used the Adafruit_GFX.h RGBmatrixPanel.h libraries and changed the character fonts to better fit the display.
REMEMBER TO COPY the Fonts "Ch_12x9.h" and "Ch_5x7.h" in in the folder: \Adafruit_GFX_Library\Fonts of your PC because the fonts I tried, does not fit perfectly.
To edit the fonts you can use the "Adafruit GFX Pixel font customiser" (link):
https://tchapi.github.io/Adafruit-GFX-Font-Customiser/

To receive the satellite signal I used a GPS module from Ublox and the model is the Neo-6M which uses a small antenna and communicates with the card via serial at 9600 baud.

The decoding of the time is done by the Tiny GPS ++ library while for the adaptation to summer or solar time I used the Timezone library by Christensen.
I personally used Central European Time (Frankfurt, Paris, Rome) since I live in Italy.

To change the color of the display I mounted three small buttons connected to three inputs and each of them corresponds to the color red, green and blue.
By pressing them repeatedly you can mix the three colors until you reach the desired color and intensity. 

Then there are key combinations that when pressed together allow you to quickly reach the desired color or turn off the display.

All these color settings are automatically saved in the card's EPROM memory so they are not lost when the display is turned off.

Be careful to power the display at no more than 5 volts and do not use the Arduino board because the regulator on the printed circuit does not have enough current.
I used an external step down regulator so I can use the 12 volt power supply.

Another thing I wanted to say is that for the serial communication between Arduino Mega and the GPS module I used the Serial1  because the use of the software serial of the library involves the interrupt, which in this case,  is better left free for the management of the panel multiplexer.

When you power up the circuit, keep in mind that the GPS module needs a few minutes to synchronize and receive satellites.
