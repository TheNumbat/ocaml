/* Raising exceptions from C. */

#include "alloc.h"
#include "fail.h"
#include "gc.h"
#include "memory.h"
#include "mlvalues.h"
#include "roots.h"
#include "signals.h"
#include "stacks.h"

/* For minor_gc.c */
struct longjmp_buffer * external_raise;

/* The globals holding predefined exceptions */

extern int Out_of_memory, Sys_error, Failure, Invalid_argument;
extern int End_of_file, Division_by_zero, Not_found, Match_failure;

/* Exception raising */

extern void raise_caml_exception P((value bucket)) Noreturn;

void mlraise(v)
     value v;
{
  leave_blocking_section();
  raise_caml_exception(v);
}

void raise_constant(tag)
     value tag;
{
  value bucket;
  Push_roots (a, 1);
  a[0] = tag;
  bucket = alloc (1, 0);
  Field(bucket, 0) = a[0];
  Pop_roots ();
  mlraise(bucket);
}

void raise_with_arg(tag, arg)
     value tag;
     value arg;
{
  value bucket;
  Push_roots (a, 2);
  a[0] = tag;
  a[1] = arg;
  bucket = alloc (2, 0);
  Field(bucket, 0) = a[0];
  Field(bucket, 1) = a[1];
  Pop_roots ();
  mlraise(bucket);
}

void raise_with_string(tag, msg)
     value tag;
     char * msg;
{
  raise_with_arg(tag, copy_string(msg));
}

void failwith (msg)
     char * msg;
{
  raise_with_string((value) &Failure, msg);
}

void invalid_argument (msg)
     char * msg;
{
  raise_with_string((value) &Invalid_argument, msg);
}

/* To raise Out_of_memory, we can't use raise_constant,
   because it allocates and we're out of memory...
   We therefore build the bucket by hand.
   This works OK because the exception value for Out_of_memory is also
   statically allocated out of the heap. */

static struct {
  header_t hdr;
  value exn;
} out_of_memory_bucket;

void raise_out_of_memory()
{
  out_of_memory_bucket.hdr = Make_header(1, 0, White);
  out_of_memory_bucket.exn = (value) &Out_of_memory;
  mlraise((value) &(out_of_memory_bucket.exn));
}

void raise_sys_error(msg)
     value msg;
{
  raise_with_arg((value) &Sys_error, msg);
}

void raise_end_of_file()
{
  raise_constant((value) &End_of_file);
}

void raise_zero_divide()
{
  raise_constant((value) &Division_by_zero);
}

void raise_not_found()
{
  raise_constant((value) &Not_found);
}

