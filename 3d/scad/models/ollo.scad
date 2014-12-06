// Diameter of plastic ollo holes
OlloHoles = 4.3;

// Width of Ollo "real" plastic
OlloWidth = 3;

// Distance between two ollo holes
OlloSpacing = 6;

// Do an Ollo hole on the part
module olloHole(depth=2.2)
{
    translate([0,0,-0.1])
      cylinder(depth+0.2, OlloHoles/2, OlloHoles/2);
}

// Do 3 ollo holes
module threeOllo(depth=2.2)
{
  for (xy=[[-OlloSpacing,0],[0,0],[OlloSpacing,0]]) {
    translate([xy[0], xy[1], 0])
    olloHole(depth);
  }
}

// Do a servo arm holes
module servoArm(depth=2.2)
{
    threeOllo(depth);
    rotate([0,0,90])
      threeOllo(depth);
}
