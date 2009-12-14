#include "colors.h"

void hsl2rgb(struct hsl source_color, struct rgb * destination_color) {
    float temp1, temp2, r_temp, b_temp, g_temp, aux_h;
    if (source_color.lightness == 0) {
        destination_color->red = 0;
        destination_color->green = 0;
        destination_color->blue = 0;
    } else {
       if (source_color.lightness < 0.5)
           temp2 = source_color.lightness * (1.0+source_color.saturation);
        else if (source_color.lightness >= 0.5)
           temp2 = source_color.lightness + source_color.saturation - source_color.lightness*source_color.saturation;

        temp1 = 2.0 * source_color.lightness - temp2;
        aux_h = source_color.hue/360.0;
        
        r_temp = aux_h+1.0/3.0;
        g_temp = aux_h;
        b_temp = aux_h-1.0/3.0;
        
        if (r_temp < 0) r_temp += 1.0;
        else if (r_temp > 1) r_temp -= 1.0;
        if (g_temp < 0) g_temp += 1.0;
        else if (g_temp > 1) g_temp -= 1.0;
        if (b_temp < 0) b_temp += 1.0;
        else if (b_temp > 1) b_temp -= 1.0;
        
        if (6.0*r_temp < 1)
            destination_color->red = temp1 + (temp2-temp1)*6.0*r_temp;
        else if (2.0*r_temp < 1)
            destination_color->red = temp2;
        else if (3.0 * r_temp < 2)
            destination_color->red = temp1 + (temp2-temp1) * ((2.0/3.0)-r_temp)*6.0;
        else
            destination_color->red = temp1;

        if (6.0*g_temp < 1)
            destination_color->green = temp1 + (temp2-temp1)*6.0*g_temp;
        else if (2.0*g_temp < 1)
            destination_color->green = temp2;
        else if (3.0 * g_temp < 2)
            destination_color->green = temp1 + (temp2-temp1) * ((2.0/3.0)-g_temp)*6.0;
        else
            destination_color->green = temp1;
            
        if (6.0*b_temp < 1)
            destination_color->blue = temp1 + (temp2-temp1)*6.0*b_temp;
        else if (2.0*b_temp < 1)
            destination_color->blue = temp2;
        else if (3.0 * b_temp < 2)
            destination_color->blue = temp1 + (temp2-temp1) * ((2.0/3.0)-b_temp)*6.0;
        else
            destination_color->blue = temp1;
    }
}
