#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../lib/bmprint.h"
#define LEN 64
#define TEST_STR "this is a test string"


int main(int argc, char **argv) {

        printf("[TEST] running string tests\n");
        char output1[LEN];
        char output2[LEN];
        const char *output_fmt_1 = "reference:  %s\n";
        const char *output_fmt_2 = "bootmode:   %s\n\n\n";


        sprintf(output1, "%s", (char *)NULL);
        bootmode_sprintf(output2, "%s", (char *)NULL);
        if (strcmp(output1, output2)) {
                printf("\n\nerror:\n\n");
                printf(output_fmt_1, output1);
                printf(output_fmt_2, output2);
        }


        sprintf(output1, "%s", TEST_STR);
        bootmode_sprintf(output2, "%s", TEST_STR);
        if (strcmp(output1, output2)) {
                printf("\n\nerror:\n\n");
                printf(output_fmt_1, output1);
                printf(output_fmt_2, output2);
        }


        char c = 'z';
        const char *fmt = "inserting character right '%c' <- here";
        sprintf(output1, fmt, c);
        bootmode_sprintf(output2, fmt, c);
        if (strcmp(output1, output2)) {
                printf("\n\nerror:\n\n");
                printf(output_fmt_1, output1);
                printf(output_fmt_2, output2);
        }






        return 0;
}
