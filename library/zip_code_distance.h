/* ------------------------------------------------------------------
zip_code_distance.h

Tim Riley
-------------------------------------------------------------------- */

#ifndef ZIP_CODE_DISTANCE_H
#define ZIP_CODE_DISTANCE_H

#define SEARCH_ZIP_FILE	"/home/opt/search_z/search_z.rnd"

double compute_distance_between_zips( char *zip_code1, char *zip_code2 );
double compute_distance_between_points( double longitude1, double latitude1,
                         		double longitude2, double latitude2 );
double compute_distance_between_zip_and_points(
					char *zip_code,
                         		double longitude, double latitude );

#endif
