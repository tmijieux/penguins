#include <d3v.h>
#include <d3v/d3v_internal.h>

#include <d3v/scene.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>


#define WINDOW_TITLE "Penguin"
#define WINDOW_POSITION_X 200
#define WINDOW_POSITION_Y 200

#define HEIGHT  700
#define WIDTH   700

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092


Display *display __internal;
Window win __internal;
GLXContext ctx __internal;
Colormap cmap __internal ;

typedef GLXContext(*glXCreateContextAttribsARBProc)
(Display *, GLXFBConfig, GLXContext, Bool, const int *);

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
static int is_extension_supported(const char *ext_list, const char *extension)
{
    const char *start;
    const char *where, *terminator;

    /* Extension names should not have spaces. */
    where = strchr(extension, ' ');
    if (where || *extension == '\0')
	return 0;

    /* It takes a bit of care to be fool-proof about parsing the
       OpenGL extensions string. Don't be fooled by sub-strings,
       etc. */
    for (start = ext_list;;) {
	where = strstr(start, extension);

	if (!where)
	    break;

	terminator = where + strlen(extension);

	if (where == start || *(where - 1) == ' ')
	    if (*terminator == ' ' || *terminator == '\0')
		return 1;

	start = terminator;
    }

    return 0;
}

static int ctx_error_occurred = 0;

static int ctx_error_handler(Display *dpy, XErrorEvent *ev)
{
    ctx_error_occurred = 1;
    return 0;
}

static int create_context(void)
{
    display = XOpenDisplay(NULL);

    if (!display) {
	fprintf(stderr, "Failed to open X display\n");
	return -1;
    }
    // Get a matching FB config
    static int visual_attribs[] = {
	GLX_X_RENDERABLE, True,
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE, GLX_RGBA_BIT,
	GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
	GLX_RED_SIZE, 8,
	GLX_GREEN_SIZE, 8,
	GLX_BLUE_SIZE, 8,
	GLX_ALPHA_SIZE, 8,
	GLX_DEPTH_SIZE, 24,
	GLX_STENCIL_SIZE, 8,
	GLX_DOUBLEBUFFER, True,
	//GLX_SAMPLE_BUFFERS  , 1,
	//GLX_SAMPLES         , 4,
	None
    };

    int glx_major, glx_minor;

    // FBConfigs were added in GLX version 1.3.
    if (!glXQueryVersion(display, &glx_major, &glx_minor) ||
	((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1)) {
	printf("Invalid GLX version");
	return -1;
    }

    int fbcount;
    GLXFBConfig *fbc = glXChooseFBConfig(display, DefaultScreen(display),
					 visual_attribs, &fbcount);
    if (!fbc) {
	fprintf(stderr, "Failed to retrieve a framebuffer config\n");
	return -1;
    }
    // Pick the FB config/visual with the most samples per pixel
    int best_fbc = -1, worst_fbc = -1,
	best_num_samp = -1, worst_num_samp = 999;

    for (int i = 0; i < fbcount; ++i) {
	XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
	if (vi) {
	    int samp_buf, samples;
	    glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS,
				 &samp_buf);
	    glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);
	    if (best_fbc < 0 || (samp_buf && (samples > best_num_samp)))
		best_fbc = i, best_num_samp = samples;
	    if (worst_fbc < 0 || (!samp_buf || samples < worst_num_samp))
		worst_fbc = i, worst_num_samp = samples;
	}
	XFree(vi);
    }

    GLXFBConfig bestFbc = fbc[best_fbc];

    // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
    XFree(fbc);

    // Get a visual
    XVisualInfo *vi = glXGetVisualFromFBConfig(display, bestFbc);

    XSetWindowAttributes swa;
    swa.colormap = cmap =
	XCreateColormap(display, RootWindow(display, vi->screen),
			vi->visual, AllocNone);

    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = KeyPressMask | ButtonPressMask |
	ButtonReleaseMask | Button1MotionMask | Button3MotionMask |
	StructureNotifyMask | ExposureMask;
    
    win = XCreateWindow(display, RootWindow(display, vi->screen),
			WINDOW_POSITION_X, WINDOW_POSITION_Y,
			WIDTH, HEIGHT, 0, vi->depth, InputOutput, vi->visual,
			CWBorderPixel | CWColormap | CWEventMask, &swa);
    if (!win) {
	fprintf(stderr, "Failed to create window.\n");
	return -1;
    }
    // Done with the visual info data
    XFree(vi);

    XStoreName(display, win, WINDOW_TITLE);

    XMapWindow(display, win);

    // Get the default screen's GLX extension list
    const char *glxExts =
	glXQueryExtensionsString(display, DefaultScreen(display));

    // NOTE: It is not necessary to create or make current to a context before
    // calling glXGetProcAddressARB
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
	glXGetProcAddressARB((const GLubyte *)
			     "glXCreateContextAttribsARB");

    ctx = 0;

    // Install an X error handler so the application won't exit if GL 3.0
    // context allocation fails.
    //
    // Note this error handler is global.
    // All display connections in all threads of a process use the same error
    // handler, so be sure to guard against other threads issuing X commands
    // while this code is running.
    ctx_error_occurred = 0;
    int (*old_handler) (Display *, XErrorEvent *) =
	XSetErrorHandler(&ctx_error_handler);

    // Check for the GLX_ARB_create_context extension string and the function.
    // If either is not present, use GLX 1.3 context creation method.
    if (!is_extension_supported(glxExts, "GLX_ARB_create_context") ||
	!glXCreateContextAttribsARB) {
	fprintf(stderr, "glXCreateContextAttribsARB() not found"
		" ... using old-style GLX context\n");
	ctx = glXCreateNewContext(display, bestFbc, GLX_RGBA_TYPE, 0, True);
    } else { // If it does, try to get a GL 3.0 context! 
	int context_attribs[] = {
	    GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
	    GLX_CONTEXT_MINOR_VERSION_ARB, 0,
	    //GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
	    None
	};

	ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True,
					 context_attribs);

	// Sync to ensure any errors generated are processed.
	XSync(display, False);
	if (!ctx_error_occurred && ctx) {
	    //  printf("Created GL 3.0 context\n");
	} else {
	    // Couldn't create GL 3.0 context.
	    // Fall back to old-style 2.x context.
	    
	    // When a context version below 3.0 is requested,
	    // implementations will return the newest context version
	    // compatible with OpenGL versions less than version 3.0.
	    
	    // GLX_CONTEXT_MAJOR_VERSION_ARB = 1
	    context_attribs[1] = 1;
	    // GLX_CONTEXT_MINOR_VERSION_ARB = 0
	    context_attribs[3] = 0;

	    ctx_error_occurred = 0;

	    fprintf(stderr, "Failed to create GL 3.0 context"
		    " ... using old-style GLX context\n");
	    ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True,
					     context_attribs);
	}
    }

    // Sync to ensure any errors generated are processed.
    XSync(display, False);

    // Restore the original error handler
    XSetErrorHandler(old_handler);

    if (ctx_error_occurred || !ctx) {
	fprintf(stderr, "Failed to create an OpenGL context\n");
	return -1;
    }

    /*
    // Verifying that context is a direct context
    if (!glXIsDirect(display, ctx)) {
	printf("Indirect GLX rendering context obtained\n");
    } else {
        printf("Direct GLX rendering context obtained\n");
    }
    */
    return 0;
}


