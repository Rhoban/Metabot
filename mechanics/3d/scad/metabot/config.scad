// Color of the parts, for rendering
PartsColor = [0.8, 0.8, 0.8];

// Head height
HeadHeight=30;

// Thickness of parts (2.2 is good for Ollo rivets)
Thickness = 2.2;
BodyThickness = 3;

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

module metabot_colorize() {
    color(PartsColor)
        children();
}
