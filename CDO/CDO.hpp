/**
 @file CDO.hpp
 @date 2022-05-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_CDO_CDO_hpp
#define Chaos_CDO_CDO_hpp

#include <string>

namespace chaos { namespace cdo {
	enum class connection_target_type
	{
		connection_string
	};
	
	/**
	 @brief Диагностическая информация для получения сообщений о возникших ошибках
	 @details Получается с помощью SQLGetDiagRec().
	 В прикладной логике доступна через классы-наследники ODBC::AbstractError
	 
	 @see ODBC::ConnectionError
	 @see ODBC::ConnectionError
	 @see ODBC::StatementError
	 @see ODBC::CursorError
	 @see ODBC::EnvironmentError
	 */
	struct diagnostic
	{
		/**
		 @brief Код
		 @todo Уточнить и добавить примеров
		 */
		const int code;
		/**
		 @brief State
		 @todo Уточнить и добавить примеров
		 */
		const std::string state;
		/**
		 @brief Сообщение
		 */
		const std::string message;
	};
} }

#endif


