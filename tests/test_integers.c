#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../lib/bmprint.h"
#define LEN 64


int main(int argc, char **argv) {

        printf("[TEST] running integer tests\n");
        char output1[LEN];
        char output2[LEN];
        const char *output_fmt_1 = "reference:  %s\n";
        const char *output_fmt_2 = "bootmode:   %s\n\n\n";
        const char *fmt;


        int a = 12345;
        fmt = "this is a %d test %d";
        sprintf(output1, fmt, a, -a);
        bootmode_sprintf(output2, fmt, a, -a);
        if (strcmp(output1, output2)) {
                printf("\n\nerror:\n\n");
                printf(output_fmt_1, output1);
                printf(output_fmt_2, output2);
        }


        unsigned int u = 12346;
        fmt = "this is a %u test %u";
        sprintf(output1, fmt, u, u);
        bootmode_sprintf(output2, fmt, u, u);
        if (strcmp(output1, output2)) {
                printf("\n\nerror:\n\n");
                printf(output_fmt_1, output1);
                printf(output_fmt_2, output2);
        }


        fmt = "this is a %# .10o test % .10o";
        sprintf(output1, fmt, u, u);
        bootmode_sprintf(output2, fmt, u, u);
        if (strcmp(output1, output2)) {
                printf("\n\nerror:\n\n");
                printf(output_fmt_1, output1);
                printf(output_fmt_2, output2);
        }

        fmt = "this is a %#.10x test %#.0x";
        sprintf(output1, fmt, u, 0);
        bootmode_sprintf(output2, fmt, u, 0);
        if (strcmp(output1, output2)) {
                printf("\n\nerror:\n\n");
                printf(output_fmt_1, output1);
                printf(output_fmt_2, output2);
        }






        return 0;
}
