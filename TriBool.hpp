//
//  TriBool.hpp
//  Chaos
//
//  Created by Daniil A Megrabyan on 20.05.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_TriBool_hpp
#define Chaos_TriBool_hpp

#include <cstdint>
#include <memory>

namespace chaos {
	class tribool
	{
	public:
		enum class state : std::uint8_t
		{
			indeterminate = 0,
			on = 1,
			off = 2
		};
		
	public:
		tribool(bool onoff)
		:
			_state(onoff ? state::on : state::off)
		{

		}
		
		tribool()
		:
			_state(state::indeterminate)
		{

		}
		
		tribool(tribool::state state)
		:
			_state(state)
		{

		}
		
	private:
		tribool::state _state;
		
	public:
		void reset(tribool::state state = tribool::state::indeterminate)
		{
			_state = state;
		}
		
		void on()
		{
			_state = tribool::state::on;
		}
		
		void off()
		{
			_state = tribool::state::off;
		}
		
	public:
		bool is_on() const
		{
			return tribool::state::on == _state;
		}
		
		bool is_off() const
		{
			return tribool::state::off == _state;
		}
		
		bool is_indeterminate() const
		{
			return tribool::state::indeterminate == _state;
		}
		
		bool is_true() const
		{
			return tribool::state::on == _state;
		}
		
		bool is_false() const
		{
			return tribool::state::off == _state;
		}
		
	public:
		tribool& operator=(bool onoff) noexcept
		{
			_state = onoff ? tribool::state::on : tribool::state::off;
			return *this;
		}
		
		tribool& operator=(std::nullptr_t) noexcept
		{
			_state = state::indeterminate;
			return *this;
		}
		
		bool operator==(const tribool& rhs) const
		{
			return _state == rhs._state;
		}
		
		bool operator==(const bool& rhs) const
		{
			return (rhs ? state::on : state::off) == _state;
		}

		bool operator!() const
		{
			return is_false();
		}
	};
}
#endif
