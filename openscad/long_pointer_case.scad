/*
long_pointer_case.scad

Copyright (C) 2021
Henrik Björkman
www.eit.se/hb

History
2021-07-14 Created by Henrik

Screws: Distrelec 300-72-069
Bolts: Distrelec 148-00-070
*/

// Options for compability with older version
// Increase this when doing non backward compatible changes.
major_version = 2;


handle_tilt = 4;
handle_x= -92;
handle_extra_length = 8;

pcb_length = 85;
pcb_width = 56;
pcb_thickness = 1.6;
pcb_margin = 0.2;
components_height = 24;
smd_height = 3;
micro_usb_width = 8;
micro_usb_height = 3;
micro_usb_extrusion = 1.5;
micro_usb_length = 5;
micro_usb_pos = 1;  // Use -1 id under PCB, 1 if above, in long term it shall be under.
components_from_edge = 2;
pcb_box_z = -20;
pcb_z = -components_height/2 + smd_height/2;
pcb_x = 0;

battery_length=71; // was 70 in first version (that was a little tight)
battery_diameter=19.5; // was 19 in first version (that was a little tight)
battery_z = -25;
battery_x=handle_x-25;


speaker_diameter=40.5;
speaker_rim_thickness=2;
speaker_thickness=5;
speaker_x=65;
speaker_z=-20;

lense_diameter=40;
lense_thickness=2;
lense_focal=190;
radius_at_diode=(lense_diameter-2)/8;
radius_at_lense=(lense_diameter-2)/2;
radius_after_lense=(lense_diameter-1)/2;

diode_diameter=3;
diode_length=4;
diode_z = 22;
diode_x = -120;

button_rim_diameter = 14;
button_diameter = 12;
button_length = 22;
button_red_diameter = 9;
button_x = handle_x + 12+1;
button_z = -10;
button_nut_diameter = 17;
button_to_nut = 5;
button_nut_length = 5;

laser_diameter = 10.5;
laser_length = 30;


ir_barrel_z = diode_z;
ir_barrel_length = 220;
ir_barrel_x = diode_x;
ir_barrel_wide_length=135;


bolt_diam=3;
screew_mount_opening = bolt_diam+0.2;
nut_diam=2*bolt_diam+0.9;
screew_mount_height = bolt_diam*2 + 3;
screew_mount_width = screew_mount_height+3;


printer_cut_x = -25; // Give 999 here if no cut is needed


barrel_top_rear_screw_x = ir_barrel_x+ir_barrel_length*0.05;
barrel_top_front_screw_x = ir_barrel_x+ir_barrel_length*0.95;
//screw_under_front_PCB_box_x = printer_cut_x+6;
screw_under_front_PCB_box_x = printer_cut_x+16;
screws_under_PCB_box_z = -31.5;

// Size of a bar to be inserted to prevent IR light to leak out.
bar_length=78;
bar_width=5;
bar_height=3;

mounting_pin_length=22;
mounting_pin_width=3.6;

// If printing with an opaque material some extra openings on PCB 
// box is needed for IR and LED light in/out.
// Use zero here if printing with transparent filament.
// Use 4 if printing with black filament.
opaque_material_window_radius = 4;
 
// Something to put screws in.  
// mode 0 for the box/mount in which the screw/bolt hole is made
// mode 1 to make the hole.
module screw_mount(mode, length)
{
    //cylinder(200,20/2,20/2,center=true);       
   
    if (mode==0)
    {
        // The box in which the screw holes can be made.
        hull()
        {
            cube([screew_mount_width,screew_mount_height,length-1], center=true);        
            cube([screew_mount_width-1,screew_mount_height-1,length], center=true);        
        }
    }
    else
    {
        // The hole for the threaded part of screw.
        cube([screew_mount_opening,screew_mount_opening, length*2], center=true);        
        
        // The hole for the screwhead or nut.

        material = 2.5;//length <= 10 ? 2 : 3;
        nut_opening_length=length;
        for(i=[-1, 1])
        {
            translate([0,0,i*(nut_opening_length/2+material)])
            cylinder(nut_opening_length,nut_diam/2,nut_diam/2, center=true,$fn=6);
        }

        // Elephant foot mitigation inner
        translate([0,0,0])
        hull()
        {
            cube([screew_mount_opening+0.3,screew_mount_opening+0.3, 0.01], center=true);        
            cube([screew_mount_opening,screew_mount_opening, 0.3], center=true);        
        }
    }
}

module mounting_pin(mode)
{
    if (mode==1)
    {
        cube([mounting_pin_width,mounting_pin_width,mounting_pin_length], center=true);        
        //cylinder(10,1, 1,$fn=6, center=true);
    }
}


module debugging_origo_marker()
{
    color("red") cube([1000,1,1], center=true);
    color("green") cube([1,1000,1], center=true);
    color("blue") cube([1,1,1000], center=true);
}


