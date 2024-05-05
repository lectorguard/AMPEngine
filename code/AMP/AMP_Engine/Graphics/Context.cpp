#include "Static/Pixel.h"
#include "Context.h"
#include <android\native_window.h>


void amp::Context::initDisplay(ANativeWindow* window)
{
	assert(!display && "display was already initialised");

	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_DEPTH_SIZE,24,
		EGL_NONE
	};
	EGLint w, h, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	const EGLint GLESVersion[] = {
		EGL_CONTEXT_CLIENT_VERSION ,3,
		EGL_NONE
	};

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Hier wählt die Anwendung die Konfiguration, die sie wünscht. In diesem
	* Beispiel haben wir einen stark vereinfachten Auswahlprozess, in dem wir die
	* erste EGLConfig auswählen, die unseren Kriterien entspricht.
	OTHER Attributes for attribs:
		EGL_BIND_TO_TEXTURE_RGBA, EGL_CONFORMANT (Only GLES conformant Context),

	*/
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* "EGL_NATIVE_VISUAL_ID" ist ein Attribut der "EGLConfig", dessen
	* Akzeptierung durch "ANativeWindow_setBuffersGeometry()" garantiert ist.
	* Sobald wir eine "EGLConfig" ausgewählt haben, können wir die ANativeWindow-Puffer
	* gefahrlos mithilfe von  "EGL_NATIVE_VISUAL_ID" entsprechend neu konfigurieren.
	*/

	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
	

	assert(window != nullptr);
	/*
	* All buffers queued after this call will be associated with the dataSpace parameter specified
	*/
	ANativeWindow_setBuffersGeometry(window, 0, 0, format);


	surface = eglCreateWindowSurface(display, config, window, NULL);
	
	//Last element should be set to the GLES Version
	context = eglCreateContext(display, config, NULL, GLESVersion);
	
	
	//attaches an EGL rendering Context to EGL surface
	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {

		return;
	}
	//Can also return the EGL_CONFIG_ID
	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	this->display = display;
	this->context = context;
	this->surface = surface;
	SetWidthAndHeight(w, h);

}

void amp::Context::SwapBuffers()
{
	if (display == NULL) {
		// Keine Anzeige.
		return;
	}
	//post EGL surface color buffer to a native window 
	//Swap behavior(EGL_SWAP_BEHAVIOR) can be changed with the eglSurfaceAttrib
	eglSwapBuffers(display, surface);
}

void amp::Context::termDisplay()
{
	//self explaining
	if (display != EGL_NO_DISPLAY) {
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (context != EGL_NO_CONTEXT) {
			eglDestroyContext(display, context);
		}
		if (surface != EGL_NO_SURFACE) {
			eglDestroySurface(display, surface);
		}
		eglTerminate(display);
	}
	display = EGL_NO_DISPLAY;
	context = EGL_NO_CONTEXT;
	surface = EGL_NO_SURFACE;
}

int32_t amp::Context::getWidth()
{
	return width;
}

int32_t amp::Context::getHeight()
{
	return height;
}

void amp::Context::SetWidthAndHeight(int32_t width, int32_t height)
{
	this->width = width;
	this->height = height;
	PIX::setResX(width);
	PIX::setResY(height);
	PIX::init();
}
