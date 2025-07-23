#include "op.h"
#include "util.h"

static void op_sum(const void *in, void *out, int *count, MPI_Datatype *dtype) {
    switch (dtype->id) {
    case NANOMPI_DTYPE_INT: {
        const int *ptr_in = (int *)in;
        int *ptr_out = (int *)out;
        for (int i = *count; i >= 0; i--) {
            ptr_out[i] = ptr_out[i] + ptr_in[i];
        }
    } break;
    case NANOMPI_DTYPE_FLOAT: {
        const float *ptr_in = (float *)in;
        float *ptr_out = (float *)out;
        for (int i = *count; i >= 0; i--) {
            ptr_out[i] = ptr_out[i] + ptr_in[i];
        }
    } break;
    case NANOMPI_DTYPE_DOUBLE: {
        const double *ptr_in = (double *)in;
        double *ptr_out = (double *)out;
        for (int i = *count; i >= 0; i--) {
            ptr_out[i] = ptr_out[i] + ptr_in[i];
        }
    } break;
    case NANOMPI_DTYPE_CHAR:
    case NANOMPI_DTYPE_BYTE: {
        const char *ptr_in = (char *)in;
        char *ptr_out = (char *)out;
        for (int i = *count; i >= 0; i--) {
            ptr_out[i] = ptr_out[i] + ptr_in[i];
        }
    } break;
    default: {
        PRINT_STDOUT("dtype %d not supported yet\n", dtype->id);
    }
    }
}

MPI_Op nanompi_op_sum = {.op_type = NANOMPI_OP_SUM, .fn = op_sum};