// Origo is center of PCB
module PCB(margin)
{
    m=margin*0.4;
    w=margin*1.2; // 1.6 was more than needed
    color("green")
    {
        // The PCB itself
        translate([0,0,0])
        cube([pcb_length+margin*1.6,pcb_width+w,pcb_thickness+margin*1.0], center=true);

        // The THT components on PCB front side
        translate([0,0,pcb_thickness/2 + components_height/2])
        cube([pcb_length-components_from_edge*2+1.5*m,pcb_width-components_from_edge*2+1*m,components_height+m], center=true);

        // The SMD components on PCB back side
        translate([0,0,-(pcb_thickness/2 + smd_height/2)])
        cube([pcb_length-components_from_edge*2+1.5*m,pcb_width-components_from_edge*2+1.5*m,smd_height], center=true);

        // Micro USb
        
        translate([-(85/2-micro_usb_length/2)-micro_usb_extrusion,0,micro_usb_pos*(pcb_thickness/2 + micro_usb_height/2)])
        cube([micro_usb_length+margin*8,micro_usb_width+margin*4,micro_usb_height+4*margin], center=true);

        // Adaptation for printing
        /*if (margin !=0)
        {
            translate([0,0,(components_height-smd_height)/2])            
            hull()
            {
            cube([pcb_length-components_from_edge*2-2,pcb_width+w,pcb_thickness-4+margin+smd_height+components_height], center=true);

            cube([pcb_length-components_from_edge*2-2,pcb_width-4+margin,pcb_thickness+margin+smd_height+components_height], center=true);

            }
        }*/

    }
    
    if ((margin) && (opaque_material_window_radius))
    {
        // openings for IR
        for(y=[-1,1])
        {
            /*translate([pcb_length/2, y*pcb_width/2, 10])
            rotate([0,90,y*45])
            cylinder(20, 4, 4, center=true);*/
            
            translate([pcb_length/2, y*(pcb_width/2-5), 10])
            rotate([0,90,0])
            cylinder(10, opaque_material_window_radius, opaque_material_window_radius, center=true);


            for(x=[-3:3])
            {
            translate([x*pcb_length*0.2, y*pcb_width/2, 10])
            rotate([90,0,0])
            cylinder(10, opaque_material_window_radius, opaque_material_window_radius, center=true);
            }

        }
    }
}


module battery(margin)
{
    m=margin*1.0;
    color("blue")   
    cylinder(battery_length+m,(battery_diameter+m)/2,(battery_diameter+m)/2,center=true);

    if (margin !=0)
    {
        translate([0, 0, ((battery_length+2*m)-10)/2])
        cylinder(10,(battery_diameter+4*m)/2,(battery_diameter+4*m)/2,center=true);

        cylinder(battery_length*0.65,(battery_diameter+4*m)/2,(battery_diameter+4*m)/2,center=true);

        translate([0, 0, -((battery_length+2*m)-10)/2])
        cylinder(10,(battery_diameter+4*m)/2,(battery_diameter+4*m)/2,center=true);
    }
}


// Origo is center of outer part of speeker
module speaker(margin)
{
    m=margin*1.0;
    color("orange")
    {
        translate([0, 0, speaker_rim_thickness/2])
        cylinder(speaker_rim_thickness+m,(speaker_diameter+m)/2,(speaker_diameter+m)/2,center=true);   

        translate([0, 0, speaker_thickness/2])
        cylinder(speaker_thickness+m,speaker_diameter/2-1,speaker_diameter/2-1,center=true);   
    }

    s=20;

    // opening for sound out
    translate([0,0,-s/2])
    if (margin !=0)
    {
        cylinder(s,4/2,4/2,center=true);   
        for(i=[0:5])
        {
            rotate([0, 0, i*60])
            translate([6,0,0])
            cylinder(s,4/2,4/2,center=true);   
        }

        for(i=[0:11])
        {
            rotate([0, 0, i*30+15])
            translate([12,0,0])
            cylinder(s,4/2,4/2,center=true);   
        }       
    }


    // opening for cable
    if (margin !=0)
    {
        d1=16;
        d2=speaker_diameter-1;
        
        translate([0,0,10/2])
        cylinder(10,d1/2,d1/2,center=true);   

        translate([0,0,8/2])
        cylinder(8,d2/2,d2/2,center=true);   
    }
}


// Origo is center of surface which diod is sitting in.
module diode(margin)
{
        m=0.5*margin;
        leg_length=7;

        // diode
        color("grey")
        hull()
        {
            translate([0,0,diode_length-(diode_diameter/2)])
            sphere((diode_diameter+margin*0.4)/2, $fn=24);
            cylinder(1+4.0*margin,(diode_diameter+margin*0.4)/2,(diode_diameter+margin*0.4)/2,center=true, $fn=24);   
        }

        // diode base
        translate([0, 0, -1/2])
        cylinder(1+m,(diode_diameter+1+m)/2,(diode_diameter+1+m)/2,center=true, $fn=24);

        // legs
        for(i=[-1,1])
        {
            translate([0, i*1.27, -leg_length/2])
            cylinder(leg_length+m,(0.7+m)/2,(0.7+m)/2,center=true);   
        }
}

