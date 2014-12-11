include <config.scad>;
include <../models/raspberry.scad>;
use <../util/screws.scad>;
include <../models/ollo.scad>;
use <../models/motor_arm.scad>;
use <parts.scad>;
use <joints.scad>;

/**
 * Angles 
 */
angles = [0, -30, -110];

module motor_on_body(alpha=0) {
    translate([0, 4*OlloSpacing, MotorDepth/2]) {
        motorArm();
        rotate([0,0,alpha]) {
            children();
        }
    }
}

module metabotLeg(a, b, c) {
	if (MotorsPerLeg == 3) {
    motor_on_body(a) {
        metabot_double_u(b) {
            metabot_side_to_side(c) {
					metabot_arm_leg();
				}
        }
    }	
	}
	if (MotorsPerLeg == 2) {
    motor_on_body(a) {
        metabot_double_u(b) {
				metabot_bottom_leg();
        }
    }	
	}
}

module metabot(angles = [0,0,0]) {
    metabot_body4();
    translate([0,0,MotorDepth+Thickness]) {
        metabot_body4(top=true);
    }

    for (leg=[1:Legs]) {
        rotate([0,0,leg*360/Legs]) {
            translate([0,BodySize/2,Thickness]) {
                metabotLeg(angles[0], angles[1], angles[2]);
            }
        }
    }
}

metabot(angles);

/*
translate([0,0,-8])
rotate([0,180,45])
raspberry();
*/
