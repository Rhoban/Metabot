include <ollo.scad>;

module arm(counter=false) {
    c = counter?[0.6,0.6,0.6]:[0.15,0.15,0.15];
    color(c) {
        difference() {
            cylinder(d=20, h=OlloWidth);
            servoArm(OlloWidth);
        }
    }
}

arm();
