#include "pch.h"
#include "ScanDefines.h"

namespace Scan
{
	int ModelBunch::InTimeMin, ModelBunch::InTimeMax, ModelBunch::TradesMin, ModelBunch::TradesMax, ModelBunch::ConMin, ModelBunch::ConMax;

	void Timeline::insert(__time32_t t, double v)
	{
		while (pts.contains(t))++t;
		pts[t] = v;
	}

	Timeline::Timeline(TradeFile const& tf)
	{
		for (auto& t : tf.GetTrades())
			insert(t.close_time, t.profit);
	}

	void Timeline::operator+=(Timeline const& oth)
	{
		for (auto& val : oth.pts)
			insert(val.first, val.second);
	}

	void ModelBunch::Add(TradeFile const* pFile)
	{
		subModels.push_back(pFile);
	}

	bool ModelBunch::Calculate()
	{
		m_Inters = {};

		for (auto pFile : subModels)
			m_Inters += *pFile;

		auto in_time{ m_Inters.GetInTime() * 100. / m_Inters.GetTotalTime() };
		if (in_time<InTimeMin || in_time>InTimeMax)
			return false;
		if (m_Inters.GetTradeCount() < TradesMin || m_Inters.GetTradeCount() > TradesMax)
			return false;
		in_time = m_Inters.GetConstructive() * 100. / m_Inters.GetInTime();
		if (in_time<ConMin || in_time>ConMax)
			return false;

		Timeline tl;

		for (auto pFile : subModels)
			tl += *pFile;

		combined = tl;
		combined /= subModels.size();

		double profit{ .0 }, loss{ .0 };
		int won{ 0 }, lost{ 0 };
		auto& data{ combined.GetData() };
		Net = .0;

		for (auto& val : data)
		{
			if (val.second > Net)
			{
				++won;
				profit += val.second - Net;
			}
			else
			{
				++lost;
				loss += Net - val.second;
			}
			Net = val.second;
		}

		Factor = profit / loss;
		Custom = combined.GetCustom();

		return true;
	}

	bool ModelBunch::operator==(ModelBunch const& oth) const
	{
		if (subModels.size() != oth.subModels.size())
			return false;

		for (auto p : subModels)
			if (!oth.has(p))
				return false;

		return true;
	}

	bool ModelBunch::has(TradeFile const* p) const
	{
		for (auto val : subModels)
			if (val == p)
				return true;
		return false;
	}

	bool ModelBunch::has_one_file(ModelBunch const& oth) const
	{
		for (auto p : oth.GetSubModels())
			if (has(p))
				return true;
		return false;
	}

	CHART_DIM Cummulative::GetDim() const
	{
		CHART_DIM dim{};

		if (!pts.empty())
		{
			auto iter{ pts.begin() };
			dim.start = dim.finish = iter->first;
			dim.high = dim.low = iter->second;

			for (auto& val : pts)
			{
				dim.finish = val.first;
				dim.high = max(dim.high, val.second);
				dim.low = min(dim.low, val.second);
			}
		}

		return dim;
	}

	Cummulative::Cummulative(Scan::Timeline const& line)
	{
		double cumm{ .0 };
		pts.reserve(line.GetPoints().size());

		for (auto& item : line.GetPoints())
		{
			cumm += item.second;
			pts.emplace_back(item.first, cumm);
		}
		CalculateLinearRegression();
		CalculateStdError();
		custom = lr_k * pts.size();
		if (std_err != .0)
			custom /= std_err;
	}

	void Cummulative::operator/=(size_t num)
	{
		for (auto& val : pts)
			val.second /= num;
	}

	void CHART_DIM::operator+=(CHART_DIM const& oth)
	{
		high = max(high, oth.high);
		low = min(low, oth.low);
		start = min(start, oth.start);
		finish = max(finish, oth.finish);
	}

	CompareObj::CompareObj(int maxModels, BOOL bNet, BOOL bFactor, BOOL bCustom)
	{
		Init(maxModels, bNet, bFactor, bCustom);
	}

	/*void CompareObj::operator+=(BestModels&& bm)
	{
		for (auto& m : bm)
			Test(m);
	}*/

