# Metabot electronics

![Electronics](/docs/imgs/elec.png)

Here are eagle schematics of the XBoard, the electronics inside the Metabot
robot.

## BOM

To assemble the board, you'll need the following components:

Name       | Farnell       | Quantity      | Description
-----------|---------------|---------------|--------------------------------------
MOLEX-53253-0370 | 1756921 | 4 | Connector for the motors
CD74HCT367M | 1739593 | 1 | Bus driver
1x20 0.1" header | 1593469 | 2 | Connector for the maple mini
1x8 0.1" header | 1593463 | 1 | Connector for the IMU
1x10 1mm header | 1109732 | 2 | Connector for the XBee
CEM-1203 | - | 1 | Buzzer
BC817 | 1081223RL | 1 | NPN transistor for the buzzer
EEE1CA470SP | 9696946 | 1 | 47uF decoupling capacitor
EEEFK1C221XP | 1850103 | 1 | 220uF decoupling capacitor
TS2940CW-3.3RP | 7261403RL | 1             | 3.3V Regulator
AP1117E50G-13 | 1825292RL  | 1             | 5V Regulator
LITTELFUSE  56000001009  | 1185363 | 1     | Fuse holder
Radial Fuse, 3.15A  37013150410 | 1716619 | 1 | Fuse
SUD50N04-8M8P-4GE3 | 1794799 | 1     | This big MOS can blow up the fuse programmatically
JST-BEC connector | - | 1 | Connector for the battery
R0805 22 ohms | - | 2 | 
R0805 470 ohms | - | 2 | 
R0805 1 Kohms | - | 1 |
R0805 4 Kohms | - | 3 |
R0805 10 Kohms | - | 1 |
R0805 20 Kohms | - | 1 |
C0805 100nF | - | 1 |

