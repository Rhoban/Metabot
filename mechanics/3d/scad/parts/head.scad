use <../models/ollo.scad>;
use <../util/octopath.scad>;
use <../util/repeat4.scad>;
use <../util/hole3.scad>;

$fn=25;

module head(height=25, thickness=2.2, print=false) {
    // The layout path
    X=48;
    Y=18;

    if (print) {
        translate([0,0,height])
        rotate([-180,0,0])
        head(height, thickness, false);
    } else {
        difference() {
            union() {    
                difference() {    
                    octopath(X, Y, thickness=height);
                    
                    // Craving the inside
                    translate([0,0,-0.1])
                    octopath(X-thickness, Y+3, thickness=height-thickness);
                }
        
                // If you don't have spacer, you can uncomment this and use long
                // screws
                /*
                repeat4()
                translate([41,0,0])
                difference() {
                    translate([0,0,10])
                    cylinder(d=6, h=height-10);
                    translate([0,0,-1])
                    cylinder(d=4, h=height-10);
                }
                */
            }
            
                    
            // Holes for magnets
            repeat4()
            translate([41,0,0])
            hole3();
        }
    }
}

head(thickness=2.2);