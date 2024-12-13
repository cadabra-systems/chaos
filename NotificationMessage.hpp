/**
 @file NotificationMessage.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_NotificationMessage_hpp
#define Chaos_NotificationMessage_hpp

#include "Flex.hpp"
#include "UID.hpp"

namespace chaos {
	class notification_message
	{
	/** @name Aliases */
	/** @{ */
	public:
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		notification_message(const std::string& body);
		notification_message(notification_message&& origin);
		notification_message(const notification_message& origin);
		virtual ~notification_message() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		uid _uid;
		std::string _body;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual chaos::flex dump() const = 0;
		virtual bool invalidate(const chaos::flex& data) const;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void set_body(const std::string& body);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const uid& get_uid() const;
		const std::string& get_body() const;
	/** @} */

	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
}

#endif
