#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>
void extern_app_run(const char *app_path) ;
char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  char* load_program=(char*)malloc(sizeof(char)*(strlen(cmd)+10));
  strncpy(load_program,cmd,strlen(cmd)-1);
  load_program[strlen(cmd)-1]='\0';
  setenv("PATH","/bin/",0);

//解析参数：设置参数的最多个数为10,单个长度是20
  char* token=strtok(load_program," ");
  char* args[2];
  args[1]=NULL;
  args[0]=strtok(NULL," ");
  //解析完毕
//刷新界面
  /*SDL_Surface* clean=new SDL_Surface;
  clean->format->BytesPerPixel=4;
  clean->pixels=(uint8_t*)malloc(sizeof(uint8_t)*350*250);
  memset(clean->pixels,0,350*250*sizeof(uint8_t));
  SDL_UpdateRect(clean,0,0,350,250);
  free(clean->pixels);
  free(clean);*/
  execvp(token,(char* const*)args);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();
  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
