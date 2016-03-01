Debug=false;

module bolt_trace(width=10, length=7, thickness=3)
{
	d = length/(2*cos(30));
		
	if (Debug) {
		color([1,0,0,0.3])
		rotate([0,0,0*60])
		cube([length,length,2],center=true);
	}

	linear_extrude(thickness)
	polygon([
	    [0,length/2],
	    [cos(60)*d, sin(60)*d],
	    [cos(0)*d, sin(0)*d],
	    [cos(-60)*d, sin(-60)*d],
	    [0,-length/2],
	    [-width,-length/2],
	    [-width,length/2]
	]);
}

bolt_trace();