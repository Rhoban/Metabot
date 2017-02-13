# Firmware

The firmware is the code that is loaded on the robot and that generates the
walking gait and talks to the motors. It is currently only adapted for the
original 12-dof Metabot (4 legs, 3 motors per leg).

## Putting the firmware on the robot

### Linux

You need to install the following packets:

```
$ apt-get install python python-serial
```

To put the firmware on the robot, simply cd into the `firmware/` repository and
run the installation script:

```
sudo python robotis-loader.py /dev/ttyACM0 metabot2.bin
```

This should reset the board and then output some progress bar, your firmware is now
on the robot!

### Windows

You'll need python and pyserial. Then, be sure to have the drivers. You can install
simply OpenCM IDE for that.

Then, run:

```
sudo python robotis-loader.py COM4 metabot2.bin
```

Replace COM4 with the right serial port (you can see it in OpenCM ide or in the devices
manager).

## The terminal

### Connecting to the on-board terminal

#### Linux

You can open a serial connection with the robot using `cu` or `screen`. You can use
for instance:

```
you@yourmachine: cu -l /dev/ttyACM0
Connected.

$  
```

Type "~." to quit cu.

#### Windows

On Windows, you can use a serial client such as PuTTy. Just choose "Serial" as connection
mode and enter "COMx", where x is the number of the device.

You can find the number of the device in the devices manager.

## Commands

### Basics

#### `start`: Enable the motors

This will enable the motors. The LEDs will turn green and the torque will rise
slowly until the robot reaches it default position.

#### `stop`: Stop the motors

Turns the LEDs red and shuts the torque down.

#### `beep [freq] [duration]`: Emit a beep

This will emit a beep using the internal buzzer, using `freq` Hz and `duration`
ms.

### Controller

Here are commands that can be used to control the motion of the robot

#### `dx`, `dy`, `turn`: Controls the robot

These are parameters that control the robot speed. Actually, `dx` and `dy`
control the size of the each step (in mm) and `turn` the angle of each step
(in °).

#### `freq`: Steps per second

This is the number of step the robot does on each second.

#### `h`: Robot height

This is the (z, in mm) height of the robot while it walks.

#### `r`: Radius

This is the radius of the legs (in mm), i.e the distance of the legs to the
center of the body while walking.

#### `alt`: Legs altitude

This parameters controls the height the legs are lifted up (in mm) while
walking.

#### `crab`: Crab position

This is an angle (in °) that can be positive or negative and allows to get a
crab-like position while walking.

## Building, developing & hacking the firmware

See the [firmware source README](src#firmware-source)
