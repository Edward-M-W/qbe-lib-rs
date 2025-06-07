#include "all.h"
#include "config.h"
#include <ctype.h>
#include <getopt.h>

Target T;

char debug['Z' + 1] = {
    ['P'] = 0, /* parsing */
    ['M'] = 0, /* memory optimization */
    ['N'] = 0, /* ssa construction */
    ['C'] = 0, /* copy elimination */
    ['F'] = 0, /* constant folding */
    ['A'] = 0, /* abi lowering */
    ['I'] = 0, /* instruction selection */
    ['L'] = 0, /* liveness */
    ['S'] = 0, /* spilling */
    ['R'] = 0, /* reg. allocation */
};

extern Target T_amd64_sysv;
extern Target T_amd64_apple;
extern Target T_arm64;
extern Target T_arm64_apple;
extern Target T_rv64;

static FILE *outf;
static int dbg;

static void data(Dat *d) {
  if (dbg)
    return;
  emitdat(d, outf);
  if (d->type == DEnd) {
    fputs("/* end data */\n\n", outf);
    freeall();
  }
}

static void func(Fn *fn) {
  uint n;

  if (dbg)
    fprintf(stderr, "**** Function %s ****", fn->name);
  if (debug['P']) {
    fprintf(stderr, "\n> After parsing:\n");
    printfn(fn, stderr);
  }
  T.abi0(fn);
  fillcfg(fn);
  filluse(fn);
  promote(fn);
  filluse(fn);
  ssa(fn);
  filluse(fn);
  ssacheck(fn);
  fillalias(fn);
  loadopt(fn);
  filluse(fn);
  fillalias(fn);
  coalesce(fn);
  filluse(fn);
  filldom(fn);
  ssacheck(fn);
  gvn(fn);
  fillcfg(fn);
  filluse(fn);
  filldom(fn);
  gcm(fn);
  filluse(fn);
  ssacheck(fn);
  T.abi1(fn);
  simpl(fn);
  fillcfg(fn);
  filluse(fn);
  T.isel(fn);
  fillcfg(fn);
  filllive(fn);
  fillloop(fn);
  fillcost(fn);
  spill(fn);
  rega(fn);
  fillcfg(fn);
  simpljmp(fn);
  fillcfg(fn);
  assert(fn->rpo[0] == fn->start);
  for (n = 0;; n++)
    if (n == fn->nblk - 1) {
      fn->rpo[n]->link = 0;
      break;
    } else
      fn->rpo[n]->link = fn->rpo[n + 1];
  if (!dbg) {
    T.emitfn(fn, outf);
    fprintf(outf, "/* end function %s */\n\n", fn->name);
  } else
    fprintf(stderr, "\n");
  freeall();
}

static void dbgfile(char *fn) { emitdbgfile(fn, outf); }

void qbeEmit(char *input, char *output) {
  FILE *inf;
  T = Deftgt;
  outf = stdout;

  inf = fopen(input, "r");
  if (!inf) {
    fprintf(stderr, "cannot open '%s'\n", input);
    exit(1);
  }
  outf = fopen(output, "w");
  if (!outf) {
    fprintf(stderr, "cannot open '%s'\n", output);
    exit(1);
  }

  parse(inf, input, dbgfile, data, func);
  fclose(inf);

  if (!dbg)
    T.emitfin(outf);

  fflush(outf);

  fclose(outf);
}
