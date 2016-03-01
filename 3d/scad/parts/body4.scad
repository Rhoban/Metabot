use <../models/ollo.scad>;
use <../util/rounded.scad>;
use <../util/bolt_trace.scad>;

$fn=25;

module body4(thickness=2.2, type="top", print=false) {
    X=45;
    Y=15;
    
    module hexapath(X, Y)
    {
        union() {
            linear_extrude(thickness)
            polygon(points=[
                [X, Y], [X, -Y], 
                [Y, -X], [-Y, -X],
                [-X, -Y], [-X, Y],
                [-Y, X], [Y, X], 
            ]);
        }
    }
    
    module repeat4()
    {
        for (angle=[0:3])
        rotate([0,0,90*angle])
        children();
    }
    
    module hole3()
    {
        cylinder(d=3.5, h=99, center=true);
    }
    
    difference() {
        hexapath(X, Y);
        
        if (type == "top") {
            hexapath(X-8, Y-8);
            
            repeat4()
            translate([41,0,0])
            hole3();
        } else if (type == "bottom") {
            repeat4()
            translate([35, 11.5, 0])
            hole3();
        }
        repeat4()
        rotate([0,0,45])
        translate([38,0,0])
        rotate([0,0,90])
        threeOllo();
    }
    
}

body4(type="top");
