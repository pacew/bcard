#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <cairo.h>
#include <cairo-pdf.h>
#include <pango/pangocairo.h>

/*
 * standard business card size 2 by 3.5 inches
 * ten cards on on 8.5 by 11 paper; 2 cards on each row
 *
 * going across: 0.75 + 2 * 3.5 + 0.75 = 8.5
 * going down: 0.5 + 5 * 2 + 0.5 = 11
 *
 * native units are points (72/inch) for postscript, pdf and cairo
 */

#define PAPER_WIDTH (8.5 * 72)
#define PAPER_HEIGHT (11 * 72)

#define CARD_WIDTH (3.5 * 72)
#define CARD_HEIGHT (2 * 72)

#define LEFT_MARGIN (0.75 * 72)
#define TOP_MARGIN (0.5 * 72)

void init_pdf (char *outname);
void draw_card (void);

struct font {
	char *fontname;
	char *pango_fontname;
	PangoFontDescription *desc;
	double points;
};

struct font *setup_font (char *fontname, double points);
void setup_text (struct font *font, char *str, double *dxp, double *dyp);
void draw_text (double x, double y);

void set_gray (double pct);
void set_black (void);


void setup_fonts (void);
struct font *name_font;
struct font *main_font;
struct font *mono_font;


void
usage (void)
{
	fprintf (stderr, "usage: bcard\n");
	exit (1);
}

cairo_surface_t *surface;
cairo_t *cr;
PangoLayout *layout;

cairo_surface_t *code_surface;
cairo_pattern_t *code_pattern;

int
main (int argc, char **argv)
{
	int c;
	int row, col;
	double x, y;
	char *outname;
		
	outname = "cards.pdf";

	while ((c = getopt (argc, argv, "")) != EOF) {
		switch (c) {
		default:
			usage ();
		}
	}

	if (optind != argc)
		usage ();

	init_pdf (outname);
	setup_fonts ();

	code_surface
		= cairo_image_surface_create_from_png ("code-med-high.png");
	code_pattern = cairo_pattern_create_for_surface (code_surface);

	for (row = 0; row < 5; row++) {
		for (col = 0; col < 2; col++) {
			cairo_save (cr);

			x = LEFT_MARGIN + col * CARD_WIDTH;
			y = TOP_MARGIN + row * CARD_HEIGHT;
			cairo_translate (cr, x, y);
			draw_card ();

			cairo_restore (cr);
		}
	}

	cairo_destroy(cr);
	cairo_surface_flush(surface);
	cairo_surface_destroy(surface);
	printf ("evince %s\n", outname);
	return (0);
}

void
setup_fonts (void)
{
	name_font = setup_font ("Lucida Sans Bold", 12);
	main_font = setup_font ("Lucida Sans", 8);
	mono_font = setup_font ("Courier Bold", 11.4);
}

void
draw_card (void)
{
	double curx, cury;
	double dx, dy;
	double scale;

	set_gray (80);

	cairo_rectangle (cr, 0, 0, CARD_WIDTH, CARD_HEIGHT);
	cairo_stroke (cr);

	curx = 12;
	cury = 15;

	set_gray (0);
	setup_text (name_font, "Alex Willisson", &dx, &dy);

	if (0) {
		/* centered */
		draw_text ((CARD_WIDTH - dx) / 2, cury);
	} else {
		draw_text (curx, cury);
	}


	cury += 22;
	setup_text (mono_font, "atw@mit.edu", &dx, &dy);
	draw_text (curx, cury);

	set_gray (30);
	cury += 50;
	setup_text (main_font, "3 Ames Street", &dx, &dy);
	draw_text (curx, cury);

	cury += 15;
	setup_text (main_font, "Cambridge, MA 02139", &dx, &dy);
	draw_text (curx, cury);

	set_gray (0);
	cury = CARD_HEIGHT - 25;
	setup_text (mono_font, "http://alex.willisson.org", &dx, &dy);
	draw_text (curx, cury);



	
	curx = 150;
	cury = 10;
	scale = .1;

	cairo_save (cr);

	cairo_translate (cr, curx, cury);

	cairo_scale (cr, scale, scale);
	cairo_rectangle (cr,
			 0, 0,
			 cairo_image_surface_get_width (code_surface),
			 cairo_image_surface_get_height (code_surface));
	cairo_set_source (cr, code_pattern);
	cairo_fill (cr);
	cairo_restore (cr);



}

void
init_pdf (char *outname)
{
	surface = cairo_pdf_surface_create(outname, PAPER_WIDTH, PAPER_HEIGHT);
	cr = cairo_create(surface);
	layout = pango_cairo_create_layout (cr);
}


struct font *
setup_font (char *fontname, double points)
{
	struct font *fp;
	char buf[1000];

	sprintf (buf, "%s %g", fontname, points);

	fp = calloc (1, sizeof *fp);
	fp->fontname = strdup (fontname);
	fp->pango_fontname = strdup (buf);
	fp->points = points;

	fp->desc = pango_font_description_from_string (fp->pango_fontname);
	if (fp == NULL) {
		fprintf (stderr, "error setting up %s\n", fp->pango_fontname);
		exit (1);
	}
	return (fp);
}

void
set_font (struct font *fp)
{
	pango_layout_set_font_description (layout, fp->desc);
}

void
setup_text (struct font *font, char *str, double *dxp, double *dyp)
{
	int pango_width, pango_height;
	set_font (font);
	pango_layout_set_markup (layout, str, strlen (str));
	pango_layout_get_size (layout, &pango_width, &pango_height);
	*dxp = (double)pango_width / PANGO_SCALE;
	*dyp = (double)pango_height / PANGO_SCALE;
}

void
draw_text (double x, double y)
{
	cairo_move_to (cr, x, y);
	pango_cairo_show_layout (cr, layout);
}

/* 0 = black; 100 = white */
void
set_gray (double pct)
{
	cairo_set_source_rgb(cr, pct / 100.0, pct / 100.0, pct / 100.0);
}

void
set_black (void)
{
	set_gray (0);
}

