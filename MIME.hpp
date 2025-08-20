//
//  MIME.hpp
//
//  Created by Daniil A Megrabyan on 14.02.17.
//  Copyright © 2017 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_MIME_hpp
#define Chaos_MIME_hpp

#include <string>
#include <string_view>
#include <ostream>

#include "BiMap.hpp"

namespace chaos {
	/** @brief
	 * Format: type "/" [tree "."] subtype ["+" suffix] *[";" parameter]
	 * Types: application, audio, example, font, image, message, model, multipart, text and video.
	 **/
	class mime
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class media_type
		{
			unknown,
			application,
			audio,
			example,
			font,
			image,
			message,
			model,
			multipart,
			text,
			book, /// < Custom
			post, /// < Custom
			video
		};

		enum class buffer_type
		{
			filename,
			filedata
		};
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
	/** @} */

	/** @name Constants */
	/** @{ */
	public:
		static const bimap<std::string, mime::media_type> type_map;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		mime();
		mime(const std::string& value);
		mime(const mime& origin);
		mime(mime&& origin);
		~mime() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		mime::media_type _type;
		std::string _tree;
		std::string _sub_type;
		std::string _suffix;
		std::string _parameter;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		mime& operator=(const mime& rhs);
		mime& operator=(mime&& rhs);
		bool operator==(const mime& rhs) const;
		bool operator==(const std::string& value) const;
		operator std::string() const;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		bool parse(const std::string& input);
		bool parse(std::ifstream& stream);
		bool parse(std::string_view buffer, buffer_type type);
		void reset();
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void type(mime::media_type value);
		void type(const std::string& value);
		void tree(const std::string& value);
		void sub_type(const std::string& value); /// < can hold (.)
		void suffix(const std::string& value);
		void parameter(const std::string& value);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const mime::media_type& type() const;
		const std::string& tree() const;
		const std::string& sub_type() const;
		const std::string& suffix() const;
		const std::string& parameter() const;
		std::string content_type() const;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool kind(mime::media_type type, const std::string& sub_type) const;
	/** @} */
	};
}
#endif
