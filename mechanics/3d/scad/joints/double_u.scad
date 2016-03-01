use <../parts/u.scad>;
use <../models/motor_arm.scad>;

module double_u(height=15, radius=8, thickness=2.2, screwsSpacing=10, screwsDiameter=2.6, screws=true, col="white", alpha=0) {
   translate([0,0,0]) {
        color(col)
            u(height, radius, thickness, screwsSpacing, screwsDiameter, screws);
        translate([0,(height+radius)*2,0]) {
            rotate([0,90,180]) {
                color(col)
                    u(height, radius, thickness, screwsSpacing, screwsDiameter, screws);
                rotate([0,180,alpha]) {
                    motorArm();
                    children();
                }
            }
        }
    }
}

double_u();
