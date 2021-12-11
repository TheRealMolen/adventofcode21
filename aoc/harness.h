#pragma once

#include <chrono>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

#include <Windows.h>
#include <wincon.h>




// --- how to use the harness ---
//
//test(3, day1(READ("+1\n-2\n+3\n+1")));
//test<string>("CABDFE", day7(LOAD(7t), 6));
//gogogo(day1(LOAD(1)));
//gogogo(day5(LOADSTR(5)));
//skip("because it needs fancy graphics");
//nest(D11Point{ 232,251,12 }, day11_2(42));
//nonono(day11_2(4455));
//nononoD(day9(string("466 players; last marble is worth 7143600 points")));


using namespace std;


// ---- COLOUR SHIZ ----
extern HANDLE hStdIn;
extern HANDLE hStdOut;
void initcolours();


inline ostream& COLOUR(ostream& _Ostr, WORD code)
{
    _Ostr.flush();
    SetConsoleTextAttribute(hStdOut, code);
    return (_Ostr);
}
inline ostream& DARK_RED(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_RED); }
inline ostream& RED(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_RED | FOREGROUND_INTENSITY); }
inline ostream& DARK_BLUE(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_BLUE); }
inline ostream& BLUE(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_BLUE | FOREGROUND_INTENSITY); }
inline ostream& DARK_GREEN(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_GREEN); }
inline ostream& GREEN(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_GREEN | FOREGROUND_INTENSITY); }
inline ostream& YELLOW(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); }
inline ostream& GOLD(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_RED | FOREGROUND_GREEN); }
inline ostream& CYAN(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY); }
inline ostream& DARK_CYAN(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_BLUE | FOREGROUND_GREEN); }
inline ostream& DARK_PURPLE(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_BLUE | FOREGROUND_RED); }
inline ostream& PURPLE(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY); }
inline ostream& GREY(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); }
inline ostream& WHITE(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); }
inline ostream& BLACK(ostream& _Ostr) { return COLOUR(_Ostr, 0); }
inline ostream& RESET(ostream& _Ostr) { return COLOUR(_Ostr, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); }

// GARLAND!
struct Garland
{
    int length;
    Garland(int length) : length(length) {/**/ }
};
inline Garland GARLAND(int length)
{
    return Garland(length);
}
inline ostream& operator<<(ostream& os, const Garland& g)
{
    os << YELLOW << "**";
    for (int i = 0; i<g.length; ++i)
    {
        os << RED << "o" << YELLOW << "*" << GREEN << "o" << YELLOW << "*";
    }
    os << "*" << RESET;
    return os;
}




// -----   STRING LISTS  -----
class stringlist : public vector<string>
{
public:

    static stringlist fromstring(const string& str)
    {
        istringstream is(str);
        return fromistream(is);
    }

    static stringlist fromfile(const string& fname)
    {
        string fn = "data/" + fname;
        ifstream ifs(fn);
        if (!ifs)
            throw string("couldn't open file " + fn);

        return fromistream(ifs);
    }

    static stringlist fromistream(istream& is)
    {
        stringlist lst;
        string s;
        while (getline(is, s))
            lst.push_back(s);

        return lst;
    }

    static stringlist fromcsv(const string& str)
    {
        stringlist lst;

        size_t start = 0;
        size_t sep;
        do {
            // TODO: FIXME: quotes...!
            sep = str.find(',', start);
            lst.push_back(move(str.substr(start, sep != string::npos ? sep - start : sep)));
            start = sep + 1;
        } while (sep != string::npos);

        return lst;
    }
};
inline ostream& operator<<(ostream& os, const stringlist& lst)
{
    for (const auto& s : lst)
    {
        os << s << '\n';
    }
    return os;
}

inline string stringfromfile(const string& fname)
{
    string str;

    ifstream ifs("data/" + fname);
    ifs.seekg(0, ios::end);
    auto size = ifs.tellg();
    if (size < 0)
        throw "couldn't discover file size o_O";

    str.reserve((size_t)size);
    ifs.seekg(0, ios::beg);

    str.assign((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());

    return str;
}


#define READ(str)       stringlist::fromstring(str)
#define LOAD(day)       stringlist::fromfile("day" #day ".txt")
#define LOADSTR(day)    stringfromfile("day" #day ".txt")


// ----- string wrangling -----
vector<string> split(const string& str, const string& delim);


// taken from: https://stackoverflow.com/a/217605
static inline void ltrim(string& s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
        }));
}
static inline void ltrim(string& s, const char* chars) {
    auto pos = s.find_first_not_of(chars);
    if (pos != string::npos)
        s.erase(s.begin(), s.begin() + pos);
}

