#/**
@file ShortMessage.hpp
@date 2014-01-01
@copyright Cadabra Systems
@author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_ShortMessage_hpp
#define Chaos_ShortMessage_hpp

#include "NotificationMessage.hpp"

#include "PhoneNumber.hpp"
#include "Flex.hpp"

#include <set>
#include <string>
#include <cstdint>

namespace chaos {
	class short_message : public notification_message
	{
	/** @name Aliases */
	/** @{ */
	public:
		using number_set = std::set<phone_number>;
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		short_message(const std::uint64_t& recipient, const std::string& body);
		short_message(const std::set<std::uint64_t>& recipient_set, const std::string& body);
		short_message(const std::set<phone_number>& recipient_set, const std::string& body);
		short_message(const phone_number& recipient, const std::string& body);
		short_message(const std::string& body);
		short_message(short_message&& origin);
		short_message(const short_message& origin);
		virtual ~short_message() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		number_set _recipient_set;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual chaos::flex dump() const override;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		bool add_recipient(const phone_number& recipient);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const number_set& get_recipient_set() const;
	/** @} */

	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
}

#endif
