/**
 @file Consumer.hpp
 @date 2026-04-17
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Kafka_Consumer_hpp
#define Chaos_Kafka_Consumer_hpp

#include "Connection.hpp"

#include "Message.hpp"

#include <set>
#include <string>

namespace chaos { namespace kafka {
	class consumer : public connection
	{
	/** @name Constructors */
	/** @{ */
	public:
		consumer(const std::set<std::pair<std::string, std::uint16_t>>& broker_set, const std::map<std::string, std::string>& configuration_map = {});
		consumer(const std::string& hostname, std::uint16_t port, const std::map<std::string, std::string>& configuration_map = {});
		consumer(consumer&&);
		consumer(const consumer&) = delete;
		virtual ~consumer() override;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		rd_kafka_queue_t* _queue;
		int _pipe[2];
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		consumer& operator=(consumer&&) = delete;
		consumer& operator=(const consumer&) = delete;
	/** @} */

	/** @name Procedures */
	/** @{ */
	protected:
		virtual bool connect(const std::string& username, const std::string& password) noexcept override;

	public:
		bool connect(const std::string& username, const std::string& password, const std::string& group_id) noexcept;
		virtual bool disconnect() noexcept override;

		bool subscribe(const std::set<std::string>& topic_name_set) noexcept;
		bool unsubscribe() noexcept;

		message poll(int timeout = 0) noexcept;

		bool commit() noexcept;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const int& get_file_descriptor() const noexcept;
	/** @} */
	};
} }

#endif
