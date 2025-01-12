# Lumina-Secret-Wall
This project is designed for the [CASA0014 Connected Environments](https://github.com/ucl-casa-ce/casa0014) module to build an IoT device to control The [Chrono Lumina](https://github.com/ucl-casa-ce/casa0014/tree/main/chronoLumina) LED lights at CASA, UCL, through an MQTT broker. This Wall is an IoT device that can control the Chrono Lumina via six LDR sensors placed inside six projected hexagons. The nodes on each hexagon correspond to the placement of six LED numbers on Chrono Lumina. By Shedding light on any hexagon's LDR sensor (with any kind of flashlight), six corresponding LEDs will light up five showing the intensity of the flashlight using a gradient of cold to warm colours and one only white indicating a piece of code to decipher. The wall's final code could be deciphered by identifying the coloured LED numbers and putting them together in order. 

<p align="center">
<img src="https://github.com/user-attachments/assets/bad94a49-4777-40f6-bd4e-9a7a83f36f9f" alt="The Fabricated Lumina wall" style="width:50%;">
</p>

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

## The project GuideMap

 The schematic model for devopling the device based on Chrono Lumina can be helpful in understanding the posisioning of the LEDs. Furthermore, it can be used to understand the Arrays defined in the code. To test the code without publishing to the broker, use the latest [Test](https://github.com/tantoon94/Lumina-Secret-Wall/blob/main/Arduino_Sketch/Test_sun_6pm/Test_sun_6pm.ino) version using the Arduino serial terminal to for debugging. 
<p align="center">
<img src="https://github.com/tantoon94/Lumina-Secret-Wall/blob/main/Images/LuminaWallGuide.png" alt="The Lumina Wall Guide" style="width:60%; center">
</p>

## Setup Instructions

### Circuit Connections:
1. **Power Supply:**
   - Connect the `3.3V` pin on the MKR 1010 to the positive rail on the breadboard.
   - Connect the `GND` pin on the MKR 1010 to the negative rail on the breadboard.

2. **LDR and Resistor Setup:**
   - For each LDR, connect one end to the positive rail (`3.3V`).
   - Connect the other end of each LDR to one end of a `10-ohm` resistor.
   - Connect the other end of each `10-ohm` resistor to the negative rail (`GND`).

3. **Analog Input Connections:**
   - Connect the junction between each LDR and its corresponding `10-ohm` resistor to an analog input pin on the MKR 1010 (e.g., `A0`, `A1`, `A2`, etc.).

<p align="center">
  <img src="https://github.com/tantoon94/Lumina-Secret-Wall/blob/main/Images/Crit.jpg" alt="The Fabricated Lumina wall" style="width:20%">
</p>

### Design
 Using the [Model](https://github.com/tantoon94/Lumina-Secret-Wall/tree/main/MODEL) folder you can access the 3D models of the project. 
 The [G-code](https://github.com/tantoon94/Lumina-Secret-Wall/tree/main/MODEL/3Dprint/G-code) for the printing settings are available in the same directory. 

**MQTT Broker and LED Control:**
- Utilized an MQTT broker to publish instructions for controlling the color and on/off status of each LED pixel (12 pixels per LED string).
- Controlled 6 LEDs with one LDR using a matrix array to hold each LED number and their respective pixels.
- Managed simultaneous messages to shared LEDs, prioritizing messages where the threshold was crossed.
- Ensured LEDs turned off when below the threshold.
 
### Limitations 

**Sensor Calibration:**
- Calibrating sensors for varying light conditions was challenging.
- Using a `10Ω` resistor reduced the sensors' sensitivity, helping distinguish between environmental and flashlight light.
- Designed hexagon modular shapes with a three-pointed star crack to minimize environmental light while allowing sufficient flashlight light.
- Aimed to find the perfect balance between too much and too little light.





##  Contact Details

Tina Samie: tina.samie.19@ucl.ac.uk
