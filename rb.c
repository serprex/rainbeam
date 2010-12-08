#ifdef GLX
#include <GL/glx.h>
#include <sys/unistd.h>
#define KEYSYM XKeycodeToKeysym(dpy,ev.xkey.keycode,0)
#define EV(y) ev.x##y
#else
#include <SDL.h>
#include <SDL_opengl.h>
#define KEYSYM ev.key.keysym.sym
#define ClientMessage SDL_QUIT
#define EV(y) ev.y
#endif
#include <stdlib.h>
#include <time.h>
int main(int argc,char**argv){
	#ifdef SDL
	if(SDL_Init(SDL_INIT_VIDEO)==-1){
		fputs(SDL_GetError(),stderr);
		return 1;
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_Surface*dpy=SDL_SetVideoMode(512,512,0,SDL_OPENGL);
	#else
	Display*dpy=XOpenDisplay(0);
	XVisualInfo*vi=glXChooseVisual(dpy,DefaultScreen(dpy),(int[]){GLX_RGBA,GLX_DOUBLEBUFFER,None});
	Window Wdo=XCreateWindow(dpy,RootWindow(dpy,vi->screen),0,0,512,512,0,vi->depth,InputOutput,vi->visual,CWColormap|CWEventMask,(XSetWindowAttributes[]){{.colormap=XCreateColormap(dpy,RootWindow(dpy,vi->screen),vi->visual,AllocNone),.event_mask=KeyPressMask}});
	XSetWMProtocols(dpy,Wdo,(Atom[]){XInternAtom(dpy,"WM_DELETE_WINDOW",False)},1);
	XMapWindow(dpy,Wdo);
	glXMakeCurrent(dpy,Wdo,glXCreateContext(dpy,vi,0,GL_TRUE));
	#endif
	srand(time(0));
	glOrtho(0,512,512,0,1,-1);
	restart:;
	unsigned short x=255,ry[64]={},rv[64];
	for(int i=0;i<64;i++)rv[i]=1+rand()%5;
	char xv=1,v=3;
	for(;;){
		#ifdef GLX
		glXSwapBuffers(dpy,Wdo);
		XEvent ev;
		while(XPending(dpy)){
			KeySym ks;
			XNextEvent(dpy,&ev);
		#else
		SDL_GL_SwapBuffers();
		SDL_Event ev;
		while(SDL_PollEvent(&ev)){
			SDLKey ks;
		#endif
			switch(ev.type){
			case ClientMessage:return 0;
			case KeyPress:
				ks=KEYSYM;
				v+=ks=='c'&&v<6;
				v-=ks=='x'&&v;
				v*=ks!='v';
				if(ks=='z')xv*=-1;
			}
		}
		x+=xv*v;
		glRecti(x-2,508,x+2,512);
		glColor3ub(rand(),rand(),rand());
		glRecti(x-1,509,x+1,511);
		for(int i=0;i<64;i++){
			glColor3ub(rand(),rand(),rand());
			glRecti(i*8,0,i*8+8,ry[i]+=rv[i]);
			if(ry[i]>511){
				if(x>i*8&&x<i*8+8){
					glClear(GL_COLOR_BUFFER_BIT);
					goto restart;
				}
				ry[i]=0;
				rv[i]=rand()&7;
			}
		}
		#ifdef GLX
		usleep(30000);
		#else
		SDL_Delay(30);
		#endif
	}
}