# Wrecker
Mostly 3D printed design for linear drive machine. 3" of travel, compact and economical. 

![image](https://user-images.githubusercontent.com/100043573/154815129-e6593b4b-1dd3-4aac-a349-174e5cd0a6ff.png)

(Safe) Video https://imgur.com/a/8FI21lm

Stainless Steel 1/4-20 rod is used though the load bearing assembly it is quite sturdy.

I apologize to all our metric friends for using imperial hardware, but it's cheap and easy to find here. I may do a version for M6 rod if there is some demand.

# Bill of Materials

## Filament
- PLA, PLA+, or PAL Pro

## Electronics:
- ESP32 Arduino - Hitlego PN ESP-WROOM-32 or eq
- 7x3cm proto PCB perfboard - from Elegoo CA-EL-CP-021 kit or eq
- 24VDC 4A power supply with barrel connector output - ALITOVE ALT-2404 or eq. An old 19.5V laptop supply should work too though a little slower.
- Barrel connector extension cord - must match 24VDC supply
- 24V to Micro USB 5V converter
- NEMA23 56mm Stepper, Bipolar, 2.8A, 2.5mH, 1.8deg - Steppers Online PN 23HS22-2804S or similar
- TB6600 Stepper motor driver
- (2) Hall Effect sensors AH3574-P-A
- 40mm Fan (24V or 5V) - 5V if powering from Arduino (recommended)
- (2) 10K linear potentiometers
- (2) 6" or longer servo extensions

## Hardware
- (1 or 2) 1/4-20 all-threaded Stainless Steel rod, 24" (304 or 18/8)
- 1/4-20 Hex coupler
- (6) 1/4-20 hex nuts
- (4) 1/4-20 washers
- (6) M4x25mm Cheese head
- (4) M4 rubber Grommets or (8) M4 rubber washers
- (4) M4 hex nuts
- (4) M4 lock washers
- (16) M3x8mm flat-head?
- (2) 1/4" dia,  1/16" thick disc magnets

## Other
- Attachment of your choice! End effector files compatible with vacuum-lock attachments and fleshlight are provided.

# Printing

- PLA is the recommended material. Its hard, smooth and abrasion resistant. It is not heat resistant!
- Print the STL files in the orientation provided.
- Files designed for printer line width of 0.5mm and later height of 0.25mm. This works well with 0.4mm nozzle
- 3 walls, 20% 3D infill like cubic

*No supports should be needed*

# Mechanical Assembly


## Preparation

## Stepper Driver setup
- Set current dip switch for 2.5A current limit
- Set dip switch for steps per pulse (micro-steps) to 32

### Base

Tap all the M3 holes in base for cover.
Tap M3 holes for TB6600 mounting.
Tap M3 hole for cable clamp
Press fit 1/4-20 nut into pocket, use 1/4 bolt from below to help seat if needed. If not secure, melt plastic around edge.
Tap the M4 holes for the handle.

### Cover

Attach plug to fan wires (if none), will need mate for attaching to proto board
Mount fan on inside and grill on outside using 4x M3x12mm flathead screws, lockwashers and nuts. Make sure airflow direction is *into* case.

### Slide

Remove and blobs and bumps with sharp knife. Clean out any crud in tube with 1/4" drill.
Put a drop of glue in the pockets, then push magnets into each pocket. Remove any excess glue.

### Pinion

Clean up gear teeth, shaft hole as needed.
Insert M2 hex nut into pocket in shaft, thread M2x4mm grub screw in nut, leave flush.

### Vacuum lock effector

Coat with a couple coats of spray-on acrylic to seal.

## Hall sensors

Cut servo extension leaving about 6 in" on the female end.
Attach wires to the 3 leads of female end, heat shrink. 
- Red: VDD pin
- Black: GND pin
- White: OUT pin

## Assembly

### Servo motor
*if using grommets*:
Place M4 grommets into base holes, and push M4x25mm throuh grommets from slide side to motor side

*if using rubber washers*:
Place M4x25mm through rubber washer, then through base, then add 2nd rubber washer.

Place motor over screw ends, with leads up. Add lock washer and nut to each screw, tighten.

### PCB
Mount 3x7 PCB with ESP32, wiring attached tp 4 mounting bosses with #2x1/4" self-tapping screws.

### 24V to USB converter

Mount to side of vertical fin with M3x8mm screws. Plug micro-usb end into ESP32.

### TB6600
Mount to base holes with M3x8mm cheese head screws.

Cut barrel extension cable near male end and discard male end and strip wire ends.
Twist + wire from barrel connector and + wire from 24V USB converter together and insert into (+) DC input
Twist - wire from barrel connector and - wire from 24V USB converter together and insert into (-) DC input
***OBSERVE POLARITY CAREFULLY***

Configure TB6600 control wiring for common ground (tie - terminal of each input to arduino ground), and the GPIO's from the arduino to the (+) control inputs.

### Slide
Cut 8" piece of 1/4-20 rod, de-burr and reform threads with nut.
*(Note: If you don't care if the machine can be broken down for storage, you can just used a single 16~20" piece)*
Place through tube in slide. 
Add washer to end w/o the nut pocket.
Add nuts to both ends of the rod. Set nut on end with pocket flush with rod.
Tight nut on other end to draw nut into pocket. Torque firmly.

### Hall sensors
Slide hall sensors fully into troughs, angled face up.
Secure with dab of hot glue.

### Vacuum lock effector

Cut 12" length of 1/4-20 SS rod. Deburr rod ends, thread nut over to restore threads.
Place rod through tube. 
Thread hex not on tip end of rod until rod is flush with nut. 
Place washer on other end of rod, then nut. Tighten nut with whrench until top nut is pulled fully into pocket.


## Post-Assembly

Lubricate slide with silicone lubricant oil (not grease).
Lubricate pinion gear with silicone grease.

# Wiring

*add diagram here*

# Arduino
Code provided is written for the ESP32 which has dual 240MHz cores, and can run the stepper at its highest rate easily. It also has Wifi functionality built in. Any Arduino or other controller could probably be used but will not work with the provided code.

I used this stepper lib:

https://github.com/gin66/FastAccelStepper

I used these tutorials to develop the wifi and webserver:

https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/

https://randomnerdtutorials.com/esp32-servo-motor-web-server-arduino-ide/

The randomnerdtutorials.com site has good tuturial how to install the needed components in the IDE. It's a bit complicated.

## Build Setup

## Code

All the sketch code is in the main ino file.

The network.h file has wifi network setup.

## Configuration

# Operation

Attach rod segment with end effector to slide shaft rod using the hex coupler with a additional nut on each side. Tight the nuts to lock in place. A 3D printable finger wrench is included.

Plug in power, wait for Wifi to initialize and/or connect, and the linear drive to auto-home. 
Adjust speed/stroke with sliders as desired.

If no device is connected to the web server, the on-board control knobs remain active. Once a web client connects, they knobs are inactive. If the web client disconnects, the knobs become active again.
...
