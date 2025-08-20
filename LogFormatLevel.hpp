/**
 @file LogFormatLevel.hpp
 @date 20.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
 */

#ifndef Chaos_LogFormatLevel_hpp
#define Chaos_LogFormatLevel_hpp

#include <cstdint>
#include <limits>

namespace chaos {
	/**
	 @brief Уровень журналирования
	 */
	enum class log_level : std::uint8_t
	{
		dead = 0, /**< Game over */
		fatal = 10, /**< System is unusable */
		critical = 50, /**< Critical Conditions/System is unusable */
		error = 100, /**< Error conditions: "Premature end of script headers" */
		alert = 125, /**< Not a fault, but very significant */
		warning	= 150,/**< Warning conditions: "child process 1234 did not exit, sending another SIGHUP" */
		checkpoint = 175, /** < Measure everything **/
		notice = 200,/**< Normal but significant condition: "Server seems busy, you may need to increase StartThreads" */
		debug = 250, /**< Very-very verbose */
		none = std::numeric_limits<std::uint8_t>::max()
	};

	/**
	 @brief Формат журнала
	 */
	enum class log_format : std::uint8_t
	{
		text = 't',
		message = 'm',
		csv = 'c',
		json = 'j',
		xml	= 'x'
	};
}

#endif
