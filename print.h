/*
 * "$Id$"
 *
 *   Print plug-in header file for the GIMP.
 *
 *   Copyright 1997-2000 Michael Sweet (mike@easysw.com) and
 *	Robert Krawitz (rlk@alum.mit.edu)
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the Free
 *   Software Foundation; either version 2 of the License, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *   for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Revision History:
 *
 *   See ChangeLog
 */

/*
 *
 * This file must not include any gimp, glib, gtk, etc. headers.
 *
 * Eventually I intend to port this to GhostScript and/or CUPS.  The only
 * file that should have GIMP-specific code is print.c.  The rest of this
 * program should be completely generic.
 *
 * rlk 20000112
 */

/*
 * Include necessary header files...
 */
#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/*
 * Constants...
 */

#define OUTPUT_GRAY		0	/* Grayscale output */
#define OUTPUT_COLOR		1	/* Color output */
#define OUTPUT_GRAY_COLOR	2 	/* Grayscale output using color */

#define ORIENT_AUTO		-1	/* Best orientation */
#define ORIENT_PORTRAIT		0	/* Portrait orientation */
#define ORIENT_LANDSCAPE	1	/* Landscape orientation */

#define MAX_CARRIAGE_WIDTH	17
#define MAX_PLIST		100
#define SCALE_WIDTH		64
#define ENTRY_WIDTH		64
#define PREVIEW_SIZE		240	/* Assuming max media size of 24" A2 */

#define IMAGE_LINE_ART		0
#define IMAGE_SOLID_TONE	1
#define IMAGE_CONTINUOUS	2
#define IMAGE_MONOCHROME	3

#ifndef MIN
#  define MIN(a,b)		((a) < (b) ? (a) : (b))
#  define MAX(a,b)		((a) > (b) ? (a) : (b))
#endif /* !MIN */


/*
 * Printer driver control structure.  See "print.c" for the actual list...
 */

typedef struct
{
  unsigned short composite[256];
  unsigned short red[256];
  unsigned short green[256];
  unsigned short blue[256];
} lut_t;


typedef struct					/* Plug-in variables */
{
  char	output_to[255],		/* Name of file or command to print to */
	driver[64],		/* Name of printer "driver" */
	ppd_file[255];		/* PPD file */
  int	output_type;		/* Color or grayscale output */
  char	resolution[64],		/* Resolution */
	media_size[64],		/* Media size */
	media_type[64],		/* Media type */
	media_source[64],	/* Media source */
	ink_type[64];		/* Ink or cartridge */
  int	brightness;		/* Output brightness */
  float	scaling;		/* Scaling, percent of printable area */
  int	orientation,		/* Orientation - 0 = port., 1 = land.,
				   -1 = auto */
	left,			/* Offset from lower-lefthand corner, points */
	top;			/* ... */
  float gamma;                  /* Gamma */
  float gamma_red;
  float gamma_green;
  float gamma_blue;
  int   contrast,		/* Output Contrast */
	red,			/* Output red level */
	green,			/* Output green level */
	blue;			/* Output blue level */
  int	linear;			/* Linear density (mostly for testing!) */
  float	saturation;		/* Output saturation */
  float	density;		/* Maximum output density */
  int	image_type;		/* Image type (line art etc.) */
  lut_t lut;			/* Look-up table */
} vars_t;

typedef struct		/**** Printer List ****/
{
  int	active;			/* Do we know about this printer? */
  char	name[17];		/* Name of printer */
  vars_t v;
} plist_t;

typedef struct
{
  char name[32];
  unsigned width;
  unsigned length;
} papersize_t;

/*
 * Abstract data type for interfacing with the image creation program
 * (in this case, the Gimp).
 */
typedef void *Image;

extern void Image_init(Image image);
extern int Image_bpp(Image image);
extern int Image_width(Image image);
extern int Image_height(Image image);
extern const char *Image_get_pluginname(Image image);
extern void Image_get_col(Image image, unsigned char *data, int column);
extern void Image_get_row(Image image, unsigned char *data, int row);
extern void Image_progress_init(Image image);
extern void Image_note_progress(Image image, double current, double total);


typedef struct printer
{
  char	*long_name,			/* Long name for UI */
	*driver;			/* Short name for printrc file */
  int	model;				/* Model number */
  char	**(*parameters)(int model, char *ppd_file, char *name, int *count);
					/* Parameter names */
  void	(*media_size)(int model, char *ppd_file, char *media_size,
                      int *width, int *length);
  void	(*imageable_area)(int model, char *ppd_file, char *media_size,
                          int *left, int *right, int *bottom, int *top);
  /* Print function */
  void	(*print)(const struct printer *printer, int copies, FILE *prn,
		 Image image, unsigned char *cmap, vars_t *v);
  vars_t printvars;
} printer_t;

extern const printer_t printers[];
extern const int printer_count;

