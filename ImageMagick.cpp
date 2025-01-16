/**
 @file ImageMagick.cpp
 @date 02.04.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#include "ImageMagick.hpp"

namespace chaos {
	void image_magick::wand_deleter::operator()(MagickWand* raw_pointer) const
	{
		raw_pointer = DestroyMagickWand(raw_pointer);
	}

	image_magick& image_magick::initialize()
	{
		static image_magick retval;
		return retval;
	}

	image_magick::image_magick()
	{
		MagickWandGenesis();
	}

	image_magick::~image_magick()
	{
		MagickWandTerminus();
	}
}
