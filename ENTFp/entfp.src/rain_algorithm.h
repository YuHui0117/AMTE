/**
* Copyright (c) rain, ICT
* All rights reserved.
*
* 
* @brief    some common algorithms, only for linux platform.
* @version	1.0
* @author   summer
* @date 	2010/4/28
* @note 	
*/

#ifndef RAIN_ALGORITHM 
#define RAIN_ALGORITHM

//force STl to use malloc for memory management
#define __STL_USE_STD_ALLOCATOR
#define __USE_MALLOC

#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <stack>
#include <cstring>
#include <string>
#include <iterator>
#include <stdexcept>
#include <map>
#include <memory>
#include <list>
#include <locale>
#include <vector>
#include <queue>
#include <numeric>
#include <utility>
#include <fstream>
#include <functional>
#include <ctime>
#include <cassert>
#include <limits>	
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

#define PRINT_INF(x)            cout << __FILE__ << ":" << __LINE__ << ":" << x << endl;
#define VVec(type)              vector<vector<type> >
#define ZERO                    1e-8
#define INT64                   long long
#define UINT64                  unsigned long long

namespace rain
{
    template<class Type>
    vector<Type>& operator += (vector<Type>& v1, const vector<Type>& v2)        
    {
        for (int id = 0; id < min((int)v1.size(), (int)v2.size()); ++id)
            v1[id] += v2[id];
        return v1;
    }

    template<class Type>
    vector<Type>& operator -= (vector<Type>& v1, const vector<Type>& v2)
    {
        for (int id = 0; id < min((int)v1.size(), (int)v2.size()); ++id)
            v1[id] -= v2[id];
        return v1;
    }

    /**
      @brief used for quick sort algorithm. [begin, end)
     */
    template<class Iterator, class Type>
    inline Iterator partition(Iterator begin, Iterator end, Iterator pos)
    {
        assert(begin <= pos && pos < end);
        Type v = *pos;
        Iterator left  = begin;
        Iterator right = end - 1;

        *pos = *right;
        for (;;)
        {
            while (left < right && *left < v)
                ++left;
            if (left == right)
                break;
            else                    
            {
                *right = *left;
                --right;
            }
            while (left < right && *right >= v)
                --right;
            if (left == right)
                break;
            else
            {
                *left = *right;
                ++left;
            }
        }
        *left = v;
        return left;
    }
    
    inline UINT64 rand64()
    {
        return rand() ^ ((UINT64)rand() << 15) ^ ((UINT64)rand() << 30) ^ ((UINT64)rand() << 45) ^ ((UINT64)rand() << 60);
    }	

    /**
      @brief count the frequency of subs appear in str.
     */
    inline int count(const string& str, string subs)
    {
        if (subs.empty())
            return 0;
        int r = 0;
        int p = -1;
        while ((p = str.find(subs, p + 1)) != (int)string::npos)
            ++r;
        return r;
    }

    /**
      @brief erase any char in chs at the begin or end of s.
     */
    inline string strip(const string& s, const string chs = " \t\n")
    {
        if (s.empty())
            return s;
        int i = 0;
        while (i < (int)s.size() && chs.find(s[i]) != string::npos)
            ++i;
        int j = (int)s.size() - 1;
        while (j >= 0 && chs.find(s[j]) != string::npos)
            --j;
        ++j;
        return i >= j ? "" : s.substr(i, j - i);
    }

    template<class ConstPointer>
    inline string join(string s, ConstPointer b, ConstPointer e)
    {
        string r;
        while (b < e)
            r += *b++ + s;
        return r.empty() ? r : (r.erase(r.end() - s.size(), r.end()), r);
    }

    inline string join(string s, const vector<string>& src_vector)
    {
        return join(s, src_vector.begin(), src_vector.end());
    }

	/**
	  @brief erase the space in the front and back of the string
	*/
	inline void trim(string& str, string tag = " \t")
	{
		if (str.empty())
		{
			return ;
		}
		string::size_type left = str.find_first_not_of( tag );
        if (left == string::npos)
        {
            str = "";
            return ;
        }
		string::size_type right = str.find_last_not_of( tag );
		str = str.substr( left, right - left + 1 );
	}

    /**
      @brief eg. split("   a   b", " ") will return [a, b].
      @param[in] delimit view delimit as a whole string.
     */
    inline vector<string> split(const string& src, string delimit)
    {
        vector<string> r;
        int f = 0;
        int p = -1;

        while ((p = (int)src.find(delimit, p + 1)) != (int)string::npos)
        {
            if (p > f)
                r.push_back(src.substr(f, p - f));				
            f = p + (int)delimit.size();
        }
        p = (int)src.size();
        if (p > f)
            r.push_back(src.substr(f, p - f));				
        return r;
    }

