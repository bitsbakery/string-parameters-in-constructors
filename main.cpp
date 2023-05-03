#include <iostream>
#include <memory>
#include <string.h>
#include "pretty.h"

struct Stats
{
	int cStrCtor	= 0;
	int defaultCtor = 0;
	int copyCtor	= 0;
	int copyAssign	= 0;
	int moveCtor	= 0;
	int moveAssign	= 0;

	void reset()
	{
		cStrCtor	= 0;
		defaultCtor = 0;
		copyCtor	= 0;
		copyAssign	= 0;
		moveCtor	= 0;
		moveAssign	= 0;
	}

	void printRow(pretty::Table& table, 
			   std::string&& testName, 
			   std::string&& stringSource) const
	{
		table.addRow(std::to_string(cStrCtor), 
					 std::to_string(defaultCtor), 
					 std::to_string(copyCtor), 
					 std::to_string(copyAssign), 
					 std::to_string(moveCtor), 
					 std::to_string(moveAssign),
					 std::move(testName), 
					 std::move(stringSource));
	}

	void printEmptyRow(pretty::Table& table) const
	{
		table.addRow("", "", "", "", "", "","","");
	}

	void printHeader(pretty::Table& table)
	{
		table.addRow("ctor(cstr)", "ctor()", "copy()", "copy=", "move()", "move=","","");
	}
};

class DbgStr
{
	std::size_t _size;
	std::unique_ptr<char[]> _s;

	static inline Stats _stats;
public:

	static Stats& stats() { return _stats; }

	operator std::string() const { return std::string{_s.get()}; }

	DbgStr(const char* cstring) :
		_size(strlen(cstring)+1),
		_s(std::make_unique<char[]>(_size))
	{
		++_stats.cStrCtor;
		memcpy(_s.get(), cstring, _size);
	}

	DbgStr() : _size(0)
	{
		++_stats.defaultCtor;
	}

	DbgStr(const DbgStr& other) :
		_size(other._size),
		_s(std::make_unique<char[]>(_size))
	{
		++_stats.copyCtor;
		memcpy(_s.get(), other._s.get(), _size);
	}

	DbgStr& operator=(const DbgStr& other)
	{
		++_stats.copyAssign;
		if (this == &other)
			return *this;

		_size = other._size;
		_s = std::make_unique<char[]>(_size);
		memcpy(_s.get(), other._s.get(), _size);

		return *this;
	}

	DbgStr(DbgStr&& other) noexcept :
		_size(other._size),
		_s(std::move(other._s))
	{
		++_stats.moveCtor;
		other._size = 0;
	}

	DbgStr& operator=(DbgStr&& other) noexcept
	{
		++_stats.moveAssign;
		if (this == &other)
			return *this;

		_size = other._size;
		_s = std::move(other._s);
		other._size = 0;

		return *this;
	}
};

struct UserConstRef
{
	DbgStr name;

	UserConstRef(const DbgStr& str) : name(str) { }
};

struct UserByVal
{
	DbgStr name;

	UserByVal(DbgStr s) : name(std::move(s)) { }
};

struct UserByRVal
{
	DbgStr name;

	UserByRVal(DbgStr&& s) : name(std::move(s)) { }
};

template <typename T>
concept StringConvertible = 
	std::is_convertible_v<T, DbgStr>;

struct UserPerfectFwd
{
	DbgStr name;

	template<StringConvertible T>
	UserPerfectFwd(T&& str) : name(std::forward<T>(str)) { }
};

template<typename TUser>
void Test(pretty::Table& table)
{
	constexpr static auto testString = "test string";

	if(table.numRows()>1)
		DbgStr::stats().printEmptyRow(table);

	auto printAndResetStats = [&table](std::string&& testName, 
									   std::string&& stringSource)
	{
		DbgStr::stats().printRow(
			table,
			std::string(typeid(TUser).name()) + std::move(stringSource),
			std::move(testName));

		DbgStr::stats().reset();
	};

	{
		TUser user { testString };
		printAndResetStats(user.name, " from a string literal");
	}

	{
		if constexpr (std::is_constructible_v<TUser, DbgStr&>)
		{
			DbgStr str { testString };
			TUser user { str };
			printAndResetStats(user.name, " from l-value reference");
		}
	}

	{
		DbgStr str { testString };
		TUser user { std::move(str) };
		printAndResetStats(user.name, " from r-value reference");
	}

	{
		TUser user{ DbgStr { testString } };
		printAndResetStats(user.name, " from r-value reference2");
	}
}

int main()
{
	pretty::Table table;

	DbgStr::stats().printHeader(table);

	Test<UserConstRef>(table);
	Test<UserByVal>(table);
	Test<UserByRVal>(table);
	Test<UserPerfectFwd>(table);

	pretty::Printer print;
	print.frame(pretty::FrameStyle::LineRounded);
	std::cout << print(table);

	std::cout << "done";
}