// Origo is center of surface which diod is sitting in.
module lense(margin)
{  
    color("grey")
    if (margin!=0)
    {
        //nof_lens_positions=1; // use an odd number 1, 3 or 5.
        //for(i=[-(nof_lens_positions-1)*0.5:(nof_lens_positions-1)*0.5])
        for(i=[-1:1])
        {
            r = (lense_diameter+margin*0.9)/2;
            translate([0,0,lense_focal+lense_thickness*3*i])
            cylinder(lense_thickness+margin*0.9,r,r,center=true);
        }
    }
    else
    {
        translate([0,0,lense_focal])
        cylinder(lense_thickness,lense_diameter/2,lense_diameter/2,center=true);
    }
}

// Origo is center of surface which diod is sitting in.
module lense_and_diode(margin)
{   
    d = 10;
    l = 8;
    {
        translate([0, 0, 0])
        rotate([0,90,0])
        lense(margin);
        
        rotate([0,90,0])
        diode(margin);

        if (margin!=0)
        {
            // Opening for IR between diode and lense
            translate([((lense_focal-2)/2)+2, 0, 0])
            rotate([0,90,0])
            cylinder((lense_focal-2),radius_at_diode,radius_at_lense,center=true);
            
            // Opening for IR from lense and out
            after_lense_length=100;
            translate([lense_focal+after_lense_length/2, 0, 0])
            rotate([0,90,0])
            cylinder(after_lense_length, radius_after_lense, radius_after_lense, center=true);


            // Opening for diode legs and cable
            translate([-l/2, 0, 0])
            rotate([0,90,0])
            cylinder(l,d/2,d/2,center=true);            
        }
    }
}

// Origo is center of rim inner "surface".
module button(margin)
{
    //debugging_origo_marker();
    m=margin*0.4;
    nut_diam=18;

    {
        color("black")
        translate([1/2, 0, 0])       
        rotate([0,90,0])
        cylinder(1,button_rim_diameter/2,button_rim_diameter/2,center=true);

        color("grey")
        translate([-button_length/2, 0, 0])       
        rotate([0,90,0])
        cylinder(button_length+m,(button_diameter+m)/2,(button_diameter+m)/2,center=true);

        color("red")
        translate([3/2, 0, 0])       
        rotate([0,90,0])
        cylinder(3,button_red_diameter/2,button_red_diameter/2,center=true);

        // Nut
        color("grey")
        translate([-button_to_nut-(button_nut_length)/2, 0, 0])
        rotate([0,90,0])
        cylinder(button_nut_length+m,(button_nut_diameter+margin)/2,(button_nut_diameter+margin)/2,center=true, $fn=6);

        if(margin!=0)
        {
            // cutout for cable
            translate([-(button_length/2+24)/2-3, 0, 0])       
            rotate([0,90,0])
            cylinder((button_length/2+8)+m,19/2,19/2,center=true);
            
            // cutout for nut
            translate([-(button_length/2+20)/2-2, 0, 0])       
            rotate([0,90,0])
            cylinder((button_length/2+16)+m,nut_diam/2,nut_diam/2,center=true);

            // cutout outside
            translate([20/2, 0, 0])       
            rotate([0,90,0])
            cylinder(20+margin,(button_rim_diameter+margin)/2,(button_rim_diameter+margin)/2,center=true);
        }
    }
}

module laser(margin)
{
    m=1.0*margin;

    color("purple")
    rotate([0,90,0])
    cylinder(laser_length+1.5*margin,(laser_diameter+m)/2,(laser_diameter+m)/2,center=true);   

    color("purple")
    rotate([0,90,0])
    cylinder(laser_length-10,(laser_diameter+2.0*margin)/2,(laser_diameter+2.0*margin)/2,center=true);   

    // opening for light out
    if (margin!=0)
    {
        beam_length=100;
        color("red")
        translate([beam_length/2,0,0])
        rotate([0,90,0])
        cylinder(beam_length,3,3,center=true); 
    }

    // opening for cable
    if (margin !=0)
    {
        cable_length=40;
        translate([-cable_length/2,0,0])
        rotate([0,90,0])
        cylinder(cable_length,laser_diameter*0.4,laser_diameter*0.4,center=true);   
    }
}

module components(margin)
{
    translate([0,0,pcb_box_z])
    PCB(margin);

    translate([battery_x,0,battery_z])
    rotate([0,handle_tilt,0])
    battery(margin);

    translate([speaker_x,0,speaker_z])
    speaker(margin);

    translate([diode_x, 0, ir_barrel_z])
    lense_and_diode(margin);

    translate([button_x,0, button_z])
    button(margin);

    translate([68,0, -5])
    laser(margin);
}


// Origo is in the Diode end

module ir_barrel()
{
    //debugging_origo_marker();
    
    t = 3.5;
    // Wide front part of IR barrel
    hull()
    {
        translate([ir_barrel_length-ir_barrel_wide_length/2, 0, 0])
        rotate([0,90,0])
        cylinder(ir_barrel_wide_length,(lense_diameter+t)/2,(lense_diameter+t)/2, center=true);
        
        translate([ir_barrel_length - ir_barrel_wide_length - (lense_diameter+t)/2, 0, 0])
        sphere((lense_diameter+t)/2, $fn=24);
    }