    /**
      @brief replace "sub" in "s" as "nstr".
    */
    inline string replace(const string& s, string sub, string nstr)
    {
        int pos = 0;
        string ns = s;
        while ((pos = ns.find(sub, pos)) != (int)string::npos)
        {
            ns = ns.replace(pos, sub.size(), nstr);
            pos += (int)nstr.size();
        }
        return ns;
    }

    /**
     * @brief split string by any blank char.
     * */
    inline vector<string> split(const string& src)
    {
        istringstream iss(src);
        vector<string> r;
        string w;
        while (iss >> w)
            r.push_back(w);
        return r;
    }

	/** added by luowei : 2010-12-21
	* @brief lowercase the string
	* */
	inline int lowerStr(string & str)
	{
		for (string::size_type index = 0; index < str.size(); ++index)
		{
			char &ch = str.at(index);
			if ('A' <= ch && ch <= 'Z')
			{
				ch += 'a' - 'A';
			}
		}
		return 0;
	}

	/** added by luowei : 2010-12-21
	 * @brief split string with ne translation
	 * */
	inline int split(const string& src, vector<string>& vecWord, map<string, vector<string> >& mapNETrans)
	{
		vecWord.clear();
		mapNETrans.clear();

		string::size_type current = 0;
		string::size_type beg_words = 0;
		string::size_type end_words = 0;
		vector<string>::size_type beg_vec_index = 0;
		vector<string>::size_type end_vec_index = 0;
		vector<string> vecStr;

		while (true)
		{
			beg_words = current;
			current = src.find("english=\"", current);
			if (current == string::npos)
				break;

			end_words = src.rfind("<", current);

			// not NE
			string str_sub = src.substr(beg_words, end_words - beg_words);
			trim(str_sub);
			if (!str_sub.empty())
			{
				vecStr.clear();
				vecStr = split(str_sub);
				vecWord.insert(vecWord.end(), vecStr.begin(), vecStr.end() );				
			}
			beg_vec_index = vecWord.size();

			//获取英文翻译和中文词语
			string::size_type eleft = current + 9;
			string::size_type eright = src.find_first_of("\"", eleft);
			string english = src.substr(eleft, eright - eleft);
			string::size_type cleft = src.find_first_of(">", eright);
			string::size_type cright = src.find_first_of("<", eleft);
			string chinese = src.substr(cleft + 1, cright - cleft - 1);
			string::size_type end_NE_Type = src.find(">", cright);

			//去掉chinese的空格
			trim(chinese);
			vecStr.clear();
			vecStr = split(chinese);
			vecWord.insert(vecWord.end(), vecStr.begin(), vecStr.end() );
			end_vec_index = vecWord.size() - 1;

			//对english进行处理
			vecStr.clear();
			vecStr = split(english, "|");
			vector<string>::iterator iterVec;
			for (iterVec = vecStr.begin(); iterVec != vecStr.end(); ++iterVec)
			{
				trim(*iterVec);
				lowerStr(*iterVec);
			}
			mapNETrans[chinese] = vecStr;

			current = end_NE_Type + 1;
		}
		if (beg_words != src.size())
		{
			string str_sub = src.substr(beg_words);
			trim(str_sub);
			vecStr.clear();
			vecStr = split(str_sub);
			vecWord.insert(vecWord.end(), vecStr.begin(), vecStr.end() );
		}

		return 0;
	}

