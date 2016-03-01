// Do a rounded cube
module rounded(x=10, y=10, z=10, r=3, center=false) {
  if (center) {
    translate([-(x/2),-(y/2),0])
      rounded(x, y, z, r);
  } else {
    translate([r,r,0])
    minkowski() {
      cube([x-2*r,y-2*r,z/2]);
      cylinder(h=z/2, r=r);
    }
  }
}
