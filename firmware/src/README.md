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

