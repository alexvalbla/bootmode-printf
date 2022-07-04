#include "formatting.h"
#include "output_utils.h"
#include "print.h"





void initiate_ctxt(bm_output_ctxt *ctxt, char *output_str) {
        ctxt->output_str = output_str; // if NULL -> outputting to stdout
        ctxt->total_needed = 0;
        ctxt->total_written = 0;
        ctxt->flags = 0;
}

void set_character_limit(bm_output_ctxt *ctxt, size_t n) {
        ctxt->limit_n = n; // irrelevant if outputting to stdout
        ctxt->flags |= FLAG_LIMIT;
}

void output_char(bm_output_ctxt *ctxt, char c) {
        if (ctxt->output_str) {
                // outputting to string
                if (!(ctxt->flags&FLAG_LIMIT) || ctxt->total_written < ctxt->limit_n) {
                        ctxt->output_str[ctxt->total_written++] = c;
                }
        } else {
                // outputting to stdout
                bm_putchar(c);
        }
        ++(ctxt->total_needed);
}

void output_char_loop(bm_output_ctxt *ctxt, char c, size_t nb_times) {
        for (size_t i = 0; i < nb_times; i++) {
                output_char(ctxt, c);
        }
}

void output_buffer(bm_output_ctxt *ctxt, const char *buffer, size_t buff_sz) {
        for (size_t i = 0; i < buff_sz; i++) {
                output_char(ctxt, buffer[i]);
        }
}

void append_nul(bm_output_ctxt *ctxt) {
        if (ctxt->output_str) {
                // only if writting to a string
                if (!(ctxt->flags&FLAG_LIMIT)) {
                        // append if there is no limit
                        ctxt->output_str[ctxt->total_written] = '\0';
                } else if (ctxt->limit_n > 0) {
                        if (ctxt->total_written < ctxt->limit_n) {
                                // there is still room for terminating '\0'
                                ctxt->output_str[ctxt->total_written] = '\0';
                        } else {
                                // overwrite last written character
                                ctxt->output_str[ctxt->total_written-1] = '\0';
                        }
                } // else: limit_n == 0, nothing can be written
        }
}
