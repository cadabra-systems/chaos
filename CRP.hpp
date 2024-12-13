/**
 @file CRP.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_CRP_hpp
#define Chaos_CRP_hpp

#include "Flex.hpp"
#include "Filepath.hpp"
#include "UID.hpp"
#include "Flex/UID.hpp"

#include <cstdint>
#include <list>
#include <sstream>

namespace chaos {
	class crp_moniker
	{
	/** @name Aliases */
	/** @{ */
	public:
		using key_type = std::uintptr_t;
		using list = std::list<key_type>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		crp_moniker()
		:
			_locator(0)
		{

		}

		crp_moniker(const crp_moniker& origin)
		:
			_path(origin._path),
			_locator(origin._locator)
		{

		}

		crp_moniker(crp_moniker&& origin)
		:
			_path(std::move(origin._path)),
			_locator(std::move(origin._locator))
		{

		}

		crp_moniker(const flex& path)
		:
			_locator(0)
		{
			parse(path);
		}

		crp_moniker(const std::string& path)
		:
			_locator(0)
		{
			parse(path);
		}

		crp_moniker(list path)
		:
			_path(path),
			_locator(0)
		{

		}

		crp_moniker(key_type hash)
		:
			_locator(0)
		{
			_path.push_back(hash);
		}
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		crp_moniker& operator=(const crp_moniker& origin)
		{
			_path = origin._path;
			return *this;
		}

		crp_moniker& operator=(crp_moniker&& origin)
		{
			_path = std::move(origin._path);
			return *this;
		}

		operator flex() const
		{
			return flex(_path);
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		list _path;
		key_type _locator;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void parse(const flex& data)
		{
			if (data.is_number_unsigned()) {
				_path.clear();
				_path.push_back(data.get<crp_moniker::key_type>());
			} else if (data.is_string()) {
				parse(data.get<std::string>());
			} else if (data.is_array()) {
				parse(data.cbegin(), data.cend());
			}
		}

		void parse(flex::const_iterator iterator_begin, flex::const_iterator iterator_end)
		{
			_path.clear();
			for (flex::const_iterator i = iterator_begin; i != iterator_end; i++) {
				if (i->is_number_unsigned()) {
					_path.push_back(i->get<key_type>());
				}
			}
		}

		void parse(std::string value)
		{
			_path.clear();
			if (value.empty()) {
				return ;
			}

			const std::string::size_type dash(value.find("#"));
			if (std::string::npos != dash) {
				_locator = std::stoll(value.substr(dash + 1));
				value.erase(dash);
			}

			filepath path(value, true);
			while (!path.eof()) {
				_path.push_back(path.ptr());
			}
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		list::const_iterator cbegin() const
		{
			return _path.cbegin();
		}

		list::const_iterator cend() const
		{
			return _path.cend();
		}

		const key_type& locator() const
		{
			return _locator;
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool empty() const
		{
			return _path.empty();
		}

		bool located() const
		{
			return _locator > 0;
		}
	/** @} */
	};

	class crp_reflector
	{
	/** @name Constructors */
	/** @{ */
	public:
		crp_reflector()
		{

		}

		crp_reflector(const crp_reflector& origin)
		:
			_interface(origin._interface),
			_method(origin._method)
		{

		}

		crp_reflector(crp_reflector&& origin)
		:
			_interface(std::move(origin._interface)),
			_method(std::move(origin._method))
		{

		}

		crp_reflector(const flex& data)
		{
			parse(data);
		}

		crp_reflector(flex::const_iterator& iterator_begin, flex::const_iterator& iterator_end)
		{			
			parse(iterator_begin, iterator_end);
		}

		crp_reflector(const std::string& path)
		{
			parse(path);
		}

		crp_reflector(const std::string& interface, const std::string& method)
		:
			_interface(interface),
			_method(method)
		{

		}

		crp_reflector& operator=(const crp_reflector& origin)
		{
			_interface = origin._interface;
			_method = origin._method;

			return *this;
		}

		crp_reflector& operator==(crp_reflector&& origin)
		{
			_interface = std::move(origin._interface);
			_method = std::move(origin._method);

			return *this;
		}
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void parse(const flex& data)
		{
			if (data.is_string()) {
				parse(data.get<std::string>());
			} else if (data.is_object()) {
				_method = data["method"].get<std::string>();
				flex::const_iterator i(data.find("interface"));
				_interface =  data.cend() == i ? "" : i.value().get<std::string>();
			} else if (data.is_array()) {
				parse(data.cbegin(), data.cend());
			}
		}

		void parse(const std::string& data)
		{
			filepath path(data);

			_interface = path.string();
			_method = path.string();

			if (_method.empty()) {
				_interface.swap(_method);
			}
		}

		void parse(flex::const_iterator iterator_begin, flex::const_iterator iterator_end)
		{
			if (std::distance(iterator_begin, iterator_end) > 1) {
				_interface = iterator_begin->get<std::string>();
				_method = (++iterator_begin)->get<std::string>();
			} else {
				_method = iterator_begin->get<std::string>();
			}
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::string _interface;
		std::string _method;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		operator flex() const
		{
			return flex::object({ {"interface", _interface}, {"method", _method} } );
		}
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void interface(const std::string& interface)
		{
			_interface = interface;
		}

		void method(const std::string& method)
		{
			_method = method;
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::string& interface() const
		{
			return _interface;
		}

		const std::string& method() const
		{
			return _method;
		}
	/** @} */
	};

	class crp_action
	{
	/** @name Constants */
	/** @{ */
	private:

	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		crp_action()
		{

		}

		crp_action(const crp_moniker& moniker, const crp_reflector& reflector, const flex& payload)
		:
			_moniker(moniker),
			_reflector(reflector),
			_payload(payload)
		{

		}

		crp_action(const crp_moniker& moniker, const crp_reflector& reflector, flex&& payload)
		:
			_moniker(moniker),
			_reflector(reflector),
			_payload(std::move(payload))
		{

		}

		crp_action(const flex& data)
		{
			parse(data);
		}
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		crp_moniker _moniker;
		crp_reflector _reflector;
		flex _payload;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		flex& flush(flex& data) const
		{
			return (data = {{"moniker", _moniker}, {"reflector", _reflector}, {"payload", _payload}});
		}

		void parse(const flex& data)
		{
			_moniker = data["moniker"];
			_reflector = data["reflector"];
			_payload = data["payload"];
		}
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void moniker(const crp_moniker& moniker)
		{
			_moniker = moniker;
		}

		void reflector(const crp_reflector& reflector)
		{
			_reflector = reflector;
		}

		void payload(flex&& payload)
		{
			_payload = std::move(payload);
		}

		void payload(const flex& payload)
		{
			_payload = payload;
		}

		template<typename T>
		void payload(const T& payload)
		{
			_payload = payload;
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const crp_moniker& moniker() const
		{
			return _moniker;
		}

		const crp_reflector& reflector() const
		{
			return _reflector;
		}

		const chaos::flex& payload() const
		{
			return _payload;
		}
	/** @} */
	};

	class crp_reaction
	{
	/** @name Constants */
	/** @{ */
	public:
		enum class payload_state : std::uint8_t
		{
			data = 0x00,
			crp_exception = 0x01,
			internal_exception = 0x02,
			external_exception = 0x03,
			unknown_exception = 0x04
		};

		enum class error_state : std::uint8_t
		{
			crp_exception = 0x01,
			internal_exception = 0x02,
			external_exception = 0x03,
			unknown_exception = 0x04
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		crp_reaction()
		:		
			_timepoint(std::chrono::system_clock::now()),
			_final(false),
			_state(payload_state::data),
			_duration(0)
		{


		}

		crp_reaction(crp_reaction&& origin)
		:
			_timepoint(std::move(origin._timepoint)),
			_final(std::move(origin._final)),
			_state(std::move(origin._state)),
			_duration(std::move(origin._duration)),
			_payload(std::move(origin._payload))
		{


		}

		crp_reaction(const crp_reaction& origin)
		:
			_timepoint(origin._timepoint),
			_final(origin._final),
			_state(origin._state),
			_duration(origin._duration),
			_payload((origin._payload))
		{


		}
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		crp_reaction& operator=(const crp_reaction& origin)
		{
			_timepoint = origin._timepoint;
			_final = origin._final;
			_state = origin._state;
			_duration = origin._duration;
			_payload = origin._payload;

			return *this;
		}

		crp_reaction& operator=(const crp_reaction&& origin)
		{
			_timepoint = std::move(origin._timepoint);
			_final = std::move(origin._final);
			_state = std::move(origin._state);
			_duration = std::move(origin._duration);
			_payload = std::move(origin._payload);

			return *this;
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::chrono::time_point<std::chrono::system_clock> _timepoint;
		bool _final;

		payload_state _state;
		std::chrono::milliseconds _duration;
		chaos::flex _payload;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		flex& flush(flex& output) const
		{
			return (output = flex::object({{"state", static_cast<std::uint8_t>(_state)}, {"duration", static_cast<std::uint64_t>(_duration.count())}, {"payload", _payload}}));
		}
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void set(error_state state, std::uint32_t code, std::string message, std::uint64_t duration = 0)
		{
			if (_final) {
				return;
			}

			_state = static_cast<payload_state>(state);
			_payload = {{"code", code}, {"message", message}};

			finalize(duration);
		}

		void set(const flex& payload, std::uint64_t duration = 0)
		{
			if (_final) {
				return;
			}

			_state = payload_state::data;
			_payload = payload;

			finalize(duration);
		}

		void set(flex&& payload, std::uint64_t duration = 0)
		{
			if (_final) {
				return;
			}

			_state = payload_state::data;
			_payload = std::move(payload);

			finalize(duration);
		}

		template <typename T>
		void set(T payload, std::uint64_t duration = 0)
		{
			if (_final) {
				return;
			}

			_state = payload_state::data;
			_payload = payload;

			finalize(duration);
		}

	private:
		void finalize(std::uint64_t duration = 0)
		{
			_final = true;

			if (duration != 0) {
				_duration = static_cast<std::chrono::milliseconds>(duration);
			}

			_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - _timepoint);
		}
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		void parse(const flex& data)
		{
			_final = true;

			_state = static_cast<payload_state>(data["state"].get<std::uint8_t>());
			_duration = static_cast<std::chrono::milliseconds>(data["duration"].get<std::uint64_t>());
			_payload = data["payload"];
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const payload_state& state() const
		{
			return _state;
		}
		const std::chrono::milliseconds& duration() const
		{
			return _duration;
		}

		const chaos::flex& payload() const
		{
			return _payload;
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool falty() const
		{
			return _state != payload_state::data;
		}

		bool finalized() const
		{
			return _final;
		}
	/** @} */
	};

	enum class crp_transproto
	{
		unsupported = 0x00,
		cmf = 0x01,
		amf = 0x02,
		json = 0x03,
		msg_pack = 0x04,
		cbor = 0x05
	};

	using crp_dialect = std::uint8_t;
	static flex parse_to_flex(std::istream& stream, crp_transproto transproto)
	{
		switch (transproto)
		{
			case crp_transproto::json:
				return flex::from_json(stream);

			case crp_transproto::cbor:
				return flex::from_cbor(stream);

			case crp_transproto::msg_pack:
				return flex::from_msgpack(stream);

			default:
				return flex();
		}
	}

	static std::ostream& parse_from_flex(std::ostream& stream, flex& data, crp_transproto transproto)
	{
		switch (transproto)
		{
			case crp_transproto::json:
				stream << flex::to_json(data);
				break;

			case crp_transproto::cbor:
				/// @todo stream << flex::to_cbor(data);
				break;

			case crp_transproto::msg_pack:
				/// @todo stream << flex::to_msgpack(data);
				break;

			default:
				return stream;
		}

		return stream;
	}

	class crp_header final
	{
	/** @name Statics */
	/** @{ */
	public:
		const static crp_dialect default_dialect = 0x01;
	/** @} */

	/** @name Operators */
	/** @{ */
		crp_header(bool generate_id = false, crp_dialect dialect = crp_header::default_dialect)
		:
			_id(generate_id),
			_dialect(dialect)
		{

		}

		crp_header(uid id, crp_dialect dialect = crp_header::default_dialect)
		:
			_id(id),
			_dialect(dialect)
		{

		}

		~crp_header() = default;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name Converters */
	/** @{ */
	public:
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		const flex& parse(const flex& data)
		{
			_id = data["id"];
			_dialect = data["dialect"];
			flex::const_iterator t(data.find("timepoint"));
			_timepoint = std::chrono::system_clock::time_point(std::chrono::microseconds(t->is_string() ? std::stoll(t->get<std::string>()) : t->get<std::int64_t>()));

			return data;
		}

		flex& flush(flex& output) const
		{
			return (output = flex::object({{"id", _id}, {"timepoint", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(_timepoint.time_since_epoch()).count())}, {"dialect", _dialect}}));
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		uid _id;
		crp_dialect _dialect;
		std::chrono::system_clock::time_point _timepoint;
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void dialect(crp_dialect dialect)
		{
			_dialect = dialect;
		}

		void id(uid id)
		{
			_id = id;
		}

		void timepoint(std::chrono::system_clock::time_point timepoint)
		{
			_timepoint = timepoint;
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const crp_dialect& dialect() const
		{
			return _dialect;
		}

		const uid& id() const
		{
			return _id;
		}

		const std::chrono::system_clock::time_point& timepoint() const
		{
			return _timepoint;
		}
	/** @} */
	};

	class crp_message
	{
	/** @name Constructors */
	/** @{ */
	public:
		crp_message(crp_transproto transproto, crp_header header)
		:
			_transproto(transproto),
			_header(header)
		{

		}

		crp_message(crp_transproto transproto)
		:
			_transproto(transproto)
		{

		}

		virtual ~crp_message()
		{

		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		crp_transproto _transproto;
		crp_header _header;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		const flex& parse(const flex& data)
		{
			return _header.parse(data);
		}

		flex& flush(flex& output) const
		{
			flex& body(_header.flush(output)["body"]);
			body = flex::array();

			return body;
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const crp_transproto& transproto() const
		{
			return _transproto;
		}

		const crp_header& header() const
		{
			return _header;
		}

		crp_header& header()
		{
			return _header;
		}
	/** @} */
	};

	class crp_request final : public crp_message
	{
	/** @name Statics */
	/** @{ */
	public:
	/** @} */

	/** @name Operators */
	/** @{ */
		crp_request(std::istream& stream, crp_transproto transproto = crp_transproto::json)
		:
			crp_message(transproto)
		{
			parse(parse_to_flex(stream, transproto));
		}

		crp_request(const chaos::flex& data, crp_transproto transproto = crp_transproto::json)
		:
			crp_message(transproto)
		{
			parse(data);
		}

		crp_request(crp_transproto transproto = crp_transproto::json)
		:
			crp_message(transproto)
		{

		}

		virtual ~crp_request() = default;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name Converters */
	/** @{ */
	public:
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void parse(std::istream& stream)
		{
			parse(parse_to_flex(stream, crp_message::transproto()));
		}

		void parse(const flex& data)
		{
			flex::const_iterator p(data.find("path"));
			if (data.cend() != p) {
				_path.parse(p.value());
			}
			_body = crp_message::parse(data)["body"].get<std::vector<crp_action>>();
		}

		void parse(flex&& data)
		{
			flex::const_iterator p(data.find("path"));
			if (data.cend() != p) {
				_path.parse(p.value());
			}
			_body = crp_message::parse(data)["body"].get<std::vector<crp_action>>();
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		crp_moniker _path;
		std::vector<crp_action> _body;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::vector<crp_action>& array() const
		{
			return _body;
		}

		const crp_moniker& path() const
		{
			return _path;
		}
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void add(crp_action action)
		{
			_body.push_back(std::move(action));
		}
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		std::ostream& flush(std::ostream& stream) const
		{
			flex output;
			crp_message::flush(output) = _body;

			return parse_from_flex(stream, output, transproto());
		}

		flex& flush(flex& output) const
		{
			crp_message::flush(output) = _body;

			return output;
		}
	/** @} */
	};

	class crp_response final : public crp_message
	{
	/** @name Statics */
	/** @{ */
	public:
	/** @} */

	/** @name Operators */
	/** @{ */
		crp_response(std::istream& stream, crp_transproto transproto = crp_transproto::json)
		:
			crp_message(transproto)
		{
			parse(parse_to_flex(stream, transproto));
		}

		crp_response(const chaos::flex& data, crp_transproto transproto = crp_transproto::json)
		:
			crp_message(transproto)
		{
			parse(data);
		}

		crp_response(const crp_request& request)
		:
			crp_message(request.transproto(), {request.header().id(), request.header().dialect()})
		{

		}

		virtual ~crp_response() = default;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name Converters */
	/** @{ */
	public:
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void parse(const flex& data)
		{
			_body = crp_message::parse(data)["body"].get<std::vector<crp_reaction>>();
		}

		void parse(flex&& data)
		{
			_body = crp_message::parse(data)["body"].get<std::vector<crp_reaction>>();
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::vector<crp_reaction> _body;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void add(crp_reaction reaction)
		{
			_body.push_back(std::move(reaction));
		}
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		std::ostream& flush(std::ostream& stream) const
		{
			flex output;
			crp_message::flush(output) = _body;

			return parse_from_flex(stream, output, transproto());
		}

		flex& flush(flex& output) const
		{
			crp_message::flush(output) = _body;

			return output;
		}
	/** @} */
	};

	/**
	 * @brief Moniker
	 */
	inline void to_flex(flex& output, const crp_moniker& input)
	{
		output = input;
	}
	inline void from_flex(const flex& input, crp_moniker& output)
	{
		output.parse(input);
	}

	/**
	 * @brief reflector
	 */
	inline void to_flex(flex& output, const crp_reflector& input)
	{
		output = input;
	}
	inline void from_flex(const flex& input, crp_reflector& output)
	{
		output.parse(input);
	}

	/**
	 * @brief Header
	 */
	inline void to_flex(flex& output, const crp_header& input)
	{
		input.flush(output);
	}
	inline void from_flex(const flex& input, crp_header& output)
	{
		output.dialect(input["dialect"]);
		output.id(input["id"]);
		flex::const_iterator t(input.find("timepoint"));
		output.timepoint(std::chrono::system_clock::time_point(std::chrono::microseconds(t->is_string() ? std::stoll(t->get<std::string>()) : t->get<std::int64_t>())));
	}

	/**
	 * @brief Request
	 */
	inline void to_flex(flex& output, const crp_request& input)
	{
		input.flush(output);
	}
	inline void from_flex(const flex& input, crp_request& output)
	{
		output.parse(input);
	}

	/**
	 * @brief Response
	 */
	inline void to_flex(flex& output, const crp_response& input)
	{
		input.flush(output);
	}
	inline void from_flex(const flex& input, crp_response& output)
	{
		output.parse(input);
	}

	/**
	 * @brief Action(request)
	 */
	inline void to_flex(flex& output, const crp_action& input)
	{
		input.flush(output);
	}
	inline void from_flex(const flex& input, crp_action& output)
	{
		output.parse(input);
	}

	/**
	 * @brief Reflex(response)
	 */
	inline void to_flex(flex& output, const crp_reaction& input)
	{
		input.flush(output);
	}
	inline void from_flex(const flex& input, crp_reaction& output)
	{
		output.parse(input);
	}
}

#endif