    // thinner rear part of IR barrel
    translate([ir_barrel_length*0.4/2, 0, 0])
    hull()
    {
        translate([0, 0, 4.2])
        rotate([0,90,0])
        cylinder(ir_barrel_length*0.4,(lense_diameter+t)*0.4,(lense_diameter+t)*.4, center=true);

        translate([0, 0, -4.2])
        rotate([0,90,0])
        cylinder(ir_barrel_length*0.4,(lense_diameter+t)*0.4,(lense_diameter+t)*.4, center=true);
    }
}

// Place for PCB
module pcb_box()
{
    /*intersection()
    {
        rotate([0,90,0])
        hull()
        {
            translate([0,pcb_width/3,0])
            cylinder(pcb_length+2,(pcb_width+3)/3,(pcb_width+3)/3, center=true);
            translate([0,-pcb_width/3,0])
            cylinder(pcb_length+2,(pcb_width+3)/3,(pcb_width+3)/3, center=true);
        }

        cube([pcb_length+2, pcb_width+3, pcb_width+10], center=true);
    }*/

    if (major_version==1)
    {
        hull()
        {
            cube([pcb_length+1, pcb_width-5, 36], center=true);
            cube([pcb_length+6.0, pcb_width+5, 28], center=true);
        }
    }
    else
    {
        translate([0,0,1/2])
        hull()
        {
            cube([pcb_length+1, pcb_width-5, 37], center=true);
            cube([pcb_length+6.0, pcb_width+5, 29], center=true);
        }
    }
}



// Origo is where the push button sit (well it should be but its a little above that)
module handle()
{
    //debugging_origo_marker();
    wall_thickness = 3.5;

    x_offset=sin(handle_tilt)*battery_length;
    z_outer_extension=4+handle_extra_length;
    z_inner_extension=8+handle_extra_length;


    //rotate([0,handle_tilt,0])
    translate([0, 0, -button_z-handle_extra_length/2])
    hull()
    {        
        // place for button
        translate([-15+6,0, battery_z])
        {          
            // upper
            translate([x_offset/4-1,0, battery_length/2])
            {
                cylinder(z_outer_extension,battery_diameter/2+wall_thickness,battery_diameter/2+wall_thickness,center=true);
                cylinder(z_inner_extension,battery_diameter/2+wall_thickness/2,battery_diameter/2+wall_thickness/2,center=true);
            }
            
            // lower
            translate([-x_offset-4,0, -battery_length/2])
            {
                cylinder(z_outer_extension,battery_diameter/2+wall_thickness,battery_diameter/2+wall_thickness,center=true);
                cylinder(z_inner_extension,battery_diameter/2+wall_thickness/2,battery_diameter/2+wall_thickness/2,center=true);
            }
            
        }

    
        // place for battery
        translate([battery_x-button_x-4,0, battery_z])
        {
            // upper
            translate([x_offset*1/4+3,0, battery_length/2])
            {
                cylinder(z_outer_extension,battery_diameter/2+wall_thickness,battery_diameter/2+wall_thickness,center=true);
                cylinder(z_inner_extension,battery_diameter/2+wall_thickness/2,battery_diameter/2+wall_thickness/2,center=true);
            }

            // lower
            translate([-x_offset*2/4+3,0, -battery_length/2])
            {
                cylinder(z_outer_extension,battery_diameter/2+wall_thickness,battery_diameter/2+wall_thickness,center=true);
                cylinder(z_inner_extension,battery_diameter/2+wall_thickness/2,battery_diameter/2+wall_thickness/2,center=true);
            }
        }


    }    
}

module screw_mounts(mode)
{
    if (mode==1)
    {
        // lower end of handle
        translate([battery_x+17, 0, battery_z - 32])
        {
            rotate([90,0,0])
            screw_mount(mode, 30);

            for(i=[0:0])
            {       
                translate([i*10, 0, 10])
                rotate([90,0,0])
                mounting_pin(mode);
            }
        }
      
        // upper rear
        translate([ir_barrel_x-5, 0, 34])
        rotate([90,0,0])
        screw_mount(mode, 24);

        // center above button
        translate([button_x+8, 0, 11])
        rotate([90,0,0])
        screw_mount(mode, 50);

        // top middle of IR barrel
        translate([-40, 0, 37])
        {
            rotate([90,0,0])
            screw_mount(mode, 40);

            /*for(i=[-3:1])
            {       
                translate([20*i, 0, 0])
                rotate([90,0,0])
                mounting_pin(mode);
            }*/
            
            // Room for a bar to prevent IR light to leak out.
            translate([-43, 0, 0])
            cube([bar_length,bar_width+0.3,bar_height+0.3],center=true);
        }
    }
    
    // behind speaker, in front of PCB
    translate([speaker_x-speaker_diameter/2+4.2, 0, 1.7])
    rotate([90,0,0])
    //rotate([0,0,90])
    screw_mount(mode, 33);


