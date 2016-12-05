scale = 1;
mm = 1 * scale;
BODY_LENGTH = 14.5 * mm;
BODY_WIDTH = 6 * mm;
BODY_HEIGHT = 5 * mm;
BODY = [BODY_WIDTH,
        BODY_LENGTH,
        BODY_HEIGHT];
PLUG_DIAMETER = 5 * mm;
PLUG_LENGTH = 2.5 * mm;
PLUG_CUTOUT_DIAMETER = 3.6 * mm;

$fn = 72;

translate([-BODY_WIDTH/2,0,0]){
  cube(BODY);
  translate([BODY_WIDTH, 2.8 * mm - 0.5 * mm, 0]){
        cube([1.5*mm, 1*mm, 0.2*mm]);
  }
  translate([BODY_WIDTH, 11.8 * mm - 0.75 * mm, 0]){
        cube([1.5*mm, 1.5*mm, 0.2*mm]);
  }
  translate([BODY_WIDTH/2 - 1.5 * mm, 14.5 * mm, 0]){
        cube([1.5*mm, 1.5*mm, 0.2*mm]);
  }
  translate([-1.5 * mm, 4.4 * mm - 0.5 * mm, 0]){
        cube([1.5*mm, 1*mm, 0.2*mm]);
  }
}

rotate([90,0,0]){
    translate([0,PLUG_DIAMETER/2,0]){
  difference(){
        cylinder(d=PLUG_DIAMETER, h = PLUG_LENGTH);
        cylinder(d=PLUG_CUTOUT_DIAMETER, h = PLUG_LENGTH);
  }
  }
}

 //cube([1.5*mm, 1*mm, 0.2*mm]);