use <../models/ollo.scad>;
use <../util/hexapath.scad>;
use <../util/repeat4.scad>;
use <../util/hole3.scad>;

$fn=25;

module body(thickness=2.2, type="top", print=false) {
    // The layout path
    X=45;
    Y=15;
    
    difference() {
        // Making the exernal path
        hexapath(X, Y, thickness=thickness);
        
        if (type == "top") {
            // Craving the board to make it accessible
            hexapath(X-8, Y-8, thickness=99, center=true);
            
            // Adding holes for the magnets
            repeat4()
            translate([41,0,0])
            hole3();
        } else if (type == "bottom") {
            // Adding holes for the XBoard
            repeat4()
            translate([35, 11.5, 0])
            hole3();
        }
        
        // Motors fixation
        repeat4()
        rotate([0,0,45])
        translate([38,0,0])
        rotate([0,0,90])
        threeOllo();
    }
    
}

body(type="top");
