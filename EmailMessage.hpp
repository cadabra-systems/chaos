#/**
@file EmailMessage.hpp
@date 2014-01-01
@copyright Cadabra Systems
@author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_EmailMessage_hpp
#define Chaos_EmailMessage_hpp

#include "NotificationMessage.hpp"

#include "EmailAddress.hpp"
#include "Flex.hpp"

#include <set>
#include <string>
#include <sstream>

namespace chaos {
	class email_message : public notification_message
	{
	/** @name Aliases */
	/** @{ */
	public:
		using address_set = std::set<email_address>;
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
		class payload
		{
		public:
			std::string data;
			size_t size;
			payload(const std::stringstream& stream);
			payload();

			bool operator!() const;
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		email_message(const email_address& sender, const email_address& recipient, const std::string& subject, const std::string& body);
		email_message(const email_address& recipient, const std::string& subject, const std::string& body);
		email_message(const std::string& subject, const std::string& body);
		email_message(email_message&& origin);
		email_message(const email_message& origin);
		virtual ~email_message() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		email_address _sender;
		address_set _recipient_set;

		std::string _subject;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		payload make_payload(const std::string& message_id_suffix = "") const;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual chaos::flex dump() const override;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void set_sender(const email_address& sender);
		void add_recipient(const email_address& recipient);
		void set_subject(const std::string& subject);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const email_address& get_sender() const;
		const address_set& get_recipient_set() const;
		const std::string& get_subject() const;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_complete() const;
	/** @} */
	};
}

#endif
