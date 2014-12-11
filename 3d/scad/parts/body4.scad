use <../models/ollo.scad>;
use <../util/rounded.scad>;

$fn=25;

module body4(size=23, legs=4, thickness=2.2, fixation=true) {
    margin = 8;
    height = 5;
    difference() {
	union() {
	    translate([0,0,thickness/2])
	    cube([size+margin, size+margin, thickness], center=true);
	    if (fixation) {
		translate([0,0,(thickness*2+height)/2])
		cube([size+margin+2*thickness, size+margin, thickness*2+height], center=true);
		translate([0,0,thickness+height+thickness/2])
		difference() {
			cube([68,size+margin,thickness], center=true);
			translate([30,3,-thickness/2])
			olloHole();
			translate([-30,-3,-thickness/2])
			olloHole();
			
		}
	    }
	}
	union() {
		if (fixation) {
			translate([0,0,(thickness*2+height)/2+thickness/2])
			cube([size+margin+1, size+margin+1, height+thickness+0.1], center=true);
		}
		for (leg=[1:legs]) {
			rotate([0,0,leg*360/legs])
			translate([0,size/2,0])
			threeOllo();
		}
	}
    }
}

body4();