    // On top of barrel
    for(i=[barrel_top_rear_screw_x,barrel_top_front_screw_x])
    {
        translate([i, 0, lense_diameter+5.5])
        rotate([90,0,0])
        screw_mount(mode, 10);
    }

    // Under barrel, in front of speaker but below laser
    translate([speaker_x+speaker_diameter/2+4, 0, speaker_z+6])
    rotate([90,90,0])
    screw_mount(mode, 10);


    // Screws needed if printer is small (prusa mini can print 18x18x18 cm)
    if(printer_cut_x!=999)
    {
        // screws mid (between barrel and PCB box)
        /*for(i=[-1,1])
        {
            translate([printer_cut_x, i*22.5, 11.5])
            rotate([0,90,0])
            rotate([0,0,90])
            screw_mount(mode, 25);
        }*/
        for(i=[-1,1])
        {
            translate([printer_cut_x, i*23.1, 12])
            rotate([0,90,0])
            screw_mount(mode, 25);
        }


        // Screws on IR barrel a bit above middle
        /*translate([printer_cut_x, 0, ir_barrel_z])
        for(i=[-1,-5])
        {
            rotate([0,90,0])            
            rotate([0,0,-30*i])
            translate([0, lense_diameter/2+5,0])
            screw_mount(mode, 22);
        }*/
        translate([printer_cut_x, 0, ir_barrel_z])
        for(i=[-1,1])
        {
            rotate([0,90,0])
            rotate([0,0,90-45*i])
            translate([0, lense_diameter/2+5,0])
            screw_mount(mode, 22);
        }


        // the screws under PCB box
        if (major_version==1)
        {
            for(i=[-1,1])
            {
                translate([printer_cut_x, i*(5.5), screws_under_PCB_box_z])
                rotate([0,90,0])
                rotate([0,0,90])
                screw_mount(mode, 25);
            }
        }
        else
        {
            for(i=[-1,1])
            {
                translate([printer_cut_x, i*(5.5+3), screws_under_PCB_box_z])
                rotate([0,90,0])
                rotate([0,0,90])
                screw_mount(mode, 25);
            }
        }        


        // Some pins between front and rear halves
        /*translate([printer_cut_x, 0, ir_barrel_z])
        for(i=[-5, -4, -3, -2, -1, 0, 1, 2, 3, 4])
        {       
            rotate([i*30+15,0,0])
            translate([0,0,16])
            rotate([0,90,0])
            mounting_pin(mode);
        }*/


        translate([printer_cut_x, 0, ir_barrel_z])
        for(i=[-4.5:4.5])
        {      
            translate([0, 16*sin(i*30),16*cos(i*30)])
            rotate([0,90,0])
            rotate([0,0,45])
            mounting_pin(mode);
        }

    }

    if (major_version!=1)
    {
        // under PCB box 
        translate([screw_under_front_PCB_box_x, 0, screws_under_PCB_box_z])
        rotate([90,0,0])
        screw_mount(mode, 10);
    }




    // More like decoration under the PCB box
    /*translate([0, 0, -28])
    for(i=[-3:2])
    {       
        translate([10*i, 0, 0])
        rotate([90,0,0])
        mounting_pin(mode);
    }*/

}

// speaker and laser
module speaker_box()
{
    w = 2.5;
    
    translate([0,0,-10])
    cylinder(20+4,(speaker_diameter+w)/2,(speaker_diameter+w)/2,center=true);

    translate([-speaker_diameter/2,0,0])
    cube([60+2, 30, speaker_diameter+w],center=true);
}

module casing_base()
{
    //debugging_origo_marker();


    translate([ir_barrel_x,0, ir_barrel_z])
    ir_barrel();      
    
    // Place for PCB
    translate([pcb_x,0, pcb_z])
    pcb_box();
    
    translate([handle_x+10,0, button_z])
    handle();  

    // place for speaker and laser
    translate([speaker_x,0,0])
    speaker_box();
    
    // place for IR diode
    translate([ir_barrel_x,0,10])
    difference()
    {
        d=20;
        h=12;
        w=10;
        hull()
        {
            //cube([20+2,20+2,10+2],center=true);
            translate([0,0,4])
            rotate([0,90,0])
            cylinder(d+2,(d+2)/2,(d+2)/2,center=true);       
            
            translate([0,0,20])
            rotate([0,90,0])
            cylinder(d+2,(d+2)/2,(d+2)/2,center=true);       
        }
        // make room for cable from IR diode down to battery.
        translate([0,0,h/2])
        cube([d-1,w,h],center=true);       
    }

    screw_mounts(0);

    // Some decorations, "fins" on the tip of barrel
    translate([ir_barrel_x+ir_barrel_length*0.95,0,ir_barrel_z])
    {
        for(i=[-2.5, -1.5, 1.5, 2.5])
        {
            rotate([i*45,0,0])
            translate([0, 0, lense_diameter/2])
            
            // sphere(6, $fn=24);
            rotate([90,0,0])
            cylinder(2,12/2,12/2,center=true);
        }           
    }


