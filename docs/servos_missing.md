# Servos are missing to scan

[« back to the documentation](index.md)

Some situation may lead your servos to disappear from the scan, here is all the notes about
that.

## Conflicting IDs

If your servo is conflicting with another one, you won't be abloe to ping them all. Just try
to connect it lonely on the bus and ping it again.

## Bad configuration

Don't forget you'll have to configure all your servos, ID 1 included. If you did not,
run:

```
$ dxl_configure 1 1
```

## ID above 20

If you did something wrong, like a typo during the configuration, one of your servo may have
an ID above 20. In this case, you can try to run an extended scan, up to 250:

```
$ dxl_scan 250
```

## Bad power

Powering *all* the servos using USB may lead to problems. You should try connecting all
legs one by one and checking if you can communicate with all servos separately. If you can, and
can't when everything is plugged, consider using an external power supply. You'll need one to
get your robot working anyway

## Check the version

If you're using other servos than XL-320, you may have to give a try to the Dynamixel V1 protocol.

To do that, you can define `DXL_VERSION_1` when compiling the firmware (or uncomment the correct
line in `build-targets.mk`.

## Firmware recovery

If the led of your servo is not blinking when it starts, maybe it's firmware is bricked.

**Warning: this may be dangerous, be really careful doing that, you could definitely brick some servos**

First, you'll need to install the official RoboPlus software (yes, it only works on Windows *sigh*).

Connect the failing servo and **ONLY THIS ONE**, be sure **NO OTHER SERVO** is connected.

Then, install the correct driver to be able to communicate with the board using PuTTy or other software,
then type:

```
$ dxl_forward 57600
```

And close your terminal. Open Dynamixel Wizard and run the Dynamixel firmware recovery. When the search
starts, disconnect and connect your failing servo, something should happpen, select XL-320 in the list
of firmwares and wait.

To check if it works, you'll have to reboot the OpenCM, and then you can try `dxl_scan` command.

### See also

* [Numbering servos](ids.md)
