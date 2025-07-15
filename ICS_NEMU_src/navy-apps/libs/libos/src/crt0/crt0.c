#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc;
  char** argv,**envp;
  argc=(int)(*(args));
  argv=(char**)(args+1);
  args+=argc+1;
  envp=(char**)args;
  environ=envp;
  exit(main(argc, argv, envp));
  assert(0);
}
