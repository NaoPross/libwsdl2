#pragma once

#include <iostream>
#include <sstream>

#ifdef DEBUG
	#ifdef __builtin_strrchr
		#define __FILENAME__ (\
			__builtin_strrchr(__FILE__, '/') ? \
			__builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

		#define npdebug_fname(); { \
				std::cerr << "[" << __FILENAME__ << ":" << __LINE__ << "] "; \
		}

		#define npdebug(...); { \
			npdebug_fname(); \
			np::va_debug(__VA_ARGS__); \
		}
	#else
		#define npdebug(...); np::va_debug(__VA_ARGS__);
	#endif

	namespace np {
		template<typename... Args>
		inline void va_debug(Args&... args) {
			(std::cerr << ... << args) << std::endl;
		}

		template<typename T>
		void range_debug(const T& t) {
			range_debug("", t);
		}

		template<typename T>
		void range_debug(const std::string& msg, const T& t) {
			std::string out;
			for (auto elem : t)
				out += elem += ", ";

			npdebug(msg, out);
		}

		template<typename T>
		T inspect(const T& t) {
			npdebug(t);
			return t;
		}

		template<typename T>
		T inspect(const std::string& msg, const T& t) {
			npdebug(msg, t);
			return t;
		}
	}
#else
	#define npdebug(...) {}

	namespace np {
		template<typename... Args>
		inline void va_debug(Args&... args) {}

		template<typename T>
		inline void range_debug(const T& t) {}

		template<typename T>
		inline void range_debug(const std::string& msg, const T& t) {}

		template<typename T>
		T inspect(const T& t) { return t; }

		template<typename T>
		T inspect(const std::string& msg, const T& t) { return t; }
	}
#endif
