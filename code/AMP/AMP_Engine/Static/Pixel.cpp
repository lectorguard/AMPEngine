#include "pch.h"
#include "Pixel.h"

int amp::PIX::SizeY = 0;
int amp::PIX::SizeX = 0;
int amp::PIX::DIPX = 0;
int amp::PIX::DIPY = 0;
int amp::PIX::Density = 0;
float amp::PIX::DensityFactor = 1.0f;

int amp::PIX::x = 0;
int amp::PIX::y = 0;
int amp::PIX::hx = 0;
int amp::PIX::hhx = 0;
int amp::PIX::hy = 0;
int amp::PIX::hhy = 0;
int amp::PIX::tx = 0;
int amp::PIX::ty = 0;


void amp::PIX::init()
{
	assert(DIPX && DIPY && Density && SizeX && SizeY);
	x = SizeX;
	y = SizeY;
	hx = SizeX / 2.0f;
	hy = SizeY / 2.0f;
	hhx = SizeX / 4.0f;
	hhy = SizeY / 4.0f;
	tx = SizeX / 3.0f;
	ty = SizeY / 3.0f;
}

int amp::PIX::XP(float percent)
{
	return X() * percent;
}

int amp::PIX::YP(float percent)
{
	return Y() * percent;
}

int amp::PIX::X()
{
	return x;
}

int amp::PIX::Y()
{
	return y;
}

int amp::PIX::HX()
{
	return hx;
}

int amp::PIX::HHX()
{
	return hhx;
}

int amp::PIX::HY()
{
	return hy;
}

int amp::PIX::HHY()
{
	return hhy;
}

int amp::PIX::TX()
{
	return tx;
}

int amp::PIX::TY()
{
	return ty;
}
