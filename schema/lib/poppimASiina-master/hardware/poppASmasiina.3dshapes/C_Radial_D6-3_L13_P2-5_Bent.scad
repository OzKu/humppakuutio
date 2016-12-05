mm = 1;

//dimensons
CAP_HEIGHT_NOM = 12.5 * mm;
CAP_HEIGHT_TOL = 0.5 * mm;
CAP_DIA_NOM = 6.3 * mm;
CAP_DIA_TOL = 0.5 * mm;
CAP_LEG_DIA_NOM = 0.5 * mm;
CAP_LEG_PITCH = 2.5 * mm;
CAP_LEG_LENGTH_MIN = 15 * mm;

//calculated values
CAP_HEIGHT_MAX = CAP_HEIGHT_NOM + CAP_HEIGHT_TOL;
CAP_DIA_MAX = CAP_DIA_NOM + CAP_DIA_TOL;

translate([CAP_LEG_PITCH/2,0,0]){
translate([CAP_LEG_DIA_NOM/2,0,CAP_DIA_MAX/2]){
    rotate([90, 0, 0]){
  cylinder(h=CAP_HEIGHT_MAX, d = CAP_DIA_MAX, $fn = 72);
    }
}
    translate([CAP_LEG_PITCH/2, 0, -(CAP_LEG_LENGTH_MIN-CAP_DIA_MAX/2) ]){
        cylinder(h = CAP_LEG_LENGTH_MIN, d = CAP_LEG_DIA_NOM, $fn = 36);
    }
     translate([-CAP_LEG_PITCH/2, 0, -(CAP_LEG_LENGTH_MIN- CAP_DIA_MAX/2) ]){
        cylinder(h = CAP_LEG_LENGTH_MIN, d = CAP_LEG_DIA_NOM, $fn = 36);
    }
}