include <../models/ollo.scad>;
use <../util/rounded.scad>;

module side(size=40, holesToBorder=5, thickness=2.2, print=false) {
    if (print) {
        translate([0,-size+holesToBorder,0])
            side(size,holesToBorder,thickness,false);
    } else {
        echo("[PART] side");
        translate([0,size-holesToBorder,0]) {
            difference() {
                rounded(20, size*2, thickness, 5, true);
                for (y=[size-holesToBorder,	 -size+holesToBorder,
                        size-holesToBorder-3*OlloSpacing, -size+holesToBorder+3*OlloSpacing]) {
                    translate([0, y, 0])
                        threeOllo();
                }
            }
        }
    }
}

side();
