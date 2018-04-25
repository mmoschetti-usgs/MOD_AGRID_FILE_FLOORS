#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "params.h"
#define WT_AVG 0.2


/* Read in a reference/background agrid file and a file with updated agrid values
 * 
 */

// fortran functions
//void read_header(FILE *fp, int nlines);
void read_parameter_file(char *paramf, struct src_params* pars);
void write_results_to_file(char *bsnm, float a_adapt[], float a_wt[], float lon[], float lat[], int ncnt);
void Parse_Text(FILE *fp, const char *querystring, const char type, void* result);



/*--------------------------------------------------------------------------*/
void write_results_to_file(char *bsnm, float a_adapt[], float a_wt[], float lon[], float lat[], int ntot)
/*--------------------------------------------------------------------------*/
{ 
  FILE *fpoutAd, *fpoutaAd, *fpoutWt, *fpoutaWt;
  char agridf_adapt[200], agridf_adapt_ascii[200], agridf_wt[200], agridf_wt_ascii[200];
  int cnt, nwritten;
 
// output file names
  sprintf(agridf_adapt,"%s_adapt.out", bsnm);
  sprintf(agridf_wt,"%s_wt.out", bsnm);
  sprintf(agridf_adapt_ascii,"%s_adapt.txt", bsnm);
  sprintf(agridf_wt_ascii,"%s_wt.txt", bsnm);

// WRITE RESULTS TO FILE
// ASCII
// adaptive-weighting
  fpoutaAd=fopen(agridf_adapt_ascii,"w");
  for(cnt=0; cnt<ntot; cnt++) {
    fprintf(fpoutaAd,"%.2f %.2f %.4e\n", lon[cnt], lat[cnt], a_adapt[cnt]);
  }
  fclose(fpoutaAd);
// uniform-weighting
  fpoutaWt=fopen(agridf_wt_ascii,"w");
  for(cnt=0; cnt<ntot; cnt++) {
    fprintf(fpoutaWt,"%.2f %.2f %.4e\n", lon[cnt], lat[cnt], a_wt[cnt]);
  }
  fclose(fpoutaWt);
// BINARY OUTPUT
// adaptive-weighting
  fpoutAd=fopen(agridf_adapt,"wb+");
  nwritten = fwrite(a_adapt,4,ntot,fpoutAd);
  fclose(fpoutAd);
// uniform-weighting
  fpoutWt=fopen(agridf_wt,"wb+");
  nwritten = fwrite(a_wt,4,ntot,fpoutWt);
  fclose(fpoutWt);

// print summary to screen
  fprintf(stderr,"Modified 10^a values written to files.  \nAdaptive weighting, binary: %s, ascii: %s\nUniform weighting, binary: %s, ascii: %s\n", agridf_adapt, agridf_adapt_ascii, agridf_wt, agridf_wt_ascii);
}


/*--------------------------------------------------------------------------*/
void read_parameter_file(char *paramf, struct src_params* pars)
/*--------------------------------------------------------------------------*/
{
  FILE *fpparam;

// open file
  if( (fpparam = fopen(paramf, "r"))==NULL) {
    fprintf(stderr,"Parameter file not found, %s\n", paramf);
    exit(1);
  }

// get parameters
  Parse_Text(fpparam,"SOURCE_MIN_LON",'f',&(pars->min_lon));
  Parse_Text(fpparam,"SOURCE_MAX_LON",'f',&(pars->max_lon));
  Parse_Text(fpparam,"SOURCE_INCR_LON",'f',&(pars->inc_lon));
  Parse_Text(fpparam,"SOURCE_MIN_LAT",'f',&(pars->min_lat));
  Parse_Text(fpparam,"SOURCE_MAX_LAT",'f',&(pars->max_lat));
  Parse_Text(fpparam,"SOURCE_INCR_LAT",'f',&(pars->inc_lat));

// 
  pars->nlon=(int)((pars->max_lon-pars->min_lon)/pars->inc_lon+1);
  pars->nlat=(int)((pars->max_lat-pars->min_lat)/pars->inc_lat+1);
  pars->ntot=pars->nlat* pars->nlon;
// check array size
  if ( pars->ntot > NV) {
    fprintf(stderr,"Too many points (%d) for array size, NV=%d\n", pars->ntot, (int)NV);
    fprintf(stderr,"nlon/nlat: %d %d\n", pars->nlon, pars->nlat);
    exit(1);
  }

}