    // place for the button, to make an opening for the button in.
    translate([button_x-button_to_nut/2, 0, button_z])
    rotate([0,90,0])
    cylinder(button_to_nut,(button_rim_diameter+2)/2,(button_rim_diameter+0.5)/2,center=true);
}

module printing_supports_flat()
{
    // Supports in the PCB cavety
    translate([0, 0, -components_height/2+2])   
    for(j=[-1:1])
    {
        translate([0, 0, j*7])   
        {
            // floor for pillars to stand on
            cube([pcb_length, 1, 2],center=true);
            
            // pillars to cary up "roof"
            for(i=[-5:5])
            {
                translate([i*(pcb_length-5)/12, 0, 0])   
                cube([2, pcb_width, 0.5],center=true);
            }

            // beams carried up by pillars
            for(i=[-1,1])
            {
                translate([0, i*(pcb_width/2+0.6), 0])   
                cube([pcb_length, 1, 0.5],center=true);

                translate([0, i*(pcb_width/4), 0])   
                cube([pcb_length, 1, 0.5],center=true);
            }
        }
        translate([j*30, 0, 0])   
        cube([2, 1, components_height+8],center=true);
    }

    // Supports in the IR barrel cavety
    translate([ir_barrel_x, 0, ir_barrel_z])   
    {
        d = 6;
        for(j=[2:34])
        {            
            x = j*d+1; //j*ir_barrel_length*0.045;
            r = radius_at_diode + x*(radius_after_lense-radius_at_diode)/lense_focal;
            // "ekrar"
            for(i=[-2:2])
            {
                rotate([12.5*i,0,0])
                translate([x, 0, 0])
                cube([0.5, r*2.1, 1.5],center=true);
            }
            // string at top 
            for(i=[-1,1])
            translate([x-d/2, i*(r-1.2), 0])
            cube([d, 0.6, 0.5],center=true);
        }
        translate([ir_barrel_length/2, 0, 0])   
        cube([ir_barrel_length*0.96, 1, 2],center=true);      

        for(i=[1:3])
        {
            x = i*(ir_barrel_length/4-1);
            r = radius_at_diode + x*(radius_after_lense-radius_at_diode)/lense_focal;
            translate([x, 0, 0])   
            cube([2, 1, r*2],center=true);      
        }
    }
    
    // Support in battery cavity
    translate([battery_x, 0, battery_z])
    rotate([0,handle_tilt,0])
    cube([0.6,2,battery_length*1.15], center=true);
    translate([battery_x, 0, battery_z])
    for(i=[-2:2])
    {
        rotate([0,handle_tilt,0])
        translate([0, 0, i*battery_length*0.17])
        cube([8, (battery_diameter+6), 0.5],center=true);      
    }



    // small support between button and battery caveties
    translate([(battery_x*0.6+button_x*0.4), 0, button_z])
    cube([1, 1,62], center=true);

    
    
    // Support in the USB opening
    translate([-pcb_length/2-1.5, 0, -15])
    cube([0.5,20,20],center=true);  


    // support for screw under read PCB box 
    translate([printer_cut_x, 0, screws_under_PCB_box_z])
    cube([24,6,9],center=true);
}

module printing_supports_standing_lens_notches()
{
    // Support for the grooves that the lense shall sit in.
    translate([ir_barrel_x+lense_focal, 0, ir_barrel_z])
    for(i=[0:7])
    {
        rotate([45*i,0,0])
        translate([0, 0.5*(lense_diameter-2), 0])
        cube([30,1,1],center=true);
    }   
}

module printing_supports_standing()
{
    // Supports in the PCB cavety
    translate([0, 0, -components_height/2+2])   
    for(j=[-2:2])
    {
        //w = (j==0) ? 0.6 : 0.6;
        zs=(components_height)*0.51;

        // Something for vertical pillars to stand on
        translate([printer_cut_x, 0, zs])   
        cube([1, 40, 2],center=true);

        translate([printer_cut_x, j*pcb_width/6, 0])   
        {
            // horisontal bars
            for(i=[0:3])
            {
                translate([i*pcb_length*0.257, 0, -0.5])
                cube([1, 1, components_height+7.8],center=true);               
            }
            
            for(i=[-1:1])
            {
                translate([3*0.258*pcb_length, 0, i*zs])
                cube([0.8, 0.4*pcb_width, 1],center=true);
            }

            // vertical pillars
            for(i=[-1:1])
            {
                translate([-printer_cut_x,0, i*zs])
                cube([pcb_length, 1, 1.5],center=true);
            }
        }
    }    


    printing_supports_standing_lens_notches();

    // Support for screw mount on top of barrel front
    for(y=[-4,0,4])
    {
        translate([barrel_top_front_screw_x, y, lense_diameter+3])
        hull()
        {
            ox = -6;
            oz = -0.5;
            h = 7.5;
            m = 1;
            w = (y==0) ? 1 : 0.5;
            translate([ox, 0, oz+m*h/2])
            cube([1,w,h],center=true);
            translate([ox-14/2, 0, oz])
            cube([14,w,0.1],center=true);
        }
    }

