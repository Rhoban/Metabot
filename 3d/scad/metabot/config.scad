// Number of legs
Legs = 4;

// Size of the body
BodySize = 28;

// Number of motors per leg (2 or 3)
MotorsPerLeg = 3;

// Static L3 angle (used only if 2 motors per leg)
L3Angle = -5;

// Color of the parts, for rendering
PartsColor = [0.8, 0.8, 0.8];

// Thickness of parts (2.2 is good for Ollo rivets)
Thickness = 2.2;

// Sizes of the parts of the leg
// Ratio of leg parts
LegSize = 100;
LegSizeA = (0.60)*LegSize;
LegSizeB = (0.20)*LegSize;
LegSizeC = (0.20)*LegSize;

// Width of the bottom part of the leg
LegSizeBottom = 10;

// Size of the "top" part of the legs, in mm
LegSizeTop = 30;

// Body screws
BodyScrews = true;
BodyScrewsSize = 3.3;
BodyScrewsW = 7;
BodyScrewsH = 12;

// Motor dimensions
MotorWidth = 24;
MotorHeight = 36;
MotorDepth = 24;
MotorArmOffset = 9;

// U dimensions
UHeight = 15.2;
URadius = 8;
UWidth = 30+Thickness/2;
UTotalHeight = UHeight+URadius;
UScrewsSpacing = 10;
UScrewsDiameter= 3.3;

// Side dimensions
SideSize = 35;
SideHolesToBorder = 5;

// Defining the resolution
$fn=35;
