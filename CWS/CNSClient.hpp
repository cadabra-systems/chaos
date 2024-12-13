/**
 @file CNSClient.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_CWS_CNSClient_hpp
#define Chaos_CWS_CNSClient_hpp

#include "../WebAPI.hpp"

#include "../EmailMessage.hpp"
#include "../PushMessage.hpp"
#include "../ShortMessage.hpp"

#include <list>
#include <set>
#include <string>

namespace chaos { namespace cws {
	class cns_client : protected web_api
	{
	/** @name Statics */
	/** @{ */
	public:
		struct trap
		{
			std::set<phone_number> phone_number_set;
			std::set<device_token> device_token_set;
			std::set<email_address> email_address_set;
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		cns_client(const uri& uri);
		cns_client(const std::string& hostname, const std::string& access_key, bool secure, bool deffer_mode = false);
		cns_client(cns_client&& origin);
		cns_client(cns_client const&) = delete;
		virtual ~cns_client();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _access_key;
		const bool _deffer_mode;
		std::size_t _batch_size;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	private:
		bool send(const std::list<std::reference_wrapper<const notification_message>>& message_list, trap* trap_set);

	public:
		bool send(const notification_message& message);
		bool send(const std::list<std::reference_wrapper<const notification_message>>& message_list);
		bool send(const std::list<std::reference_wrapper<const notification_message>>& message_list, trap& trap);
	/** @} */

	/** @name States*/
	/** @{ */
	public:
		bool is_deffered() const;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		cns_client& operator=(cns_client&& right) = delete;
		cns_client& operator=(cns_client&) = delete;
		cns_client& operator=(cns_client const&) = delete;
		cns_client& operator=(cns_client const&&) = delete;
	/** @} */
	};
} }

#endif
