/**
 @file InputEcho.hpp
 @date 04.03.16
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#ifndef Chaos_InputEcho_hpp
#define Chaos_InputEcho_hpp

#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace chaos {
	class input_echo
	{
	private:
		static void onoff(bool enable)
		{
#ifdef WIN32
			HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
			DWORD mode;
			GetConsoleMode(hStdin, &mode);

			if (enable) {
				mode &= ~ENABLE_ECHO_INPUT;
			} else {
				mode |= ENABLE_ECHO_INPUT;
			}
			SetConsoleMode(hStdin, mode );
#else
			struct termios tty;
			tcgetattr(STDIN_FILENO, &tty);
			if (enable) {
				tty.c_lflag &= ~ECHO;
			}
			else {
				tty.c_lflag |= ECHO;
			}
			(void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
		}

	public:
		input_echo(bool auto_on = true)
		:
			_on(false)
		{
			if ((_on = auto_on)) {
				onoff(true);
			}
		}

		~input_echo()
		{
			off();
		}

		void on()
		{
			if (!_on) {
				_on = true;
				onoff(true);
			}
		}

		void off()
		{
			if (_on) {
				_on = false;
				onoff(false);
			}
		}
	private:
		bool _on;
	};
}

#endif
