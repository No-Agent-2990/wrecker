# Wrecker
Mostly 3D printed design for linear drive machine. 4" of travel, compact and economical. 

![image](https://user-images.githubusercontent.com/100043573/154815129-e6593b4b-1dd3-4aac-a349-174e5cd0a6ff.png)

# Bill of Materials

## Filament
- PLA, PLA+, or PAL Pro

## Electronics:
- ESP32 Arduino - Hitlego PN ESP-WROOM-32 or eq
- 70x30mm proto PCB perfboard - from Elegoo CA-EL-CP-021 kit or eq
- 24VDC 4A power supply with barrel connector output - ALITOVE ALT-2404 or eq
- Barrel connector extension cord - must match 24VDC supply
- 24V to Micro USB 5V converter
- NEMA23 56mm Stepper, Bipolar, 2.8A, 2.5mH, 1.8deg - Steppers Online PN 23HS22-2804S or similar
- TB6600 Stepper motor driver
- (2) Hall Effect sensors AH3574-P-A
- 40mm Fan (24V or 5V) - 5V if powering from Arduino (recommended)

## Hardware
- (2) 1/4-20 all-threaded Stainless Steel rod, 24" (304 or 18/8)
- 1/4-20 Hex coupler
- (6) 1/4-20 hex nuts
- (4) 1/4-20 washers
- (6) M4x25mm Cheese head
- (4) M4 rubber Grommets or (8) M4 rubber washers
- (4) M4 hex nuts
- (4) M4 Spring washers
- (16) M3x8mm flat-head?

## Other
- Attachment of your choice! End effector files compatible with vacuum-lock attachments and fleshlight are provided.

# Printing

- PLA is the recommended material. Its hard, smooth and abrasion resistant. It is not heat resistant!
- Print the STL files in the orientation provided.
- Files designed for printer line width of 0.5mm and later height of 0.25mm. This works well with 0.4mm nozzle
- 3 walls, 20% 3D infill like cubic

*No support should be needed*


# Mechanical Assembly

## Stepper Driver setup
- Set current dip switch for 2.5A current limit
- Set dip switch for steps per pulse (micro-steps) to 32

## Preparation

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

### Pinion

Clean up gear teeth, shaft hole as needed.
Insert M2 hex nut into pocket in shaft, thread M2x4mm grub screw in nut, leave flush.

## Assembly

## Post-Assembly

Lubricate slide with silicone lubricant oil (not grease).
Lubricate pinion gear with silicone grease.

# Wiring

# Arduino
Code provided is written for the ESP32 which has dual 240MHz cores, and can run the stepper at its highest rate easily. It also has Wifi functionality built in.

Any Arduino or other controller could probably be used but will not work with the provided code.

## Build Setup

## Code

## Configuration

# Operation

Attach rod segment with end effector to slide shaft rod using the hex coupler with a additional nut on each side. Tight the nuts to lock in place. A 3D printable palm wrench is included.

Plug in power, wait for Wifi to initialize and/or connect, and the linear drive to auto-home. 
...
