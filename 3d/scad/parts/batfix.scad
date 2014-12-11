include <../models/ollo.scad>;
use <../util/rounded.scad>;
use <../util/screws.scad>;

module batfix(thickness=2.2, cavity=13, height=10, screws=3.2,print=false)
{
	shift=9;
	holesSpacingW = OlloSpacing*7;
	holesSpacingH = OlloSpacing*2;
	R=print?[90,0,0]:[0,0,0];
	translate([-shift,0,height-2*thickness])
	rotate(R)
	difference() {
		union() {
			rounded(50, 20, thickness, 2, center=true);
			translate([shift,0,thickness-height/2])
			cube([cavity+2*thickness,20,height], center=true);
		}
		
		for (X=[-holesSpacingW/2, holesSpacingW/2])
		for (Y=[-holesSpacingH/2, holesSpacingH/2])
		translate([X,Y,-1])			
			cylinder(d=screws, h=thickness+2);
		
		translate([shift,0,0.01+2*thickness-height/2])
			cube([cavity,22,height], center=true);
		
		translate([shift,0,thickness-height])
			children();
	}
}

batfix();
