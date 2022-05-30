#ifndef BM_OUTPUT_UTILS_H
#define BM_OUTPUT_UTILS_H


#include "print.h"


// adjustable if needed
#define MAX_PREC 500
#define MAX_FIELD_WIDTH 500


typedef struct {
        char *output_str; // when writing to string, e.g. sprintf, snprintf ...
        size_t limit_n; // limit on the number of characters to be written, e.g. in snprintf
        size_t total_needed; // to completely output what is specified in formatting string
        size_t total_written; // total number of characters written so far

        uint16_t flags;
        uint16_t field_width;
        uint16_t precision;

        char lmods[3]; // length modifiers
        char specifier;
        // char padding_char; // either (space) or '0'
} bm_output_ctxt;


void initiate_ctxt(bm_output_ctxt *ctxt, char *output_str);

void set_character_limit(bm_output_ctxt *ctxt, size_t n);

void output_char(bm_output_ctxt *ctxt, char c);

void output_char_loop(bm_output_ctxt *ctxt, char c, size_t nb_times);

void output_buffer(bm_output_ctxt *ctxt, const char *buffer, size_t buff_sz);

void append_nul(bm_output_ctxt *ctxt);


#endif // output_utils.h
