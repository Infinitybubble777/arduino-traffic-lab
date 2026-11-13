# Arduino Traffic Lab

A breadboard model of a pedestrian crossing. A debounced button requests a crossing, and a state machine moves the LEDs through each phase without using blocking delays. This is an AI-assisted classroom and portfolio demo for Arduino Uno.

## Wire the model

Connect each LED's anode to its output pin through a 220–330 Ω resistor. Connect each cathode to ground. Connect a normally open pushbutton between pin 2 and ground. The sketch enables the internal pull-up on that pin.

| Component | Uno pin |
| --- | --- |
| Car red LED | D4 |
| Car yellow LED | D5 |
| Car green LED | D6 |
| Pedestrian red LED | D7 |
| Pedestrian green LED | D8 |
| Pushbutton | D2 |

Open `arduino-traffic-lab.ino` in Arduino IDE, select Arduino Uno and the board's port, then upload.

## Try it

1. At startup, the car signal is green and the pedestrian signal is red.
2. Press the button. If car green has lasted less than five seconds, the request waits until the five-second minimum has passed.
3. The car light turns yellow for two seconds. Both signals then show red for one second.
4. The pedestrian green light stays on for five seconds. Both signals then show red for one second before car green returns.
5. Hold the button across a cycle. Release and press it again to request another crossing.

The state machine keeps the two green lights mutually exclusive. All intervals use elapsed time from `millis()`, so the button can be polled throughout the cycle.
