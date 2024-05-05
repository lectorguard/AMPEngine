#pragma once

#include <EGL/egl.h>
#include <android\native_window.h>
#include "stdio.h"
#include "cassert"

namespace amp
{
	/// This is the AMP context 
	/// 
	/// This class opens and closes the window the application is running in.
	class Context
	{
	public:
		/// Opens the window
		///
		/// @param window The window received from the native android application
		void initDisplay(ANativeWindow* window);

		/// Swaps the buffer when frame is fully rendered
		void SwapBuffers();

		/// Closes the window
		void termDisplay();

		/// @return The display width of the current device in pixels
		int32_t getWidth();

		/// @return The display height of the current device in pixels
		int32_t getHeight();
	
		EGLDisplay display = EGL_NO_DISPLAY;
	private:
		void SetWidthAndHeight(int32_t width, int32_t height);
		EGLSurface surface = EGL_NO_SURFACE;
		EGLContext context = EGL_NO_CONTEXT;
		int32_t width = 0;
		int32_t height = 0;
		ANativeWindow* window = nullptr;
	};
	
}
