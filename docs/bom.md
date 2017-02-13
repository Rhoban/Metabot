# Bill of material

Metabot V2 bill of materials

## ROBOTIS parts

Name                   | Quantity | Description                  
-----------------------|----------|----------------------------------
XL-320                 | 12       | Robot servo-motors
Short Rivets           | ~300     | Short rivets
Long Rivets            | 48       | Long rivets
Ollo Pulley            | 12       | The pulley for the counter-axis
Ollo Tool              | 1        | A plastic tool that can be used to handle rivets

## Electronics

Name                   | Quantity | Description                  
-----------------------|----------|----------------------------------
XBoard                 | 1        | This is the custom [board](/electronics/) of the robot
Maple Mini             | 1        | A controller featuring a STM32F103CBT6, with sockets
Mini USB cable         | 1        | You'll need it to connect to the Maple Mini
GY-85 IMU              | 1        | 9DOF inertial measurement units (accelerometer, gyroscope and magnetometer)
XBee Bluetooth         | 1        | You can actually choose any bluetooth chip you want
GP2Y0A21YK             | 1        | 10 to 80 cm infrared distance sensor

## Batteries

The XBoard features a JST-BEC female connector, which is compatible with many LiPo battery.
You can actually use a lot of different power sources, but we recommend you the following references:

Name                   | Quantity | Description                  
-----------------------|----------|----------------------------------
1000mAh 2S (7.4) LiPo  | 1        | A battery, be careful about its geometry
IMAX B3 AC charger     | 1        | A cheap and simple to use LiPo charger

## Mechanics

Name                   | Quantity | Description                  
-----------------------|----------|----------------------------------
M4x40                  | 12        | Tie the leg tips and the body
M3x8                   | 7        | 4 for the magnets to the head, 1 for the IMU and 2 for the GP-2
M3x5 (countersunk)     | 4        | Used to tie the magnets to the spacers
M3 nylstop caps        | 16       | We recommend nylstop caps
M4 nylstop caps        | 12       | We recommend nylstop caps
M3 blind cap nut       | 4        | Blind caps are prettiest for the top of the head
Nylon M3 cap           | 17       | 8 for the electronics, 1 for the IMU, 2 for the GP-2, 4 for the body spacers, 2 to replace magnets (to avoid magnetic interference)
M3x12 male spacer      | 1        | For the IMU
M3x15ish male spacer      | 4        | For the electronics
M3x10                  | 20        | 16 for the Us, 4 for the electronics 
Countersunk magnet (10x3mm, 3mm hole) | 6        | To plug and unplug the head
Googly eyes            | 2        | You can paste it on the front on the robot to get it nicer

Note that you'll need allen 2 and 2.5, and a flat plier or an hex wrench for the nylstop caps

## Parts

For mechanical parts, you have choice between 3D and laser. You can either print the
parts yourself or cut it and bend it.

### The 3D option

Name                   | Quantity 
-----------------------|----------
[U](/mechanics/3d/u.stl) | 8      
[side](/mechanics/3d/side.stl) | 8
[leg](/mechanics/3d/leg.stl) | 4
[body_bottom](/mechanics/3d/body_bottom.stl) | 1
[body_top](/mechanics/3d/body_top.stl) | 1
[head](/mechanics/3d/head.stl) | 1

### The laser option

The parts are provided as DXF, the black layer (0) is cutting and the red one (1)
is 1mm engraving. The target material is 3mm thick PMMA (acrylic).

A packet version can also be [found here](/mechanics/laser/metabot.dxf).

Name                   | Quantity | Comment
-----------------------|----------|-------------
[U](/mechanics/laser/parts/u.dxf) | 8 | [Requires bending](/mechanics/laser/doc/u_bending.pdf)
[side](/mechanics/laser/parts/side.dxf) | 8 | 
[leg](/mechanics/laser/parts/leg.dxf) | 4 |
[tip](/mechanics/laser/parts/tip.dxf) | 4 |
[tip_up](/mechanics/laser/parts/tip_up.dxf) | 4 |
[body_bottom](/mechanics/laser/parts/body_bottom.dxf) | 1 | [Requires bending](/mechanics/laser/doc/body_bending.pdf)
[body_top](/mechanics/laser/parts/body_top.dxf) | 1
[head](/mechanics/laser/parts/head.dxf) | 1 | [Required bending](/mechanics/laser/doc/head_bending.pdf)


