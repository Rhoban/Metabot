# Firmware source

## Building, developing & hacking the firmware

See the [firmware source](src/README.md)

You should install the arm cross-compilation toolchain:

```
sudo apt-get install libstdc++-arm-none-eabi-newlib gcc-arm-none-eabi libnewlib-arm-none-eabi binutils-arm-none-eabi
```

The firmware source is in the `src/` directory. To build it, you'll first have to run the
prepare script to get the Maple framework:

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

## Creating behaviors

We already designed a simple architecture with few examples that can be found in the
firmware in order to create behaviors for the robot.

First, edit `behavior.h` and change the `BEHAVIOR_ENABLE` constant to `1`. Then, edit
`behavior.cpp`, you can read the examples functions and uncomment the one(s) you like
in the `behavior_tick` method.

This will be executed when the robot is started (i.e when the `start` command is sent and
after the motors are enabled). Feel free to edit this to create your own, custom behavior.
