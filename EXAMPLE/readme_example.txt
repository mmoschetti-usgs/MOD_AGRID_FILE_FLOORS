echo "SOURCE_MIN_LON=-116" > params_coords.txt
echo "SOURCE_MAX_LON=-60" >> params_coords.txt
echo "SOURCE_INCR_LON=0.1" >> params_coords.txt
echo "SOURCE_MIN_LAT=23.0" >> params_coords.txt
echo "SOURCE_MAX_LAT=53" >> params_coords.txt
echo "SOURCE_INCR_LAT=0.1" >> params_coords.txt
paramf=params_coords.txt
agrid_smoothed_f=agrd_adN4_incr_Mmin3p2_1870_2017_M0.out
agrid_avg_f=avg_M0_CEUS_2zone.out
agrid_out_f=agrd_adSm_2zone
mod_agrid_file_floors $paramf $agrid_smoothed_f $agrid_avg_f $agrid_out_f
