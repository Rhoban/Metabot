
module repeat4()
{
    for (r=[0:3])
    rotate([0,0,90*r])
    children();
}
