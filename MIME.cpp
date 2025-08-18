//
//  MIME.cpp
//
//  Created by Daniil A Megrabyan on 14.02.17.
//  Copyright © 2017 Cadabra Systems. All rights reserved.
//

#include "MIME.hpp"

#include <magic.h>
#include "String.hpp"

namespace chaos {
	const bimap<std::string, mime::media_type> mime::type_map = {
		{
			{"unknown", mime::media_type::unknown},
			{"application", mime::media_type::application},
			{"audio", mime::media_type::audio},
			{"example", mime::media_type::example},
			{"font", mime::media_type::font},
			{"image", mime::media_type::image},
			{"message", mime::media_type::message},
			{"model", mime::media_type::model},
			{"multipart", mime::media_type::multipart},
			{"text", mime::media_type::text},
			{"video", mime::media_type::video}
		},
		mime::media_type::unknown
	};

	mime::mime()
	:
		_type(mime::media_type::unknown)
	{

	}

	/**
	 * Okay. So it seems if you want to specify a Content-Type header with parameters, simply do it like this:
	 * Content-Type: multipart/related; foo=bar; something=else
	 * ... and a folded version of the same header would look like this:
	 * Content-Type: multipart/related;
	 *    foo=bar;
	 *    something=else
	 */
	mime::mime(const std::string& value)
	:
		_type(mime::media_type::unknown)
	{
		parse(value);
	}

	mime::mime(const mime& origin)
	:
		_type(origin._type),
		_tree(origin._tree),
		_sub_type(origin._sub_type),
		_suffix(origin._suffix),
		_parameter(origin._parameter)
	{

	}

	mime::mime(mime&& origin)
	:
		_type(std::move(origin._type)),
		_tree(std::move(origin._tree)),
		_sub_type(std::move(origin._sub_type)),
		_suffix(std::move(origin._suffix)),
		_parameter(std::move(origin._parameter))
	{

	}

	mime& mime::operator=(const mime& rhs)
	{
		_type = rhs._type;
		_tree = rhs._tree;
		_sub_type = rhs._sub_type;
		_suffix = rhs._suffix;
		_parameter = rhs._parameter;

		return *this;
	}

	mime& mime::operator=(mime&& rhs)
	{
		_type = std::move(rhs._type);
		_tree = std::move(rhs._tree);
		_sub_type = std::move(rhs._sub_type);
		_suffix = std::move(rhs._suffix);
		_parameter = std::move(rhs._parameter);

		return *this;
	}

	bool mime::operator==(const mime& rhs) const
	{
		if (
			_type != rhs._type
			||
			_tree != rhs._tree
			||
			_sub_type != rhs._sub_type
			||
			_suffix != rhs._suffix
			||
			_parameter != rhs._parameter
		) {
			return false;
		}
		return true;
	}

	bool mime::operator==(const std::string& value) const
	{
		return *this == mime(value);
	}

	mime::operator std::string() const
	{
		/// @brief type "/" [tree "."] subtype ["+" suffix] *[";" parameter]
		/// @brief suffixes = +xml, +json, +ber, +der, +fastinfoset, +wbxml, +zip, +gzip
		std::string retval;
		if (!_tree.empty()) {
			retval.append(_tree + ".");
		}
		retval.append(_sub_type);
		if (!_suffix.empty()) {
			retval.append("+" + _suffix);
		}

		return mime::type_map.at(_type, true) + "/" + retval;
	}

	bool mime::parse(const std::string& input)
	{
		std::string value(input);
		value.erase(remove_if(value.begin(), value.end(), isspace), value.end());

		const std::size_t slash_offset(value.find_first_of('/'));
		if (slash_offset == std::string::npos) {
			return false;
		}
		_type = mime::type_map.at(value.substr(0, slash_offset), true);
		if (mime::media_type::unknown == _type) {
			return false;
		}

		const std::size_t dot_offset(value.find_first_of('.', slash_offset));
		if (dot_offset != std::string::npos) {
			_tree = value.substr(slash_offset + 1, dot_offset - slash_offset + 1);
		}

		const std::size_t plus_offset(value.find_first_of('+', dot_offset != std::string::npos ? dot_offset : slash_offset));
		if (plus_offset != std::string::npos) {
			_suffix = value.substr(plus_offset + 1);
		}

		const std::size_t semicolon_offset(value.find_first_of(';', slash_offset));

		if (dot_offset == std::string::npos && plus_offset == std::string::npos) {
			_sub_type = value.substr(slash_offset + 1, slash_offset != std::string::npos ? semicolon_offset - slash_offset - 1: std::string::npos);
			return true;
		} else if (plus_offset == std::string::npos) {
			_sub_type = value.substr(dot_offset + 1, slash_offset != std::string::npos ? semicolon_offset - slash_offset - 1: std::string::npos);
			return true;
		} else if (dot_offset == std::string::npos) {
			_sub_type = value.substr(slash_offset + 1, plus_offset - slash_offset - 1);
			return true;
		}
		return false;
	}

	bool mime::parse(std::ifstream& stream)
	{
		/// @todo
		return false;
	}

	bool mime::parse(std::string_view buffer, buffer_type type)
	{
		/// @todo implement as thread safe singleton wrapper
		magic_t magic = magic_open(MAGIC_MIME_TYPE);
		if (nullptr == magic) {
			return false;
		}

		if (0 != magic_load(magic, nullptr)) {
			magic_close(magic);
			return false;
		}

		std::string retval
		(
			type == buffer_type::filedata
			? magic_buffer(magic, buffer.data(), buffer.length())
			: magic_file(magic, buffer.data())
		);
		magic_close(magic);
		return parse(retval);
	}

	void mime::reset()
	{
		_type = media_type::unknown;
		_tree.clear();
		_sub_type.clear();
		_suffix.clear();
		_parameter.clear();
	}

	void mime::type(mime::media_type value)
	{
		_type = value;
	}

	void mime::type(const std::string& value)
	{
		_type = mime::type_map.at(value, true);
	}

	void mime::tree(const std::string& value)
	{
		_tree = value;
	}

	void mime::sub_type(const std::string& value)
	{
		/// @todo parse value
		_sub_type = value;
	}

	void mime::suffix(const std::string& value)
	{
		_suffix = value;
	}

	void mime::parameter(const std::string& value)
	{
		_parameter = value;
	}

	const mime::media_type& mime::type() const
	{
		return _type;
	}

	const std::string& mime::tree() const
	{
		return _tree;
	}

	const std::string& mime::sub_type() const
	{
		return _sub_type;
	}

	const std::string& mime::suffix() const
	{
		return _suffix;
	}

	const std::string& mime::parameter() const
	{
		return _parameter;
	}

	std::string mime::content_type() const
	{
		/// @brief type "/" [tree "."] subtype ["+" suffix] *[";" parameter]
		/// @brief suffixes = +xml, +json, +ber, +der, +fastinfoset, +wbxml, +zip, +gzip

		std::string retval(mime::type_map.at(_type, true) + "/");
		if (!_tree.empty()) {
			retval += "." + _tree;
		}
		if (!_sub_type.empty()) {
			retval += _sub_type;
		}
		if (!_suffix.empty()) {
			retval += "+" + _suffix;
		}

		return retval;
	}

	bool mime::kind(mime::media_type type, const std::string& sub_type) const
	{
		return (_type == type && _sub_type.compare(sub_type) == 0);
	}
}
