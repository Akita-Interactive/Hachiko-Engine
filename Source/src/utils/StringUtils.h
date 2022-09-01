#pragma once
#include <string>

namespace Hachiko
{
    class StringUtils final
    {
    public:
        static std::string Format(const char* format, ...)
        {
            static char tmp_string[4096];
            va_list ap;
            va_start(ap, format);
            vsprintf_s(tmp_string, 4096, format, ap);
            va_end(ap);
            return tmp_string;
        }

        // copied from https://cpp-optimizations.netlify.app/strings_concatenation/
        template<class... Args>
        static std::string Concat(const Args&... args)
        {
            size_t tot_size = StrSize(args...);
            std::string out;
            out.reserve(tot_size);

            StrAppend(out, args...);
            return out;
        }

        static std::wstring StringToWString(const std::string& s)
        {
            std::wstring temp(s.length(), L' ');
            std::copy(s.begin(), s.end(), temp.begin());
            return temp;
        }

        static std::string ToString(bool value)
        {
            return value ? "true" : "false";
        }

        static std::string ToString(int value)
        {
            return ToString(static_cast<bool>(value));
        }

    private:
        static size_t StrSize(const char* str)
        {
            return strlen(str);
        }

        static size_t StrSize(const std::string& str)
        {
            return str.size();
        }

        template<class Head, class... Tail>
        static size_t StrSize(const Head& head, const Tail&... tail)
        {
            return StrSize(head) + StrSize(tail...);
        }

        template<class Head>
        static void StrAppend(std::string& out, const Head& head)
        {
            out += head;
        }

        template<class Head, class... Args>
        static void StrAppend(std::string& out, const Head& head, const Args&... args)
        {
            out += head;
            StrAppend(out, args...);
        }
    };
}
