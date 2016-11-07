# Firmware

[« back to the documentation](index.md)

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
sudo python robotis-loader.py /dev/ttyACM0 metabot.bin
```

This should reset the board and then output some progress bar, your firmware is now
on the robot!

### Windows

You'll need python and pyserial. Then, be sure to have the drivers. You can install
simply OpenCM IDE for that.

Then, run:

```
sudo python robotis-loader.py COM4 metabot.bin
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

### Terminal commands

You can type "help" to see all the available commands. You can use for instance `dxl_scan`
to scan for servos, or `dxl_monitor` to monitor the position, temperature and voltage
of all servos.

## Building, developing & hacking the firmware

The firmware source is in the `src/` directory. To build it, you'll first have to run the
prepare script to get the RobotsWar framework:

```
./prepare.sh
```

Then, run `make`:

```
make
```

You can use `make install` to send directly the firmware on the robot:

```
make install
```

This will work exactly like the above `install.sh` script.


