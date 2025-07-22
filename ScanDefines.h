#pragma once
#include "Trades.h"
#include "Intervals.h"

namespace Scan
{
	struct CHART_DIM
	{
		double high, low;
		__time32_t start, finish;

		void operator+=(CHART_DIM const& oth);
	};

	class Timeline
	{
		std::map<__time32_t, double> pts;

	public:
		Timeline() = default;
		Timeline(TradeFile const& tf);

		void operator+=(Timeline const&);
		Timeline operator+(Timeline const&)const;
		void insert(__time32_t t, double v);

		std::map<__time32_t, double> const& GetPoints()const { return pts; }
	};

	class Cummulative
	{
	public:
		using point = std::pair<__time32_t, double>;

	public:
		Cummulative() = default;
		Cummulative(Scan::Timeline const&);

		void operator/=(size_t);

		CHART_DIM GetDim()const;
		inline size_t GetCount()const { return pts.size(); }
		inline auto& GetData()const { return pts; }
		inline double GetCustom()const { return custom; }

	private:
		void CalculateLinearRegression();
		void  CalculateStdError();

	private:
		std::vector<point> pts;
		double lr_k, lr_b, std_err, custom;
	};

	class ModelBunch
	{
		std::vector<TradeFile const*> subModels;
		Cummulative combined;
		double Net, Factor, Custom;
		size_t inTime;
		Intervals m_Inters;

	public:
		void Add(TradeFile const*);
		void Calculate();
		inline double GetNet()const { return Net; }
		inline double GetFactor()const { return Factor; }
		inline double GetCustom()const { return Custom; }
		inline Cummulative const& GetCummulative()const { return combined; }
		bool operator==(ModelBunch const&)const;
		bool has(TradeFile const*)const;
		inline std::vector<TradeFile const*> const& GetSubModels()const { return subModels; }
		Intervals const& GetIntervals()const { return m_Inters; }
	};

	using BestModels = std::vector<Scan::ModelBunch>;

	class CompareObj
	{
		int maxModelCount;
		std::unique_ptr<BestModels> bestNet, bestFactor, bestCustom;

	public:
		CompareObj() = default;
		CompareObj(int maxModels, BOOL bNet, BOOL bFactor, BOOL bCustom);

		void operator+=(BestModels&&);

		void Init(int maxModels, BOOL bNet, BOOL bFactor, BOOL bCustom);
		void Reset();

		bool Test(const ModelBunch&);
		BestModels GetBest()const;
	};
}