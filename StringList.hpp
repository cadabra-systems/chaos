/**
 @file StringList.hpp
 @date 26.03.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_StringList_hpp
#define Chaos_StringList_hpp

#include <sstream>
#include <string>
#include <list>

namespace chaos {
	/// @xxx Это какой-то неправильный StringList
	/// @todo Нужно переименовать во что-то другое для обратной совместимости и реализовать настоящий StringList
	class string_list
	{
	/** @name Constants */
	/** @{ */
	public:
		static const char line = '\n';
		static const char tab = '\t';
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		string_list() = default;
		virtual ~string_list() = default;
	/** @} */
		
	/** @name Factories */
	/** @{ */
	public:
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		string_list& operator<<(const std::list<std::string>& l);
		string_list& operator<<(const string_list& l);
		string_list& operator<<(std::ostream& (*f)(std::ostream&));
		
		template<typename T>
		string_list& operator<<(const T& t)
		{
			_stream << t;
			
			return *this;
		}
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		std::stringstream _stream;
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
	/** @} */
		
	/** @name Converters */
	/** @{ */
	public:
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		virtual const std::stringstream& stream() const;
	/** @} */
		
	/** @name Formatters */
	/** @{ */
	public:
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
	/** @} */
		
	/** @name Hooks */
	/** @{ */
	public:
	/** @} */
		
	/** @name Templates */
	/** @{ */
	public:
	/** @} */
	};
}
#endif
