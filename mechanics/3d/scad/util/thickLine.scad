//Draw a line with given width in XY plane with given height
//in Z direction from point 1 to point 2
module thickLine(pt1=[0,0], pt2=[1,0], width=1, height=1) {
        dist = sqrt(
                (pt1[0]-pt2[0])*(pt1[0]-pt2[0])+
                (pt1[1]-pt2[1])*(pt1[1]-pt2[1]));
        translate([pt1[0],pt1[1],0])
        rotate(atan2(pt2[1]-pt1[1], pt2[0]-pt1[0]))
        translate([0,-width/2,0])
        cube([dist, width, height]);
}

