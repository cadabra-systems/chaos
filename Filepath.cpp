/**
 @file Filepath.cpp
 @date 16.04.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#include "Filepath.hpp"

namespace chaos {
	filepath::filepath(const std::string& path, bool auto_location)
	:
		_stream(path)
	{
		if (auto_location) {
			while (!eof()) {
				if (!nip().empty()) {
					rewind();
					break;
				}
			}
		}
		
	}
	
	const std::string& filepath::nip(bool next)
	{
		if (!next) {
			return _current;
		}

		if (eof()) {
			_current = "";
		} else {
			std::getline(_stream, _current, '/');
		}
		
		return _current;
	}
	
	bool filepath::forward()
	{
		if (eof()) {
			return false;
		}
		
		nip();
		
		return true;
	}
	
	bool filepath::rewind()
	{
		_stream.unget();

		while (_stream.good()) {
			if (_stream.tellg() == std::ios_base::seekdir::beg) {
				return true;
			}

			_stream.seekg(-1, std::ios_base::seekdir::cur);

			if (_stream.peek() == '/') {
				_stream.get();
				return true;

			}
		}
		
		return false;
	}
	
	std::string filepath::string(bool next)
	{
		return nip(next);
	}
	
	std::int64_t filepath::id(bool next)
	{
		const std::string& retval = nip(next);
		
		if (retval.empty()) {
			return 0;
		}
		
		return std::stoll(retval);
	}
	
	std::uint16_t filepath::type(bool next)
	{
		const std::string& retval = nip(next);
		
		if (retval.empty()) {
			return 0;
		}
		
		/// @xxx
		return std::stoul(retval);
	}
	
	std::uint32_t filepath::size(bool next)
	{
		return std::stoul(nip(next));
	}	
	
	std::uint64_t filepath::index(bool next)
	{
		return std::stoull(nip(next));
	}

	std::uintptr_t filepath::ptr(bool next)
	{
		return std::stoull(nip(next));
	}
	
	std::string filepath::blob(bool next)
	{
		/// @todo base64
		return "";
	}

	void filepath::operator>>(std::string& value)
	{
		value = nip();
	}
	
	void filepath::operator>>(std::uint16_t& value)
	{
		//value = std::stoui(nip_token());
	}
	
	void filepath::operator>>(std::int16_t& value)
	{
		//value = std::stos(nip_token());
	}
	
	void filepath::operator>>(std::uint32_t& value)
	{
		value = std::stoul(nip());
	}
	
	void filepath::operator>>(std::int32_t& value)
	{
		value = std::stol(nip());
	}
	
	void filepath::operator>>(std::uint64_t& value)
	{
		value = std::stoull(nip());
	}
	
	void filepath::operator>>(std::int64_t& value)
	{
		value = std::stoll(nip());
	}

	void filepath::operator>>(double& value)
	{
		value = std::stod(nip());
	}
	
	void filepath::operator>>(bool& value)
	{
		value = true;
	}
	
	void filepath::operator>>(std::chrono::system_clock::time_point& value)
	{

	}
	
	/**
	 * @bug
	 * @return
	 */
	bool filepath::eof() const
	{
		return _stream.eof();
	}
	
	bool filepath::bof() const
	{		
		return _stream.tellg() == 0;
	}
	
	bool filepath::empty() const
	{
		return bof() && eof();
	}
}
