#pragma once
#include "SettingsFile.h"
struct TRADES_STATISTIC
{
	long long profit, loss, iWon, iLost, timeIn, timeTotal;
	double custom;
};

class TradeFile
{
public:
	using TradeList = std::vector<TradeSummary>;
public:
	TradeFile() = default;
	TradeFile(TradeFile const&) = default;
	TradeFile(TradeFile&&) = default;
	TradeFile(fs::path const&);
	TradeFile(std::string);

	TradeFile& operator=(TradeFile const&) = default;
	TradeFile& operator=(TradeFile&&) = default;

	TradeFile& operator+=(TradeFile const&);
	TradeFile operator+(TradeFile const&)const;

	inline double Net()const { return (m_Stat.profit - m_Stat.loss) / 100.; }
	inline TRADES_STATISTIC const& GetStats()const { return m_Stat; }
	inline std::vector<TradeSummary> const& GetTrades()const { return m_Trades; }
	inline int GetID()const { return m_ID; }
	void SetID(int id) { m_ID = id; }
	inline fs::path const& GetFilepath()const { return m_Filepath; }
	fs::path GetSettingsFilePath()const;
	CString GetSettingsFileText()const;

private:
	void ParseText(std::istream&);
	static TradeSummary ParseLine(std::string const& line);
	void UpdateStatistics();
	std::vector<double> GetCumulative()const;
	static bool CalculateLinearRegression(std::vector<double> const chartline, double& k, double& b);
	static bool  CalculateStdError(std::vector<double> const& data, double  a_coef, double  b_coef, double& std_err);

private:
	fs::path m_Filepath;
	int m_ID;
	TRADES_STATISTIC m_Stat;
	TradeList m_Trades;
};

namespace Scan
{
	using model_pair = std::pair<TradeFile const*, TradeFile const*>;
	using pair_result = std::tuple<double, model_pair>;
	using pair_vector = std::vector<pair_result>;
}

