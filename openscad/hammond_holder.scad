/*
hammond_holder.scad

Copyright (C) 2021
Henrik Björkman
www.eit.se/hb

History
2021-07-14 Created by Henrik



box
https://www.elfa.se/Web/Downloads/aw/_e/bk1591A_draw_e.pdf

The 10 mm screws supplied with box need to be replaces with longer, such as these.
Screws, Cross head M3 16mm, philips galvanic
Elfa 301-72-660

*/

dist_between_screws = 36.2;
length = 10;
height = 4.5;
width = 50.0;
band_height = 2;
screw_diam = 3;
band_width = 27.0;
screw_head_height=1.5;
screw_sink=0.3;

difference()
{
    cube([width, height, length], center=true); 

    for(x=[-dist_between_screws/2, dist_between_screws/2])
    {
        translate([x,0,0])
        rotate([90,0,0])
        cylinder(height+0.001, screw_diam/2+0.3, screw_diam/2+0.3, center=true, $fn=32);

        translate([x,-(height/2-screw_sink-screw_head_height/2),0])
        rotate([90,0,0])
        cylinder(screw_head_height+0.001, screw_diam/2+0.3, screw_diam+0.3, center=true, $fn=32);

        translate([x,-(height/2-screw_sink/2),0])
        rotate([90,0,0])
        cylinder(screw_sink+0.001, screw_diam+0.3, screw_diam+0.3, center=true, $fn=32);

        translate([0,(height-band_height)/2,0])
        cube([band_width, band_height+0.01, length+0.01], center=true); 
    }

}


