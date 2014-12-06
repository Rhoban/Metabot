module screws(count=4, radius=5, size=3.3, thickness=2.2)
{
	for (screw=[1:count]) {
		rotate([0,0,screw*360/count])
		translate([radius,0,-1])
		cylinder(d=size, h=thickness+2);
	}
}

module squareScrews(w=10, h=10, size=3.3, thickness=2.2) {
	for (X=[-w/2, w/2])
	for (Y=[-h/2, h/2])
	translate([X,Y,-1])		
		cylinder(d=size, h=thickness+2);
}