    // support for screw under front PCB box 
    translate([printer_cut_x, 0, screws_under_PCB_box_z])
    cube([20,6,9],center=true);

    // support for screw under front PCB box 
    /*translate([screw_under_front_PCB_box_x, 0, screws_under_PCB_box_z])
    hull()
    {
        ox = -6;
        oz = 3;
        h = 7.5;
        m = -1;
        translate([ox, 0, oz+m*h/2])
        cube([1,1,h],center=true);
        translate([ox-14/2, 0, oz])
        cube([14,1,0.1],center=true);
    }*/
}

/*
supports: 0= no supports, 1 = for printing laying, 2 = for standing
*/
module casing(supports)
{
    difference()
    {
        casing_base();  

        // Make rom for components such as PCB, battery, button etc.
        components(1);
        
        // Opening for cable to IR diode
        /*translate([ir_barrel_x,0,0])
        cylinder(20,(20)/2,(20)/2,center=true);
        translate([3,0,0])
        rotate([0,90,0])
        cylinder(pcb_length-3,21/2,11/2,center=true);*/


        // Opening for cable to speaker
        translate([pcb_length/2,0,speaker_z+5])
        rotate([0,90,0])        
        hull()
        {
            for(i=[-1,1])
            {
                translate([0,5*i,0])
                cylinder(20,(8)/2,(8)/2,center=true);
            }
        }


        // Opening for cable from button (vertical part)
        rotate([0,handle_tilt,0])
        translate([button_x-12, 0, button_z-10])
        cylinder(55,(battery_diameter*0.8)/2,(battery_diameter*0.8)/2,center=true);

        // Opening for cable from button (horisontal from battery/button to PCB)
        translate([button_x/2-14, 0, 5])
        rotate([0,90,0])
        hull()
        {
            for(i=[-1,1])
            {
                translate([0,4*i,0])
                cylinder(80,(6)/2,(6)/2,center=true);
            }
        }

      
        
        // Between IR-Diode, battery and button
        translate([(battery_x+(button_x-12))/2-2,0,11.5])
        cube([battery_diameter+15,battery_diameter*0.9,6.1],center=true);


        // Adaptation of PCB cavity for printing
        // The slit for PCB will be a problem if printing lying down.
        w=(supports == 1) ? 2 : -1;
        translate([0,0, pcb_z])
        {
            hull()
            {
                cube([pcb_length-components_from_edge*2-2,pcb_width+w,pcb_thickness+smd_height+components_height-3], center=true);

                cube([pcb_length-components_from_edge*2-2,pcb_width-3,pcb_thickness+smd_height+components_height+2.5], center=true);
            }
        }       

        screw_mounts(1);

        // Openings for detectors and LEDs
        /*
        cube([6,100,6],center=true);

        translate([36,0,-5])
        cube([6,100,8],center=true);

        translate([26,0,-5])
        cube([6,100,8],center=true);

        translate([16,0,-5])
        cube([6,100,8],center=true);

        translate([0,24,-5])
        cube([100,6,8],center=true);
    */

        // some decorations

        // Some elongated opening on the rear of IR barrel 
        translate([ir_barrel_x+10,0,ir_barrel_z])
        {
            for(i=[-2,2])
            {
                rotate([i*45,0,0])
                translate([0, 0, 20])
                
                hull()
                {
                    sphere(6, $fn=24);
                    translate([30, 0, 0])
                    sphere(6, $fn=24);
                }
            }
        }

        cooling_opening = 6/40*lense_diameter;

        // A number of cooling "circular" openings on the front of IR Barrel inspired by tec 9.
        for(x=[0.5:2.5])
        {
            translate([ir_barrel_x+106+x*24,0,ir_barrel_z])
            {                
                for(i=[-2:2])
                {
                    rotate([i*45,0,0])
                    translate([0, 0, lense_diameter/2+2])
                    
                    cylinder(6, cooling_opening*0.7, cooling_opening*0.7, $fn=24, center=true);
                }           
                translate([12, 0, 0])
                for(i=[-2.5:2.5])
                {
                    rotate([i*45,0,0])
                    translate([0, 0, lense_diameter/2+2])

                    cylinder(6, cooling_opening*0.7, cooling_opening*0.7, $fn=24, center=true);
                }           
            }
        }

        /*font = "Liberation Sans";   
        translate ([40,30,-15]) {
           rotate([90,0,180])
           linear_extrude(height = 0.6) {
               text("drekkar", font = font, size = 12);
             }
         }*/


        /*if (supports == 8)
        {          
            translate([ir_barrel_x+(lense_focal)/2,0,ir_barrel_z])        
            rotate([0,90,0])
            cylinder((lense_focal-2),0.4*lense_diameter/2,0.4*lense_diameter/2,center=true);
        }*/

    }       
    
    if (supports == 1)
    {
        printing_supports_flat();
    }       
    if (supports == 2)
    {
        printing_supports_standing();
    }
    if (supports == 4)
    {
        printing_supports_standing_lens_notches();
    }