/*--------------------------------------------------------------------------*/
void print_parameter_file_format(void)
/*--------------------------------------------------------------------------*/
{
//  fprintf(stderr,"--h option not working yet!!\n");
  exit(1);
}


/*--------------------------------------------------------------------------*/
int main (int argc, char *argv[])
/*--------------------------------------------------------------------------*/
{
  FILE *fp_ref, *fp_avg;
  int ntot, nread;
  int cnt=0, cnt1=0, cnt2=0;
  float wtavg;
  float agrid_adapt[NV], agrid_wt[NV], lon_arr[NV], lat_arr[NV], agrid_ref[NV], agrid_avg[NV];
  char paramf[200], agrid_bsnm_f[200], agrdf_ref[200], agrdf_avg[200];
  struct src_params src;

/* CHECK INPUT ARGUMENTS */
  if ( (argc == 2 ) && STREQ(argv[1],"--h") ) print_parameter_file_format();
  if ( argc != 5 ) {
    fprintf(stderr,"USAGE: %s [parameter file] [background agrd file] [agrid zone-averaged values] [agrid output base-name]\n", argv[0]);
    fprintf(stderr,"Output basename used to write [_adapt/_wt][.out/.txt] files.\n");
    fprintf(stderr,"Enter '--h' for required parameter file values\n");
    exit(1);
  }
  sscanf(argv[1],"%s",paramf);
  sscanf(argv[2],"%s",agrdf_ref);
  sscanf(argv[3],"%s",agrdf_avg);
  sscanf(argv[4],"%s",agrid_bsnm_f);

// read parameters
  read_parameter_file(paramf,&src);
  fprintf(stderr,"\nSource parameters:\n");
  fprintf(stderr," lon: %.2f %.2f %.2f (%d pts)\n", src.max_lon, src.min_lon, src.inc_lon, src.nlon);
  fprintf(stderr," lat: %.2f %.2f %.2f (%d pts) \n", src.max_lat, src.min_lat, src.inc_lat, src.nlat);

// read smoothed seismicity and zone-averaged seismicity rates
  ntot=src.ntot;
  fp_ref=fopen(agrdf_ref,"rb");
  nread = fread(agrid_ref,4,ntot,fp_ref);
  fclose(fp_ref);
  fprintf(stderr,"Read %d (of %d) from file, %s\n", nread, ntot, agrdf_ref);
// zone-averaged rates
  fp_avg=fopen(agrdf_avg,"rb");
  fprintf(stderr,"Opened file, %s\n", agrdf_avg);
  nread = fread(agrid_avg,4,ntot,fp_avg);
  fclose(fp_avg);
  fprintf(stderr,"Read %d (of %d) from file, %s\n", nread, ntot, agrdf_avg);


// modify agrids - adaptive weighting and uniform weighting outputs
  wtavg=(float)WT_AVG;
  for(cnt=0; cnt<ntot; cnt++) {
    agrid_wt[cnt]=wtavg*agrid_avg[cnt]+(1.0-wtavg)*agrid_ref[cnt];
    if ( agrid_ref[cnt] >= agrid_avg[cnt] ) {
      agrid_adapt[cnt]=agrid_ref[cnt];
    }
    else {
      agrid_adapt[cnt]=agrid_wt[cnt];
    }
  }

// assign lon/lat values
  cnt=0;
//  need this ordering to write a-values to binary in the order expected by hazgrid codes
  for(cnt2=src.nlat-1; cnt2>=0; cnt2--) {
    for(cnt1=0; cnt1<src.nlon; cnt1++) {
      lat_arr[cnt]=src.min_lat+src.inc_lat*cnt2;
      lon_arr[cnt]=src.min_lon+src.inc_lon*cnt1;
      cnt++;
    }
  }

// WRITE RESULTS TO FILE
  write_results_to_file(agrid_bsnm_f,agrid_adapt,agrid_wt,lon_arr,lat_arr,ntot);

  return 0;
}
