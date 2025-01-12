# Lumina-Secret-Wall
This project is designed for the [CASA0014 Connected Environments](https://github.com/ucl-casa-ce/casa0014) module to build an IoT device to control The [Chrono Lumina](https://github.com/ucl-casa-ce/casa0014/tree/main/chronoLumina) LED lights at CASA, UCL, through an MQTT broker. This Wall is an IoT device that can control the Chrono Lumina via six LDR sensors placed inside six projected hexagons. The nodes on each hexagon correspond to the placement of six LED numbers on Chrono Lumina. By Shedding light on any hexagon's LDR sensor (with any kind of flashlight), six corresponding LEDs will light up five showing the intensity of the flashlight using a gradient of cold to warm colours and one only white indicating a piece of code to decipher. The wall's final code could be deciphered by identifying the coloured LED numbers and putting them together in order. 


![TheFabricated Lumina wall](https://github.com/user-attachments/assets/bad94a49-4777-40f6-bd4e-9a7a83f36f9f)

## Components

1. MCU Board (Arduino MKR 1010)
2. Sensors (LDR MJ5549)
7. Resistors (1kΩ for controlling the LDR sensitivity)
8. Breadboard (for prototyping)
9. Jumper Wires (male-to-male and male-to-female)
10 Power Supply (5V power supply is sufficient, Barrel Jack)

## Libraries
Make sure to include the following libraries in your Arduino IDE:

1. WiFiNINA
2. PubSubClient

## Setup Instructions

* Connect the LDR sensors to the Arduino MKR 1010: A0,
* Connect the linear servo motor to the Arduino: Pin 9
* Connect the rotary servo motor to the Arduino: Pin 10
* Connect the NeoPixel LED strip to the Arduino: Pin 5
* Connect the push button to the Arduino: Pin 2 (Use a pull-up resistor (10kΩ) between pin 2 and 5V)
* Upload the provided Arduino code to your board.

## The Design 

The device works by pressing an illuminated push button. while pushed, a green LED light turns on indicating the active recording. Meanwhile, the disc begins to rotate steadily via a continuous 360-degree servo and the microphone will input the recorded sound as a value between 500-900 (out of a possible 0-1023) to the Arduino. After processing the sound and extracting the amplitude value, it will be mapped to an angle of 0-180 that will drive a 180-degree servo attached to a linear gear holding the pen making it move linearly drawing on the rotary canvas. When the button is released, the LED light will go off and so will the servos and the result of the recorded sound will be a circular shape drawn on a removable disc. 

## Hindrances & Issues

1. The noise the servos produce could affect the microphone's input. 
2. The pin can record only one feature of the sound (amplitude) and not different frequency levels.
3. The design of the pin holder is not flexible making it difficult to remove and replace the disc.
4. The screw for the lid of the disc holder base does not entirely fit when printed out. 

## Further Developments
How can the servo noise issues be fixed? 
  - You can adjust the sensitivity of the microphone by turning the small screw located in the back of the sensor counterclockwise.
  - You can use capacitors in parallel to the microphone to smooth out the noise
  - Alternatively, you can adjust the threshold of the microphone's input in the Arduino code
What are the features that could be improved? How can they be improved?
  - The location of the microphone in the design is not ideal for recording sound. It could be placed in an isolated compartment on top of the device. Alternatively, the microphone could be separated entirely from the device and connected to it through a wireless network connection. 
What are the missing features?
  - The current version of this device only records one parameter of sound, the amplitude. Using multiple pins capturing different frequencies can help develop the device further in future versions.


##  Contact Details

Tina Samie: tina.samie.19@ucl.ac.uk
