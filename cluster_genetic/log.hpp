#ifndef LOG_HPP_INCLUDED
#define LOG_HPP_INCLUDED

#include <string>
#include <iterator>
#include <cerrno>
#include <ctime>
#include <cstring>

#define CL_FT_BLACK             "\x1b[30m"
#define CL_FT_RED               "\x1b[31m"
#define CL_FT_GREEN             "\x1b[32m"
#define CL_FT_BROWN             "\x1b[33m"
#define CL_FT_BLUE              "\x1b[34m"
#define CL_FT_PURPLE            "\x1b[35m"
#define CL_FT_MAGENTA           "\x1b[36m"
#define CL_FT_GRAY              "\x1b[37m"

#define CL_BG_BLACK             "\x1b[40m"
#define CL_BG_RED               "\x1b[41m"
#define CL_BG_GREEN             "\x1b[42m"
#define CL_BG_BROWN             "\x1b[43m"
#define CL_BG_BLUE              "\x1b[44m"
#define CL_BG_PURPLE            "\x1b[45m"
#define CL_BG_MAGENTA           "\x1b[46m"
#define CL_BG_GRAY              "\x1b[47m"

#define TY_FT_DEFAULT           "\x1b[0m"
#define TY_FT_BOLD              "\x1b[1m"
#define TY_FT_HBOLD             "\x1b[2m"
#define TY_FT_UNDERLINE         "\x1b[4m"
//#define TY_FT_BOLD            "\x1b[5m"
#define TY_FT_REVERSE           "\x1b[7m"

#define SET_FONT(X) printf(X)
#define printf_color(color, ...) \
        { \
                SET_FONT(color); \
                printf(__VA_ARGS__); \
                SET_FONT(TY_FT_DEFAULT); \
        }

// LOG MACRO
#if defined(DEBUG)
        #define log(format, ...) \
        { \
                char __tb[101]; \
                time_t t = time(NULL); \
                struct tm* tmp = localtime(&t); \
                strftime(__tb, 100, "%d/%m/%Y %H:%M:%S ", tmp); \
                printf("%s [pid=%d] ", __tb, (int)getpid()); \
                printf(format, ##__VA_ARGS__); \
                printf("\n"); \
                fflush(stdout); \
        }
#else
        #define log(format, ...) ;
#endif

// PERROR MACRO
#define PERROR(format, ...) \
        { \
                char __tb[101]; \
                time_t t = time(NULL); \
                struct tm* tmp = localtime(&t); \
                strftime(__tb, 100, "%d/%m/%Y %H:%M:%S ", tmp); \
                printf("%s [pid=%d]", __tb, (int)getpid()); \
                SET_FONT(CL_FT_RED); \
                printf(" ERROR! "); \
                SET_FONT(TY_FT_DEFAULT); \
                printf(format, ##__VA_ARGS__); \
                printf(" (%s)\n", strerror(errno)); \
                fflush(stdout); \
        }



namespace Log
{
	template< typename _T1, typename _T2 = __b_int >
	void dump_linear_container(_T1 & container)
	{
        std::copy(container.begin(), container.end(), std::ostream_iterator< _T2 >(std::cout, " "));
        std::cout << std::endl;
	}

	template< typename _T1, typename _T2 = __b_int >
	void dump_linear_container(std::string title, _T1 & container)
	{
		std::cout << title << ": ";
        std::copy(container.begin(), container.end(), std::ostream_iterator< _T2 >(std::cout, " "));
        std::cout << std::endl;
	}
}

#endif // LOG_HPP_INCLUDED
