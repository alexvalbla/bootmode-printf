#include "formatting.h"
#include "output_utils.h"
#include "print.h"





void initiate_ctxt(bm_output_ctxt *ctxt) {
        ctxt->flags = 0; //none set
        ctxt->lmods[0] = ctxt->lmods[1] = ctxt->lmods[2] = '\0';
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
                ++(ctxt->total_written);
        }
        ++(ctxt->total_needed);
}

void output_char_loop(bm_output_ctxt *ctxt, char c, size_t nb_times) {
        size_t nb_times_trunc = nb_times;
        if (ctxt->output_str) {
                // outputting to string
                size_t space_left = ctxt->limit_n - ctxt->total_written;
                if (ctxt->flags&FLAG_LIMIT && space_left < nb_times) {
                        nb_times_trunc = space_left;
                }
                for (size_t i = 0; i < nb_times_trunc; ++i) {
                        ctxt->output_str[ctxt->total_written+i] = c;
                }
        } else {
                // outputting to stdout
                for (size_t i = 0; i < nb_times_trunc; ++i) {
                        bm_putchar(c);
                }

        }
        ctxt->total_written += nb_times_trunc;
        ctxt->total_needed += nb_times;
}

void output_buffer(bm_output_ctxt *ctxt, char *buffer, size_t buff_sz) {
        size_t loop_length = buff_sz;
        if (ctxt->output_str) {
                // outputting to string
                size_t space_left = ctxt->limit_n - ctxt->total_written;
                if (ctxt->flags&FLAG_LIMIT && space_left < buff_sz) {
                        loop_length = space_left;
                }
                for (size_t i = 0; i < loop_length; ++i) {
                        ctxt->output_str[ctxt->total_written+i] = buffer[i];
                }
        } else {
                // outputting to stdout
                for (size_t i = 0; i < loop_length; ++i) {
                        bm_putchar(buffer[i]);
                }

        }
        ctxt->total_written += loop_length;
        ctxt->total_needed += buff_sz;
}
