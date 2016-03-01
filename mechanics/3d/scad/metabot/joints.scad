include <config.scad>;
use <../joints/double_u.scad>;
use <../joints/side_to_side.scad>;
use <../parts/leg.scad>;

module metabot_double_u(alpha=0) {
    double_u(UHeight, URadius, Thickness, UScrewsSpacing, UScrewsDiameter, col=PartsColor, alpha=alpha) {
        children();
    }
}

module metabot_side_to_side(alpha=0) {
    side_to_side(SideSize, SideHolesToBorder, Thickness, col=PartsColor, alpha=alpha) {
        children();
    }
}

module metabot_leg(print=false) {
    metabot_colorize()
        rotate([print ? 0 : 90,print ? 0 : 90,0])
        leg(LegSizeA, LegSizeB, LegSizeC, LegSizeBottom,
                LegSizeTop, Thickness, print=print);
}

metabot_leg();
