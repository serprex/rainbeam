#ifdef GLX
#include <GL/glx.h>
#include <sys/unistd.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#define ClientMessage SDL_QUIT
#define KeyPress SDL_KEYDOWN
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
	SDL_Window*dpy=SDL_CreateWindow(0,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,512,512,SDL_WINDOW_OPENGL);
	SDL_GL_CreateContext(dpy);
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
	int x=255,xv=2;
	float rv[64],ry[64]={};
	for(int i=0;i<64;i++)rv[i]=1+rand()/(float)RAND_MAX*3;
	for(;;){
		#ifdef GLX
		glXSwapBuffers(dpy,Wdo);
		XEvent ev;
		while(XPending(dpy)){
			XNextEvent(dpy,&ev);
		#else
		SDL_GL_SwapWindow(dpy);
		SDL_Event ev;
		while(SDL_PollEvent(&ev)){
		#endif
			switch(ev.type){
			case ClientMessage:return 0;
			case KeyPress:xv*=-1;
			}
		}
		x=(x+xv)&511;
		glRecti(x-2,0,x+2,512);
		glColor3ub(rand(),rand(),rand());
		glRecti(x-1,500,x+1,512);
		for(int i=0;i<64;i++){
			glColor3ub(rand(),rand(),rand());
			glRecti(i*8,0,i*8+8,ry[i]+=rv[i]);
			if(ry[i]>511){
				if(x>i*8&&x<i*8+8){
					glClear(GL_COLOR_BUFFER_BIT);
					goto restart;
				}
				ry[i]=0;
				rv[i]=rv[i]=1+rand()/(float)RAND_MAX*3;
			}
		}
		#ifdef GLX
		usleep(33000);
		#else
		SDL_Delay(33);
		#endif
	}
}