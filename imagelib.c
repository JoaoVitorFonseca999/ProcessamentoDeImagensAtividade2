#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "imagelib.h"
#define PER_LINE 16

/*-------------------------------------------------------------------------
 * Image allocation and free routines
 *   nr = number of rows
 *   nc = number of columns
 *-------------------------------------------------------------------------*/
image img_alloc(int nr, int nc)
{
    return (image)malloc(nr * nc * sizeof(int));
}

int img_free(image Im)
{
    free(Im);
}

/*-------------------------------------------------------------------------
 * Define input and output image name
 * Params:
 *   name = image name file
 *   in = input image name
 *   out = output image name
 *   tp = image type (BW, GRAY, COLOR)
 
 *-------------------------------------------------------------------------*/
void img_name(char *name, char *in, char *out, int tp)
{
    char *ext[3] = {".pbm", ".pgm", ".ppm"};
    char *p = strstr(name, ext[tp - 1]);
    if (p)
        *p = 0;
    sprintf(in, "%s%s", name, ext[tp - 1]);
    sprintf(out, "%s-result%s", name, ext[tp - 1]);
}

/*-------------------------------------------------------------------------
 * Display image information
 *   name = file name image
 *   nr = number of rows
 *   nc = number os columns
 *   ml = max grayscale level
 *   tp = image type (1, 2 ou 3)
 *-------------------------------------------------------------------------*/
void img_info(char *name, int nr, int nc, int ml, int tp)
{
    printf("\nImage Informations:");
    printf("\n--------------------------\n");
    printf("Image file name.............: %s \n", name);
    printf("Image type..................: P%d\n", tp);
    printf("Number of rows..............: %d \n", nr);
    printf("Number of columns...........: %d \n", nc);
    printf("Max intensity level.........: %d \n\n", ml);
}

/*-------------------------------------------------------------------------
 * Error message
 *   str - string message control
 *   ... - parameters
 *-------------------------------------------------------------------------*/
void errormsg(char *str, ...)
{
    char format[255];
    va_list arg;
    va_start(arg, str);
    sprintf(format, "ERROR: %s\n\n", str);
    vprintf(format, arg);
    va_end(arg);
    exit(1);
}

/*-------------------------------------------------------------------------
 * Read pnm ascii image
 * Params (in):
 *   name = image file name
 *   tp = image type (BW, GRAY or COLOR)
 * Params (out):
 *   nr = number of rows
 *   nc = number of columns
 *   ml = max grayscale level
 * Returns:
 *   image vector
 *-------------------------------------------------------------------------+*/
image img_get(char *name, int *nr, int *nc, int *ml, int tp)
{
    char lines[100];
    image img;
    FILE *fimg;
    ERROR((fimg = fopen(name, "r")) == NULL, errormsg("File open error: <%s>", name));
    /*--- PNM = "P1" or "P2" or "P3" ---*/
    fgets(lines, 80, fimg);
    ERROR((lines[0] != 'P' || (lines[1] != tp + '0')), errormsg("File type image error: <%s>", name));
    /*--- Comment lines ---*/
    fgets(lines, 80, fimg);
    while (strchr(lines, '#'))
        fgets(lines, 80, fimg);
    sscanf(lines, "%d %d", nc, nr);
    if (tp != BW)
        fscanf(fimg, "%d", ml);
    else
        *ml = 1;
    ERROR(*nc == 0 || *nr == 0 || *ml == 0, errormsg("Image dimensions error: <%s>", name));
    img = img_alloc(*nr, *nc);
    ERROR(!img, errormsg("Image allocation error: %s\n\n img_get routine", name));
    for (int i = 0; i < (*nr) * (*nc); i++)
        if (tp != COLOR)
        {
            int k;
            fscanf(fimg, "%d", &k);
            ERROR(k > *ml, errormsg("Max pixel intensity in the image error: <%s>", name));
            img[i] = k;
        }
        else
        {
            int r, g, b;
            fscanf(fimg, "%d %d %d", &r, &g, &b);
            ERROR(r > *ml || r < 0 || g > *ml || g < 0 || b > *ml || b < 0,
                  errormsg("Max intensity of color in the image error: <%s>", name));
            img[i] = (r << 16) + (g << 8) + b;
        }
    fclose(fimg);
    img_info(name, *nr, *nc, *ml, tp);
    return img;
}

/*-------------------------------------------------------------------------
 * Write pnm image
 * Params:
 *   img = image
 *   name = image file name
 *   nr = number of rows
 *   nc = number of columns
 *   ml = max intensity level
 *   tp = image type (BW, GRAY or COLOR)
 *-------------------------------------------------------------------------*/
void img_put(image img, char *name, int nr, int nc, int ml, int tp)
{
    int count;
    FILE *fimg;
    ERROR((fimg = fopen(name, "wt")) == NULL, errormsg("Image creation error: <%s>", name));
    fprintf(fimg, "P%c\n", tp + '0');
    fputs(CREATOR, fimg);
    fprintf(fimg, "%d  %d\n", nc, nr);
    if (tp != BW)
        fprintf(fimg, "%d\n", ml);
    count = 0;
    for (int i = 0; i < nr * nc; i++)
    {
        if (tp != COLOR)
        {
            int x = img[i];
            fprintf(fimg, "%3d ", x);
        }
        else
        {
            int r = (img[i] >> 16) & 0xFF;
            int g = (img[i] >> 8) & 0xFF;
            int b = img[i] & 0xFF;
            fprintf(fimg, "%3d %3d %3d ", r, g, b);
        }
        count++;
        if (count > PER_LINE)
        {
            fprintf(fimg, "\n");
            count = 0;
        }
    }
    fclose(fimg);
}