static inline void rtrim(string& s) {
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
        }).base(), s.end());
}
static inline void rtrim(string& s, const char* chars) {
    auto pos = s.find_last_not_of(chars);
    if (pos != string::npos)
        s.erase(s.begin() + pos + 1, s.end());
}

static inline void trim(string& s) {
    ltrim(s);
    rtrim(s);
}
static inline void trim(string& s, const char* chars) {
    ltrim(s, chars);
    rtrim(s, chars);
}

static inline string ltrim_copy(string s) {
    ltrim(s);
    return s;
}
static inline string ltrim_copy(string s, const char* chars) {
    ltrim(s, chars);
    return s;
}

static inline string rtrim_copy(string s) {
    rtrim(s);
    return s;
}
static inline string rtrim_copy(string s, const char* chars) {
    rtrim(s, chars);
    return s;
}

static inline string trim_copy(string s) {
    trim(s);
    return s;
}
static inline string trim_copy(string s, const char* chars) {
    trim(s, chars);
    return s;
}


// read a delimiter that must match a certain string
// eg: is >> x >> "," >> y;
inline istream& operator>>(istream& is, const char* checkstr)
{
    // skip ws at the start of checkstr
    const char* currcheck = checkstr;
    while (isspace(*currcheck))
        ++currcheck;

    // skip ws at start of istream
    while (isspace(is.peek()))
        is.get();

    // go through checkstr char by char and ensure we get matching data out of is
    while (*currcheck)
    {
        if (is.peek() != *currcheck)
        {
            // oh no!
            int badval = is.peek();
            is.setstate(ios_base::failbit);

            throw format("unexpected input character. expected '{}'({}) but found '{}'({})", char(*currcheck), int(*currcheck), char(badval), int(badval));
        }

        is.get();
        ++currcheck;
    }

    return is;
}



// ----- profiling -----
class ScopeTimer
{
    string m_name;
    chrono::steady_clock::time_point m_start;

public:
    explicit ScopeTimer(const char* name) : m_name(name), m_start(chrono::high_resolution_clock::now())
    { /**/ }
    ~ScopeTimer()
    {
        const auto end = chrono::high_resolution_clock::now();
        const auto duration_ns = chrono::duration_cast<chrono::nanoseconds>(end - m_start).count();
        cout << m_name << " took " << duration_ns/1000 << "us" << endl;
    }
};


// ----- day harness -----
extern int gday;
extern int gpart;
extern int gtest;

template<typename TResult, typename TShouldBe = TResult>
bool test(TShouldBe shouldbe, TResult result)
{
    cout << "day" << gday << ",p" << gpart << ": test " << gtest << ": ";
    gtest++;

    if (result == shouldbe)
    {
        cout << GREEN << "PASS" << RESET;
    }
    else
    {
        cout << RED << "FAIL" << RESET << ": should be " << shouldbe << ", result was " << result << "\n";
    }

    cout << endl;

    return (result == shouldbe);
}

template<typename TResult>
void gogogo(TResult result)
{
    cout << "day" << gday << ",p" << gpart << ": FINAL RESULT: " << YELLOW << result << RESET << endl;

    if (gpart == 2)
    {
        gday++;
        gpart = 1;
        cout << "\n" << GARLAND(4) << "\n" << endl;
    }
    else
    {
        gpart++;
        cout << endl;
    }

    gtest = 1;
}

template<typename TResult>
void gogogo(TResult result, TResult correct)
{
    gogogo(result);

    if (result != correct)
    {
        cerr << RED << " !! regression error -> should have been " << YELLOW << correct << RESET << endl;
    }
}

void skip(const char* message = "cos it's really slow!");

void jumptoday(int day);

#define nest(...)
#define nonono(...) skip()

#ifdef _DEBUG
#define nononoD(...) skip("cos it's too slow for debug...")
#define nestD(...)
#define nD(expr)
#else
#define nononoD(...) gogogo(__VA_ARGS__)
#define nestD(...) test(__VA_ARGS__)
#define nD(expr) expr
#endif




int twinkleforever();