	void CompareObj::Init(int maxModels, BOOL bNet, BOOL bFactor, BOOL bCustom)
	{
		CSingleLock _o{ &m_CS, TRUE };
		if (bNet)
		{
			bestNet = std::make_unique<BestModels>();
			bestNet->maxModelCount = maxModels;
		}
		if (bFactor)
		{
			bestFactor = std::make_unique<BestModels>();
			bestFactor->maxModelCount = maxModels;
		}
		if (bCustom)
		{
			bestCustom = std::make_unique<BestModels>();
			bestCustom->maxModelCount = maxModels;
		}
	}

	void CompareObj::Reset(int maxModelCount)
	{
		Init(maxModelCount, (bool)bestNet, (bool)bestFactor, (bool)bestCustom);
	}

	bool CompareObj::Test(const ModelBunch& mb)
	{
		CSingleLock _o{ &m_CS, TRUE };
		bool bUpdated{ false };

		if (bestNet)
			bUpdated |= bestNet->update<BestModels::Net>(mb);
		if (bestFactor)
			bUpdated |= bestFactor->update<BestModels::Factor>(mb);
		if (bestCustom)
			bUpdated |= bestCustom->update<BestModels::Custom>(mb);

#ifdef DEBUG0
		auto compare_best_single = [](BestModels const& bm)
			{
				for (auto it_a = bm.begin(); it_a != bm.end(); it_a++)
					for (auto it_b = std::next(it_a); it_b != bm.end(); it_b++)
						assert(*it_a != *it_b);
			};
		auto compare_best_double = [](BestModels const&, BestModels const&)
			{
			};
		if (bestNet)
			compare_best_single(*bestNet);
		if (bestFactor)
			compare_best_single(*bestFactor);
		if (bestCustom)
			compare_best_single(*bestCustom);
		if (bestNet && bestFactor)
			compare_best_double(*bestNet, *bestFactor);
		if (bestNet && bestCustom)
			compare_best_double(*bestNet, *bestCustom);
		if (bestFactor && bestCustom)
			compare_best_double(*bestFactor, *bestNet);
#endif // DEBUG

		return bUpdated;
	}

	BestModels CompareObj::GetBest() const
	{
		BestModels ret;

		auto find = [&ret](ModelBunch const& m)->bool
			{
				for (auto& model : ret)
					if (model == m)
						return true;
				return false;
			};

		auto add_block = [&ret, find](BestModels const* pBest)
			{
				if (pBest)
					if (ret.empty())
						ret = *pBest;
					else for (auto& val : *pBest)
						if (!find(val))
							ret.push_back(val);
			};

		CSingleLock _o{ &m_CS, TRUE };
		add_block(bestNet.get());
		add_block(bestFactor.get());
		add_block(bestCustom.get());

		return ret;
	}

	void Cummulative::CalculateLinearRegression()
	{
		lr_k = lr_b = .0;
		size_t const N{ pts.size() };
		//--- check for data sufficiency
		if (N < 3)
			return;

		//--- now, calculate regression ratios
		double x{ .0 }, y{ .0 }, x2{ .0 }, xy{ .0 };
		size_t index{ 0ull };

		for (auto& val : pts)
		{
			x += index;
			y += val.second;
			xy += index * val.second;
			x2 += index * index;

			++index;
		}

		lr_k = (N * xy - x * y) / (N * x2 - x * x);
		lr_b = (y - lr_k * x) / N;
	}
	//+------------------------------------------------------------------+
	//|  Calculate mean-square deviation error for specified a and b     |
	//+------------------------------------------------------------------+
	void  Cummulative::CalculateStdError()
	{
		std_err = .0;

		//--- sum of error squares
		double error{ .0 };
		auto N{ pts.size() };
		if (N <= 2)
			return;

		auto sq = [](auto val) {return val * val; };

		for (size_t i = 0; i < N; ++i)
			error += sq(lr_k * i + lr_b - pts[i].second);

		std_err = std::sqrt(error / (N - 2));
	}

	BestModels::iterator BestModels::test_for_unique(ModelBunch const& mb)
	{
		auto iter{ begin() };

		while (iter != end())
			if (iter->has_one_file(mb))
				break;
			else ++iter;

		return iter;
	}
}