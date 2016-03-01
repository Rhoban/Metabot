// Follow an octogonal path
module octopath(X, Y, thickness, center=false)
{
    union() {
        linear_extrude(thickness, center=center)
            polygon(points=[
                    [X, Y], [X, -Y], 
                    [Y, -X], [-Y, -X],
                    [-X, -Y], [-X, Y],
                    [-Y, X], [Y, X], 
            ]);
    }
}