	/** added by luowei : 2010-12-21
	 * @brief split string with ne translation
	 * */
	inline int split(const string& src, vector<string>& vecWord, map<pair<unsigned int, unsigned int>, vector<string> >& mapNESpans)
	{
		vecWord.clear();
		mapNESpans.clear();

		string::size_type current = 0;
		string::size_type beg_words = 0;
		string::size_type end_words = 0;
		vector<string>::size_type beg_vec_index = 0;
		vector<string>::size_type end_vec_index = 0;
		vector<string> vecStr;

		while (true)
		{
			beg_words = current;
			current = src.find("english=\"", current);
			if (current == string::npos)
				break;

			end_words = src.rfind("<", current);

			// not NE
			string str_sub = src.substr(beg_words, end_words - beg_words);
			trim(str_sub);
			if (!str_sub.empty())
			{
				vecStr.clear();
				vecStr = split(str_sub);
				vecWord.insert(vecWord.end(), vecStr.begin(), vecStr.end() );				
			}
			beg_vec_index = vecWord.size();

			//获取英文翻译和中文词语
			string::size_type eleft = current + 9;
			string::size_type eright = src.find_first_of("\"", eleft);
			string english = src.substr(eleft, eright - eleft);
			string::size_type cleft = src.find_first_of(">", eright);
			string::size_type cright = src.find_first_of("<", eleft);
			string chinese = src.substr(cleft + 1, cright - cleft - 1);
			string::size_type end_NE_Type = src.find(">", cright);

			//去掉chinese的空格
			trim(chinese);
			vecStr.clear();
			vecStr = split(chinese);
			vecWord.insert(vecWord.end(), vecStr.begin(), vecStr.end() );
			end_vec_index = vecWord.size() - 1;

			//对english进行处理
			vecStr.clear();
			vecStr = split(english, "|");
			vector<string>::iterator iterVec;
			for (iterVec = vecStr.begin(); iterVec != vecStr.end(); ++iterVec)
			{
				trim(*iterVec);
				lowerStr(*iterVec);
			}
			mapNESpans[ make_pair(beg_vec_index, end_vec_index) ] = vecStr;

			current = end_NE_Type + 1;
		}
		if (beg_words != src.size())
		{
			string str_sub = src.substr(beg_words);
			trim(str_sub);
			vecStr.clear();
			vecStr = split(str_sub);
			vecWord.insert(vecWord.end(), vecStr.begin(), vecStr.end() );
		}

		return 0;
	}

    /**
      @brief return a random sequence in [begin, end).
     */
    inline int rand_range(int begin, int end)
    {
        assert(begin < end);
        return int(rand64() % (end - begin)) + begin;
    }

    /**
      @brief return a random sequence in [0, size).
     */
    inline vector<int> rand_range(int size)
    {
        vector<int> v(size);
        for (int i = 0; i < size; ++i)
            v[i] = i;
        vector<int> r;
        r.reserve(size);
        while (!v.empty())
        {
            int rd = rand_range(0, (int)v.size());
            r.push_back(v[rd]);
            swap(v[rd], v.back());
            v.pop_back();
        }
        return r;
    }

    inline string to_string(double m)
    {		
        char r[64];
        int len;
        r[len = sprintf(r, "%.12f", m)] = '\0';
        int i;
        for (i = 0; i < len && r[i] != '.'; ++i)
        {}
        int j;
        for (j = len - 1; j >= 0 && j > i && r[j] == '0'; r[j--] = '\0')
        {}
        if (i == j)
            r[i] = '\0';

        return string(r);
    }	

    inline string to_string(int m)
    {
        return to_string((double)m);
    }

    inline double to_double(const char* s)
    {
        return atof(s);
    }

    inline double to_double(const string& s)
    {
        return to_double(s.c_str());
    }

    inline int	to_int(const char* s)
    {
        return (int)to_double(s);
    }

    inline int	to_int(const string& s)
    {
        return to_int(s.c_str());
    }

    /**
      @brief fastly read a text file into a string. 
     */
    inline void read_txt(string file, string& content, int capacity = 1024 * 100)
    {
        vector<char> buffer(1024);
        ifstream fin(file.c_str());
        assert(fin.good() || !(cerr << "can't read " << file << endl));

        content.clear();
        content.reserve(max(1024 * 100, capacity));

        while (fin.read(&buffer[0], (int)buffer.size()), fin.gcount() > 0)
            content.append(&buffer[0], &buffer[fin.gcount()]);
    }

    /**
      @brief save the string into a textfile.
      @param[in] append open file mode.	
     */
    inline void save_txt(string file, const string& content, bool append = false)
    {
        ofstream fou;
        if (append)
            fou.open(file.c_str(), ios_base::app);
        else
            fou.open(file.c_str());
        assert(fou.good() || !(cerr << "can't write to " << file << endl));
        fou.write(content.c_str(), (int)content.size());
    }

    inline UINT64 hash(const string &s)
    {
        static bool first = true;
        static UINT64 hashbit[1024][256];

        assert(s.size() < 1024 || !(cerr << (int)s.size() << endl));
        if (first)
        {
            first = !first;
            //srand(time(NULL));
            srand(0);
            for (int pos = 0; pos < 1024; ++pos)
            {
                for (int ch = 0; ch < 256; ++ch)
                    hashbit[pos][ch] = rand64();
            }
        }

        UINT64 r = 0;
        for (int i = 0; i < (int)s.size(); ++i)
            r ^= hashbit[i][(unsigned char)s[i]];
        return r;
    }

    inline bool eq(double m, double n, double prec = ZERO)
    {
        return fabs(m - n) < prec;
    }

    inline bool ge(double m, double n, double prec = ZERO)
    {
        return m > n || eq(m, n, prec);
    }

