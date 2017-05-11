#include <iostream>
#include <fstream>
#include <cstring>

#include <stdio.h>							//for va_start
#include <stdarg.h>							//for va_start

#include "textutl.h"


#define _ass(exp)			((void)0)
#define rel_assert(exp)			((void)0)


using namespace std;

__int64 get_file_size(const char *file_name)
{
	std::ifstream file(file_name, std::ios::in | std::ios::binary);
	file.seekg(0, std::ios::end);
	return file.tellg();
}


bool read_file_to_str(const char *file_name, char *_buf, int size)
{
	//открываем файл
	FILE *file1 = fopen(file_name, "rb");
	if(!file1)
		return (_ass(0), false);

	//читаем весь файл в память
	int bb2 = (int)fread(_buf, 1, size, file1);
	_ass(bb2 == size);

	_buf[size] = 0;

	fclose(file1);
	return true;
}


bool read_file_to_str(const char *file_name, std::string &buf)
{
	int size = (int)get_file_size(file_name);
	if(!size)
		return (_ass(0), false);

	buf.resize(size);

	return read_file_to_str(file_name, &buf[0], (int)buf.size());
}


bool needed_char(char c)
{
    return textutl::is_letter_rus(c) && c == textutl::get_lower_rus(c) || textutl::is_numeric(c);
}

int per1000(int val, int cnt)
{
	return (int)((double)val / cnt * 1000 + 0.5);
}

//------------------------------------------------------------------------------
namespace zstr
{
std::string fmt(const char *_sFormat, ...)
{
	va_list args;
	va_start( args, _sFormat );

	string ret;
	int len = vsnprintf(NULL, 0, _sFormat, args ) + 1;
	if(len > 1)
	{
		struct ZSTR_FMT{char *p;int len;ZSTR_FMT():p(0),len(0){}; ~ZSTR_FMT(){if(p)delete p;p=0;len=0;};};
		static ZSTR_FMT buf;
		if(len > buf.len)
		{
			delete buf.p;
			if(len < 128)
				len = 128;
			buf.len = len;
			buf.p = new char[len];
		}
		if(vsprintf(buf.p, _sFormat, args) >= 0)
			ret = buf.p;
		va_end(args);
	}

	return ret;
}
}

std::string freq(int val, int cnt)
{
	return zstr::fmt("%.02f", (double)val / cnt * 100);
}


void calc_letters_stat(const char* file_name, const char* res_file_name)
{
	std::string buf;
	if (!read_file_to_str(file_name, buf))
		return (_ass(0));

	ofstream out(res_file_name);

	int arr[256];
	memset(arr, 0, sizeof(arr));

	for (int i = 0; i < (int)buf.size(); i++)
	{
		char c = buf[i];
		if (textutl::is_letter_rus(c) || textutl::is_numeric(c))
		{
			c = textutl::get_lower_rus(c);
			unsigned char cc = (unsigned char)c;
			arr[cc]++;
		}
	}

	out << "лексема";
	out << "\tколичество";
	out << "\tчастота," << '%';
	out << "\tчастота на 1000";
	out << "\n";

	int cnt = 0;
	for (int i = 0; i < 256; i++)
	{
		char c = (char)i;
		if (needed_char(c))
			cnt += arr[i];
	}

	out << "всего";
	out << "\t" << cnt;
	out << "\t" << freq(cnt, cnt);
	out << "\t" << per1000(cnt, cnt);
	out << "\n";

	for (int i = 0; i < 256; i++)
	{
		char c = (char)i;
		if (needed_char(c))
		{
			out << c;
			out << "\t" << arr[i];
			out << "\t" << freq(arr[i], cnt);
			out << "\t" << per1000(arr[i], cnt);
			out << "\n";
		}
	}
}


void calc_bigrams_stat(const char* file_name, const char* res_file_name)
{
	std::string buf;
	if (!read_file_to_str(file_name, buf))
		return (_ass(0));

	ofstream out(res_file_name);

	int arr[256*256];
	memset(arr, 0, sizeof(arr));

	for (int i = 0; i < (int)buf.size()-1; i++)
	{
		char c1 = buf[i];
		char c2 = buf[i+1];
		if ((textutl::is_letter_rus(c1)/* || textutl::is_numeric(c1)*/)
            &&
            (textutl::is_letter_rus(c2)/* || textutl::is_numeric(c2)*/))
		{
			c1 = textutl::get_lower_rus(c1);
			c2 = textutl::get_lower_rus(c2);
			unsigned char cc1 = (unsigned char)c1;
			unsigned char cc2 = (unsigned char)c2;
			arr[cc1 * 256 + cc2]++;
		}
	}

	out << "биграмм";
	out << "\tколичество";
	out << "\tчастота," << '%';
	out << "\tчастота на 1000";
	out << "\n";

	int cnt = 0;
	for (int i = 0; i < 256; i++)
	for (int j = 0; j < 256; j++)
	{
		char c1 = (char)i;
		char c2 = (char)j;
		if (needed_char(c1) && needed_char(c2))
			cnt += arr[256 * i + j];
	}

	out << "всего";
	out << "\t" << cnt;
	out << "\t" << freq(cnt, cnt);
	out << "\t" << per1000(cnt, cnt);
	out << "\n";

	for (int i = 0; i < 256; i++)
    for (int j = 0; j < 256; j++)
	{
		char c1 = (char)i;
		char c2 = (char)j;
        int val = arr[256 * i + j];
		if (val > 0)
		{
			out << c1 << c2;
			out << "\t" << val;
			out << "\t" << freq(val, cnt);
			out << "\t" << per1000(val, cnt);
			out << "\n";
		}
	}
}


int main()
{
	setlocale(LC_ALL, "Russian");

    //cout << "Hello world!" << endl;
    calc_letters_stat("data/text.txt", "data/text_stat.txt");
    calc_bigrams_stat("data/text.txt", "data/text_bigrams_stat.txt");

    return 0;
}
