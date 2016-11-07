# Building, developing & hacking the firmware

See the [firmware source](src/README.md)

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