    /**
     * @brief judge whether "str" start with "prefix", compatible with python function.
     * */
    inline bool startswith(const string &str, string prefix)
    {
        return prefix.size() <= str.size() && strncmp(str.c_str(), prefix.c_str(), prefix.size()) == 0;
    }

    /**
     * @brief judge whether "str" end with "postfix", compatible with python function.
     * */
    inline bool endswith(const string& str, string postfix)
    {
        return postfix.size() <= str.size() && strcmp(str.c_str() + (str.size() - postfix.size()), postfix.c_str()) == 0;
    }

    /**
     * @brief return the content between "le" and "ri" in "str". 
     * e.g. match_str("<summer><rain>", "<", ">") is "summer", and return the postion next
     * to match.
     * */
    inline int match_str(const string& str, string le, string ri, string& con, int fpos = 0)
    {
        int npos = (int)string::npos;
        int p1;
        int p2;
        if ((p1 = (int)str.find(le, fpos)) != npos && (p2 = (int)str.find(ri, p1 + 1)) != npos)
        {
            con = str.substr(p1 + (int)le.size(), p2 - p1 - (int)le.size());
            return p2 + (int)ri.size();
        }
        return npos;
    }
}

namespace rain
{
    template<class Type1, class Type2, class Type3>
    class Triple
    {
    public:
        Triple(const Type1& first = Type1(), const Type2& second = Type2(), const Type3& third = Type3()):
            first(first), second(second), third(third)
        {}

        bool operator < (const Triple& ot) const
        {
            if (first != ot.first)
                return first < ot.first;
            else if (second != ot.second)
                return second < ot.second;
            else 
                return third < third;
        }

        Type1   first;
        Type2   second;
        Type3   third;
    };

    class Format
    {
    public:
        Format(const string& fmt): str(fmt)
        {}

        Format operator () (const string& sub, const string& nstr) const
        {
            return Format(replace(str, sub, nstr));
        }

        Format operator () (const string& sub, int nstr) const
        {
            return Format(replace(str, sub, to_string(nstr)));
        }

        Format operator () (const string& sub, double nstr) const
        {
            return Format(replace(str, sub, to_string(nstr)));
        }

        string operator () () const
        {
            return str;
        }

        friend ostream& operator << (ostream &out, const Format& fmt) 
        {
            return (out << fmt.str);
        }
    protected:    
        string  str;
    };

    class CountTime
    {
    public:
        CountTime(string inf = "counting time"): inf(inf)/*, start(clock())*/
        {
            start = clock();
            cout << "*****---------------------------------------" << '\n';
            cout << Format("(start - {inf})")("{inf}", inf) << endl;
        }

        ~CountTime()
        {
            finish = clock();
            double tsecond = (finish - start) / (double)CLOCKS_PER_SEC;
            int h = (int)(tsecond / (60 * 60));
            int m = (int)((tsecond - h * 60 * 60) / 60);
            double s = tsecond - h * 60 * 60 - m * 60;

            Format fmt("(finish - {inf}): {h} h {m} m {s} s");
            cout << fmt("{inf}", inf)("{h}", h)("{m}", m)("{s}", s) << '\n';
            cout << "----------------------------------------*****" << endl;
        }
    private:
        string  inf;
        time_t  start;
        time_t  finish;
    };

    template<class Type1, class Type2, class TypeValue>
    class Dict
    {
    public:
        typedef             map<Type2, TypeValue>               DictClumn;
        typedef typename    map<Type1, DictClumn>::iterator     Iterator1;
        typedef typename    DictClumn::iterator                 Iterator2;

        void clear()
        {
            dict_mps.clear();
        }

        bool exist(const Type1& w1, const Type2& w2, TypeValue& value)
        {
            Iterator1 ite1 = dict_mps.find(w1);
            Iterator2 ite2;
            return ite1 != dict_mps.end() && (ite2 = ite1->second.find(w2)) != ite1->second.end() ? (value = ite2->second, true) : false;
        }

        TypeValue get(const Type1& w1, const Type2& w2, const TypeValue& df_value)
        {
            TypeValue value;
            return exist(w1, w2, value) ? value : df_value;
        }

        TypeValue& operator () (const Type1& w1, const Type2& w2)
        {
            return dict_mps[w1][w2];
        }

        pair<Iterator1, Iterator1> iterator()
        {
            return make_pair(dict_mps.begin(), dict_mps.end());
        }

        pair<Iterator2, Iterator2> iterator(const Type1& w1)
        {
            if (dict_mps.find(w1) != dict_mps.end())
                return make_pair(dict_mps[w1].begin(), dict_mps[w1].end());
            else
                return make_pair(Iterator2(), Iterator2());
        }

    private:
        map<Type1, DictClumn>  dict_mps;
    };
}

#endif

