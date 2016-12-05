scale = 1;
mm = 1 * scale;

WT32_BASE_WIDTH  = 15.90 * mm;
WT32_BASE_LENGTH = 23.90 * mm;
WT32_BASE_HEIGHT = 1.6 * mm;

WT32_COVER_WIDTH  = 14.90 * mm;
WT32_COVER_LENGTH = 17.90 * mm;
WT32_COVER_HEIGHT = 0.90 * mm;

WT32_ANTENNA_WIDTH  = 3.7 * mm;
WT32_ANTENNA_LENGTH = 2.0 * mm;
WT32_ANTENNA_HEIGHT = 0.60 * mm;
WT32_ANTENNA_X_CENTER = -2.40 * mm;


translate([0,0,WT32_BASE_HEIGHT/2]){
cube([WT32_BASE_WIDTH,WT32_BASE_LENGTH,WT32_BASE_HEIGHT], center = true);
    translate([0,
               (WT32_COVER_LENGTH - WT32_BASE_LENGTH) / 2,
                (WT32_BASE_HEIGHT + WT32_COVER_HEIGHT) / 2]){
                    cube([WT32_COVER_WIDTH,WT32_COVER_LENGTH, WT32_COVER_HEIGHT], center = true);
                }
                translate([WT32_ANTENNA_X_CENTER,
               (WT32_BASE_LENGTH - WT32_ANTENNA_LENGTH) / 2,
                (WT32_BASE_HEIGHT + WT32_ANTENNA_HEIGHT) / 2]){
                    cube([WT32_ANTENNA_WIDTH,WT32_ANTENNA_LENGTH, WT32_ANTENNA_HEIGHT], center = true);
                }
}