use <../models/ollo.scad>;
use <../util/rounded.scad>;
use <../util/bolt_trace.scad>;

$fn=25;

module body4(size=23, legs=4, thickness=2.2, fixation=true, print=false) {
    margin = 8;
    height = 11;
    R= (print && fixation)? [90,0,0] : [0,0,0];
    rotate(R)
    difference() {
	union() {
	    rounded(size+margin, size+margin, thickness, 10, center=true);
	    if (fixation) {
		translate([0,0,(thickness*2+height)/2])
		cube([size+margin+2*thickness, size+margin, thickness*2+height], center=true);
		translate([0,0,thickness+height+thickness/2])
		difference() {
			union() {
				cube([70,size+margin,thickness], center=true);
				translate([24,-(size+margin)/2,-5-thickness/2])
				cube([11,size+margin,5]);
				translate([-35,-(size+margin)/2,-5-thickness/2])
				cube([11,size+margin,5]);
			}
			translate([30,6,-thickness/2]) {
				cylinder(d=4.3,h=100,center=true);
				translate([0,0,-3])
				rotate([0,0,-90])
				bolt_trace(width=100, length=6, thickness=2.8);
			}
			translate([-30,-6,-thickness/2]) {
				cylinder(d=4.3,h=100,center=true);
				translate([0,0,-3])
				rotate([0,0,90])
				bolt_trace(width=100, length=6, thickness=2.8);
			}
			
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

body4(fixation=true);
