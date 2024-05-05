#pragma once
#include <cassert>
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

namespace amp
{	
	/// This is the AMP layout system for UI
	/// 
	/// PIX allows the creation of device independent UI 
	/// You have two choices inside PIX 
	/// First, relative locations and scales dependent on the devices screen width and height
	/// Second, real world location and scale unit in centimeter
	/// The second one is very important for creating buttons that are on all devices big enough to touch with a finger
	/// @note You can use PIX in every situation, where you have to specify a size or a location in pixels
	struct PIX {

		/// The density factor of the current device
		///
		/// @note Here you can find an overview of the different density factors https://developer.android.com/training/multiscreen/screendensities
		/// @return The factor is between 1.0f and 4.0f
		static float getDensFactor() {
			return DensityFactor;
		}

		/// Calculates actual pixels from density independent pixels
		///
		/// @param densityIndependentPixel Each density independent pixel has a size of 0.015875cm, btw. 160 densityIndependentPixels have a size of one inch(2,54cm)
		/// @return The densityIndependentPixels converted the pixels for the current device with the same length
		static int DP(int densityIndependentPixel) {
			return densityIndependentPixel * DensityFactor;
		}

		/// Calculates the x and y pixels from a percentage vector
		///
		/// @param vecInPercent The percentage vector. E.g: vec2(0.5f,0.5f) would return the center location of the device in pixels
		/// @return The percentage location in pixels
		static glm::vec2 P(glm::vec2 vecInPercent) {
			return glm::vec2(XP(vecInPercent.x), YP(vecInPercent.y));
		}

		/// Calculates the current device pixels in centimeter
		///
		/// @param pixels The amount of pixels you want to have in centimeter
		/// @return The size of the pixels in centimeter
		static float GET(int pixels){
			return (pixels / (float)Density) * 2.54f;
		}

		/// Calculates the current device pixels in centimeter from a vector
		///
		/// @param pixelVector The amount of pixels you want to have in centimeter from x and y
		/// @return The size of the x and y pixels in centimeter
		static glm::vec2 GET(glm::vec2 pixelVector) {
			return glm::vec2(GET(pixelVector.x), GET(pixelVector.y));
		}

		/// Calculates real world unit cm into current device pixels
		///
		/// @param cm The size in cm you want to have in pixels
		/// @return The current devices pixels from a cm size
		static int CM(float cm) {
			return DP((cm / 2.54f) * 160.0f);
		}

		/// Calculates pixels from the width of your screen in percent (1.0f would result in the screen width in pixels)
		///
		/// @param percent The percentage of the width you want to have in pixels. E.g: passing 0.5f would result at the x position over the home button(center)
		/// @return The pixels of your current device screen from a width percentage
		static int XP(float percent);

		/// Calculates pixels from the height of your screen in percent (1.0f would result in the screen height in pixels)
		///
		/// @param percent The percentage of the height you want to have in pixels. E.g: passing 0.5f would result at the height center of your screen 
		/// @return The pixels of your current device screen from a height percentage
		static int YP(float percent);

		/// @return The screen width in pixels
		static int X();

		/// @return The screen height in pixels
		static int Y();

		/// @return The half screen width in pixels (1/2 width)
		static int HX();
		
		/// @return The half half screen width in pixels (1/4 width)
		static int HHX();

		/// @return The half screen height in pixels (1/2 height)
		static int HY();

		/// @return The half half screen height in pixels (1/4 height)
		static int HHY();

		/// @return One third screen width in pixels (1/3 width)
		static int TX();

		/// @return One third screen height in pixels (1/3 height)
		static int TY();
	
	
	private:
		static void setDIPX(int x) {
			DIPX = x; 
		};
		static void setDIPY(int y) {
			DIPY = y; 
		};
		static void setResX(int x) {
			SizeX = x;
		}
		static void setResY(int y) {
			SizeY = y;
		}
		static void setDensity(int dens) {
			Density = dens; 
			DensityFactor = Density / 160.0f;
		};
		static void init();
		//IN PIXELS
		//Density Independent Pixels Y
		static int DIPY;
		//Density Independent Pixels X
		static int DIPX;
		//Actual screen resolution X
		static int SizeX;
		//Actual screen resolution Y
		static int SizeY;
		//Density
		static int Density;
		//Density Factor
		static float DensityFactor;


		// Following Values are initialized after init is called (from Context)
		static int x;
		static int y;
		static int hx;
		static int hhx;
		static int hy;
		static int hhy;
		static int tx;
		static int ty;
	
		friend class AMP_Engine;
		friend class Context;
	};
}