    // support for screw under front PCB box 
    translate([printer_cut_x, 0, screws_under_PCB_box_z])
    cube([23,6,9],center=true);
}

module left()
{
    //rotate([90,0,0])
    difference()
    {    
        casing(0);
        translate([0, -60/2, 0])
        cube([400,60,400],center=true);

        //translate([200/2+printer_cut_x, 0, 0])
        //cube([200,100,200],center=true);     
    }
}

module front()
{
    //rotate([90,0,0])
    difference()
    {    
        casing(0);
        translate([printer_cut_x+140/2, 0, 0])
        cube([140,100,120],center=true);

        //translate([200/2+printer_cut_x, 0, 0])
        //cube([200,100,200],center=true);     
    }
}


module left_rear(support)
{
    rotate([90,0,0])
    difference()
    {    
        casing(support);
        translate([0, -60/2, 0])
        cube([400,60,400],center=true);

        translate([200/2+printer_cut_x, 0, 0])
        cube([200,100,200],center=true);     
    }
}

module right_rear(support)
{
    rotate([-90,0,0])
    difference()
    {    
        casing(support);
        translate([0, 60/2, 0])
        cube([400,60,400],center=true);

        translate([200/2+printer_cut_x, 0, 0])
        cube([200,100,200],center=true);     
    }
}

module right_front(support)
{
    rotate([-90,0,0])
    difference()
    {    
        casing(support);
        translate([0, 60/2, 0])
        cube([400,60,400],center=true);

        translate([-(200/2)+printer_cut_x, 0, 0])
        cube([200,100,200],center=true);     
    }
}

module left_front(support)
{
    rotate([90,0,0])
    difference()
    {    
        casing(support);
        translate([0, -60/2, 0])
        cube([400,60,400],center=true);

        translate([-(200/2)+printer_cut_x, 0, 0])
        cube([200,100,200],center=true);     
    }
}

module right_front_standing(support)
{
    rotate([0,-90,0])
    translate([-printer_cut_x, 0, 0])
    difference()
    {    
        casing(support);
        translate([0, 60/2, 0])
        cube([400,60,400],center=true);

        translate([-(200/2)+printer_cut_x, 0, 0])
        cube([200,100,200],center=true);     
    }
}

module left_front_standing(support)
{
    rotate([0,-90,0])
    translate([-printer_cut_x, 0, 0])
    difference()
    {    
        casing(support);
        translate([0, -60/2, 0])
        cube([400,60,400],center=true);

        translate([-(200/2)+printer_cut_x, 0, 0])
        cube([200,100,200],center=true);     
    }
}

as_width=18;
as_height=14;
as_length=10;

module misc_parts(margin)
{
    w=1;
    m=0.4*margin;
        
    // The bar to be placed in upper rear of barrel to prevent IR leaking out.
    translate([0,0, (bar_height-0.5)/2])
    cube([bar_length-0.6,bar_width-0.5,bar_height-0.5],center=true);

    // The angled sheet to be placed behind IR diode to prevent light leaking
    translate([0, 20, (as_width-m)/2])   
    cube([w, as_height-m, (as_width-m)],center=true);
    
    translate([-(as_length-m-w)/2, 20+(as_height-m-w)/2, (as_width-m)/2])
    cube([as_length-m,w,(as_width-m)],center=true);
 
 
    for(i=[1:10])
    {
        translate([0, -10*i, (mounting_pin_width-m)/2])   
        cube([mounting_pin_length-4*m, mounting_pin_width-m, mounting_pin_width-m], center=true);
    }
  
}

module round_windows()
{
    n = 12;
    r = opaque_material_window_radius-0.3;
    for(i=[-(n/2-0.5):(n/2-0.5)])
    {
        translate([i*((opaque_material_window_radius)*2+3), 0, 0])
        {
            translate([0, 0, 2/2])
            cylinder(2, r, r, $fn=24, center=true);

            translate([0, 0, 0.6/2])
            cylinder(0.6, r+1, r+1, $fn=24, center=true);
        }
    }
}

//battery(1);
//button(1);
//PCB(1);
//speaker(1);
//lense_and_diode(1);
//laser(1);

// Printing supports
// 0: no supports, set slicer to "support on build plate only"
// 1: own supports for printing on long side down.
// 2: own supports for printing standing (on center).
// 4: Minimalisic own supports, set slicer to "support on build plate only"
// 8: Expanded caviteis, set slicer to "support on build plate only"

// Uncomment one of the below
//casing();
left();
//front();
//components(0);
//diode(0);
//lense_and_diode(0);
//left_front(1);
//right_front(1);

// Without supports, set slicer to "support on build plate only".
// This currently gives best quality but takes a little longer printing time.
//left_rear(0);
//right_rear(0);
//left_front_standing(4);
//right_front_standing(4);

// With own custom supports
// Prints faster but interior will look less nice.
//left_rear(1);
//right_rear(1);
//left_front_standing(2);
//right_front_standing(2);

// This part are small pieces to put between the main parts.
//misc_parts(1);

// This part are small round windows to be used if case is opaque
//round_windows();
