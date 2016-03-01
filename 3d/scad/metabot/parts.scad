include <config.scad>;
use <../util/screws.scad>;
use <../parts/body.scad>;
use <../parts/body4.scad>;
use <../parts/u.scad>;
use <../parts/side.scad>;
use <../parts/leg.scad>;

module metabot_u(print=false) {
    metabot_colorize()
        u(UHeight, URadius, Thickness, UScrewsSpacing, UScrewsDiameter, print=print);
}

module metabot_body_screws() {
	squareScrews(BodyScrewsW, BodyScrewsH, BodyScrewsSize, Thickness);
}

module metabot_body(print=false, top=false) {
    metabot_colorize()
        body(BodySize, Legs, Thickness, print=print)
		if (top && BodyScrews) {
			metabot_body_screws();
		}
}

module metabot_body4(print=false, type="top") {
    metabot_colorize()
    body4(Thickness, print=print, type=type);
}

module metabot_side(print=false) {
    metabot_colorize()
        side(SideSize, SideHolesToBorder, Thickness, print=print);
}
