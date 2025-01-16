/**
 @file ImageMagick.hpp
 @date 02.04.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_ImageMagick_hpp
#define Chaos_ImageMagick_hpp

#include <cstdio>
#include <memory>

#if !defined(MAGICKCORE_QUANTUM_DEPTH)
#define MAGICKCORE_QUANTUM_DEPTH 16
#endif
#if !defined(MAGICKCORE_HDRI_ENABLE)
#define MAGICKCORE_HDRI_ENABLE 0
#endif

#ifdef __has_include /// < Check if __has_include is present
#if __has_include(<wand/MagickWand.h>)/// < Check for a standard path
#include <wand/MagickWand.h>
#elif __has_include(<MagickWand/MagickWand.h>) /// < Try with Alpine-specific
#include <MagickWand/MagickWand.h>
#endif
#else
#include <wand/MagickWand.h> /// < Cannot do anything else or except #ifdef (test ...) is okay ?
#endif

namespace chaos {
	class image_magick
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class resource_type : int
		{
			undfined = UndefinedResource,
			area = AreaResource,
			disk = DiskResource,
			file = FileResource,
			map = MapResource,
			memory = MemoryResource,
			thread = ThreadResource,
			time = TimeResource,
			throttle = ThrottleResource,
			width = WidthResource,
			height = HeightResource
		};

	private:
		struct wand_deleter
		{
			void operator()(MagickWand* raw_pointer) const;
		};
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static image_magick& initialize();
		static inline std::shared_ptr<MagickWand> make_wand()
		{
			return std::shared_ptr<MagickWand>(NewMagickWand(), image_magick::wand_deleter());
		}

		static inline bool set_resource_limit(resource_type type, std::size_t value)
		{
			return SetMagickResourceLimit(static_cast<ResourceType>(type), value);
		}

		static inline std::size_t get_resource_limit(resource_type type)
		{
			return GetMagickResourceLimit(static_cast<ResourceType>(type));
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	private:
		image_magick();
		~image_magick();
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
}

#endif