typedef void 	(*convert_t)(unsigned char *in, unsigned short *out, int width,
			     int bpp, unsigned char *cmap, vars_t *vars);

/*
 * Prototypes...
 */

extern void *	init_dither(int in_width, int out_width,
			    int horizontal_overdensity);
extern void 	dither_set_black_lower(void *vd, double);
extern void 	dither_set_black_upper(void *vd, double);
extern void	dither_set_black_levels(void *vd, double, double, double);
extern void 	dither_set_randomizers(void *vd, int, int, int, int);
extern void 	dither_set_ink_darkness(void *vd, double, double, double);
extern void 	dither_set_light_inks(void *vd, double, double, double);
extern void	dither_set_c_levels(void *vd, int nlevels, double *levels);
extern void	dither_set_lc_levels(void *vd, int nlevels, double *levels);
extern void	dither_set_m_levels(void *vd, int nlevels, double *levels);
extern void	dither_set_lm_levels(void *vd, int nlevels, double *levels);
extern void	dither_set_y_levels(void *vd, int nlevels, double *levels);
extern void	dither_set_ly_levels(void *vd, int nlevels, double *levels);
extern void	dither_set_k_levels(void *vd, int nlevels, double *levels);


extern void	free_dither(void *);
extern void	dither_black(unsigned short *, int, void *, unsigned char *);

extern void	dither_cmyk(unsigned short *, int, void *, unsigned char *,
			    unsigned char *, unsigned char *,
			    unsigned char *, unsigned char *,
			    unsigned char *, unsigned char *);

extern void	dither_black_n(unsigned short *, int, void *, unsigned char *,
			       int);

extern void	dither_cmyk_n(unsigned short *, int, void *, unsigned char *,
			      unsigned char *, unsigned char *,
			      unsigned char *, unsigned char *,
			      unsigned char *, unsigned char *, int);

extern void	gray_to_gray(unsigned char *, unsigned short *, int, int,
			     unsigned char *, vars_t *);
extern void	indexed_to_gray(unsigned char *, unsigned short *, int, int,
				unsigned char *, vars_t *);
extern void	indexed_to_rgb(unsigned char *, unsigned short *, int, int,
			       unsigned char *, vars_t *);
extern void	rgb_to_gray(unsigned char *, unsigned short *, int, int,
			    unsigned char *, vars_t *);
extern void	rgb_to_rgb(unsigned char *, unsigned short *, int, int,
			   unsigned char *, vars_t *);
extern void	gray_to_rgb(unsigned char *, unsigned short *, int, int,
			    unsigned char *, vars_t *);

extern void	compute_lut(float print_gamma, float app_gamma, vars_t *v);


extern void	default_media_size(int model, char *ppd_file, char *media_size,
		                   int *width, int *length);


extern char	**canon_parameters(int model, char *ppd_file, char *name,
		                   int *count);
extern void	canon_imageable_area(int model, char *ppd_file,
				     char *media_size, int *left, int *right,
				     int *bottom, int *top);
extern void	canon_print(const printer_t *printer, int copies, FILE *prn,
			    Image image, unsigned char *cmap, vars_t *v);


extern char	**escp2_parameters(int model, char *ppd_file, char *name,
		                   int *count);
extern void	escp2_imageable_area(int model, char *ppd_file,
				     char *media_size, int *left, int *right,
				     int *bottom, int *top);
extern void	escp2_print(const printer_t *printer, int copies, FILE *prn,
			    Image image, unsigned char *cmap, vars_t *v);


extern char	**pcl_parameters(int model, char *ppd_file, char *name,
		                 int *count);
extern void	pcl_imageable_area(int model, char *ppd_file, char *media_size,
		                   int *left, int *right, int *bottom,
				   int *top);
extern void	pcl_print(const printer_t *printer, int copies, FILE *prn,
			  Image image, unsigned char *cmap, vars_t *v);


extern char	**ps_parameters(int model, char *ppd_file, char *name,
		                int *count);
extern void	ps_media_size(int model, char *ppd_file, char *media_size,
		              int *width, int *length);
extern void	ps_imageable_area(int model, char *ppd_file, char *media_size,
		                  int *left, int *right, int *bottom,
				  int *top);
extern void	ps_print(const printer_t *printer, int copies, FILE *prn,
			 Image image, unsigned char *cmap, vars_t *v);

int		      known_papersizes(void);
const papersize_t    *get_papersizes(void);
const papersize_t    *get_papersize_by_name(const char *);

int		      known_printers(void);
const printer_t      *get_printers(void);
const printer_t	     *get_printer_by_index(int);
const printer_t      *get_printer_by_long_name(const char *);
const printer_t      *get_printer_by_driver(const char *);
int	              get_printer_index_by_driver(const char *);

void                  gtk_create_main_window(void);

/*
 * End of "$Id$".
 */
