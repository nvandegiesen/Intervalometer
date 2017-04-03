//Piezo Holder
$fn=64;
diameterPiezo=36.0; //intern
radiusPiezo=diameterPiezo/2;
inlay=0.25;
width_inlay=2.0;
width_rim=1.0;
width_bottom=5.0;
height=10;
height_bottom=3;
radius_hole=1.25;
rad1=radiusPiezo+width_rim;
rad2=radiusPiezo-width_bottom;
rad3=radiusPiezo-width_inlay;
rad4=radiusPiezo;
h1=height-height_bottom;
	difference()
    {    
        cylinder(r=rad1,h=height);
        translate ([0,0,height_bottom])
        cylinder (r1=rad2,r2=rad3,h=h1);
        translate ([0,0,height-inlay])
        cylinder (r=rad4,h=height);
        translate ([0,0,-2])
        cylinder (r=radius_hole,h=height);
      //Slot for solder
        translate ([0,rad4-2,height])
        cube(size = [3,4,4], center = true);
    }
    
 