/**
 *  Initialisation du module OpenGL.
 */
static void opengl_init(void)
{
    //Initialisation de l'etat d'OpenGL
    glXMakeCurrent(display, win, ctx);
    glClearColor(0.6, 0.9, 0.9, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1.0, 1.0, 1.0);

    glShadeModel(GL_SMOOTH);
    //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Parametrage du materiau
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glPolygonMode(GL_FRONT, GL_FILL);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static int main_loop_quit = 0;
static int user_need_redraw = 0;
static int need_redraw = 1;

void d3v_exit_main_loop(void)
{
    main_loop_quit = 1;
}

void d3v_post_redisplay(void)
{
    user_need_redraw = 1;
}

static int d3v_main_loop(void)
{
    XEvent xev;
    
    while (!main_loop_quit) {
	if (XPending(display) || !need_redraw) {
	    XNextEvent(display, &xev);
	    switch (xev.type) {
	    case KeyPress:
		d3v_key(xev.xkey.keycode, xev.xkey.x, xev.xkey.y);
		d3v_special_input(xev.xkey.keycode, xev.xkey.x, xev.xkey.y);
		break;

	    case ButtonPress:
	    case ButtonRelease:
		d3v_button(xev.xbutton.button, xev.type,
			   xev.xbutton.x, xev.xbutton.y);
		break;
	    
	    case MotionNotify:
		d3v_mouse_motion(xev.xmotion.x, xev.xmotion.y);
		break;
	    
	    case ConfigureNotify:
		d3v_reshape(xev.xconfigure.width,
			    xev.xconfigure.height);
		break;

	    case DestroyNotify:
		main_loop_quit = 1;
		break;
	    
	    default:
		break;
	    };
	    need_redraw = 1;
	} else {
	    if (!need_redraw)
		continue;
	    d3v_scene_draw();
	    if (user_need_redraw) {
		need_redraw = 1;
		user_need_redraw = 0;
	    } else {
		need_redraw = 0;
	    }
	}
    }
    return 0;
}

// PUBLIC

int d3v_init(int object_count_clue)
{
    create_context();
    opengl_init();
    d3v_scene_init(object_count_clue);
    return 0;
}

int d3v_start(vec3 *pos)
{
    glXMakeCurrent(display, win, ctx);
    d3v_scene_start(pos);
    d3v_main_loop();
    return 0;
}

int d3v_exit(void)
{
    d3v_scene_exit();
    
    XDestroyWindow(display, win);
    XFreeColormap(display, cmap);
    XCloseDisplay(display);

    return 0;
}
