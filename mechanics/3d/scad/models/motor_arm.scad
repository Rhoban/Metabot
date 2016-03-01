include <motor.scad>;
use <arm.scad>;

module motorArm() {
  motor();
  translate([0,0,MotorDepth/2])
    arm();
  translate([0,0,-MotorDepth/2-OlloWidth])
    arm(true);	
}

motorArm();
