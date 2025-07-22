#pragma once
#include "Trades.h"

namespace Scan
{
	class Intervals
	{
		using Positions = std::map<__time32_t, int>;

	public:
		Intervals() = default;
		Intervals(TradeFile const&);

		void operator+=(Intervals const&);
		inline size_t GetConstructive()const { return m_conTime; }
		inline size_t GetDestructive()const { return m_deTime; }
		inline size_t GetInTime()const { return m_inTime; }
		inline size_t GetTradeCount()const { return m_uTradeCount; }
		inline size_t GetTotalTime()const { return m_Positions.empty() ? 0ull : (m_Positions.rbegin()->first - m_Positions.begin()->first); }

	private:
		size_t m_conTime, m_deTime, m_inTime, m_uTradeCount;
		Positions m_Positions;
	};
}