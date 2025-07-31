#include "pch.h"
#include "Trades.h"

TradeFile::TradeFile(fs::path const& fname)
	:m_Filepath{ fname }
	, m_Stat{}
{
	std::ifstream file{ fname };
	std::string line;

	if (file.is_open())
		ParseText(file);

	UpdateStatistics();
}

TradeFile::TradeFile(std::string text)
{
	std::istringstream iss{ text };
	ParseText(iss);
}

TradeFile TradeFile::operator+(TradeFile const& oth) const
{
	//Timeline line;
	//line += *this;
	//line += oth;

	//for(auto &item:line)


	return *this;
}

CString TradeFile::GetSettingsFileText() const
{
	CString ret;
	CFile file;
	if (file.Open(GetSettingsFilePath().wstring().c_str(), CFile::modeRead))
	{
		auto const bytes{ file.GetLength() };
		auto buff{ std::make_unique<WCHAR[]>(bytes / sizeof WCHAR + 1) };
		file.Read(buff.get(), (UINT)bytes);
		buff[bytes / sizeof WCHAR] = L'\0';
		ret.Insert(0, &buff[1]);
	}
	return ret;
}

fs::path TradeFile::GetSettingsFilePath() const
{
	auto ret{ m_Filepath };
	ret.replace_extension(".set");
	return ret;
}

void TradeFile::ParseText(std::istream& iss)
{
	std::string line;

	while (std::getline(iss, line))
		m_Trades.push_back(ParseLine(line));
}

TradeSummary TradeFile::ParseLine(std::string const& line)
{
	constexpr auto delim{ '\t' };
	std::stringstream ss(line);
	std::string token;
	TradeSummary trade = {};

	auto to_time = [](std::string const& txt)->unsigned long
		{
			if (txt.length() != 19)
				return 0;

			int const
				year = std::stoi(txt.substr(0, 4)),
				mon = std::stoi(txt.substr(5, 2)),
				day = std::stoi(txt.substr(8, 2)),
				hour = std::stoi(txt.substr(11, 2)),
				min = std::stoi(txt.substr(14, 2)),
				sec = std::stoi(txt.substr(17, 2));
			return (unsigned long)CTime(year, mon, day, hour, min, sec).GetTime();
		};

	try
	{
		// Parse is_buy (true/false or 1/0)
		std::getline(ss, token, delim);
		//token.erase(0, token.find_first_not_of(" \t"));
		//token.erase(token.find_last_not_of(" \t") + 1);
		trade.is_buy = token == "buy";

		// Parse open_time
		std::getline(ss, token, delim);
		//token.erase(0, token.find_first_not_of(" \t"));
		//token.erase(token.find_last_not_of(" \t") + 1);
		trade.open_time = to_time(token);

		// Parse close_time
		std::getline(ss, token, delim);
		//token.erase(0, token.find_first_not_of(" \t"));
		//token.erase(token.find_last_not_of(" \t") + 1);
		trade.close_time = to_time(token);

		// Parse open_price
		std::getline(ss, token, delim);
		trade.open_price = std::stod(token);

		// Parse close_price
		std::getline(ss, token, delim);
		trade.close_price = std::stod(token);

		// Parse volume
		std::getline(ss, token, delim);
		trade.volume = std::stod(token);

		// Parse profit
		std::getline(ss, token, delim);
		trade.profit = std::stod(token);

		// Add to vector

	}
	catch (const std::exception&)
	{
		trade = {};
	}

	return trade;
}

void TradeFile::UpdateStatistics()
{
	long long val;
	ZeroMemory(&m_Stat, sizeof TRADES_STATISTIC);
	if (m_Trades.empty())
		return;

	for (auto const& t : m_Trades)
	{
		val = (long long)std::round(t.profit * 100.);
		if (val > 0)
		{
			m_Stat.profit += val;
			++m_Stat.iWon;
		}
		else
		{
			m_Stat.loss -= val;
			++m_Stat.iLost;
		}
		m_Stat.timeIn += t.close_time - t.open_time;
	}

	m_Stat.timeTotal = m_Trades.back().close_time - m_Trades.front().open_time;

	double k, b, err;
	auto const cumm{ GetCumulative() };
	CalculateLinearRegression(cumm, k, b);
	CalculateStdError(cumm, k, b, err);
	m_Stat.custom = k * m_Trades.size();
	if (err != .0)
		m_Stat.custom /= err;
}

//--- create a chart array with an accumulation
std::vector<double> TradeFile::GetCumulative()const
{
	//--- check for data sufficiency
	if (m_Trades.size() < 3)
		return {};

	std::vector<double> ret(m_Trades.size());
	auto itChange{ m_Trades.begin() };
	ret.front() = m_Trades.front().profit;

	for (auto b4{ ret.begin() }, iter{ std::next(b4) }; iter != ret.end(); ++itChange, b4 = iter++)
		*iter = *b4 + itChange->profit;

	return ret;
}

bool TradeFile::CalculateLinearRegression(std::vector<double> const chartline, double& k, double& b)
{
	size_t const N{ chartline.size() };
	//--- check for data sufficiency
	if (N < 3)
		return false;

	//--- now, calculate regression ratios
	double x{ .0 }, y{ .0 }, x2{ .0 }, xy{ .0 };
	size_t index{ 0ull };

	for (auto val : chartline)
	{
		x += index;
		y += val;
		xy += index * val;
		x2 += index * index;

		++index;
	}

	k = (N * xy - x * y) / (N * x2 - x * x);
	b = (y - k * x) / N;
	//---
	return true;
}
//+------------------------------------------------------------------+
//|  Calculate mean-square deviation error for specified a and b     |
//+------------------------------------------------------------------+
bool  TradeFile::CalculateStdError(std::vector<double> const& data, double  a_coef, double  b_coef, double& std_err)
{
	//--- sum of error squares
	double error{ .0 };
	auto N{ data.size() };
	if (N <= 2)
		return (false);

	auto sq = [](auto val) {return val * val; };

	for (size_t i = 0; i < N; ++i)
		error += sq(a_coef * i + b_coef - data[i]);

	std_err = std::sqrt(error / (N - 2));
	//---
	return (true);
}
