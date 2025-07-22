#include "pch.h"
#include "Intervals.h"

namespace Scan
{
	Intervals::Intervals(TradeFile const& file)
	{
		for (auto& v : file.GetTrades())
		{
			m_Positions[v.open_time] += v.is_buy ? 1 : -1;
			m_Positions[v.close_time] += v.is_buy ? -1 : 1;
		}
	}

	void Intervals::operator+=(Intervals const& oth)
	{
		if (m_Positions.empty())
			m_Positions = oth.m_Positions;
		else
		{
			int pos_a{ 0 }, pos_b{ 0 };
			__time32_t con_start{}, de_start{}, in_time{};

			m_uTradeCount = 0ull;

			auto is_constructive = [&] {return (pos_a > 0 && pos_b > 0) || (pos_a < 0 && pos_b < 0); };
			auto is_destructive = [&] {return (pos_a > 0 && pos_b < 0) || (pos_a < 0 && pos_b > 0); };
			auto is_open_trade = [&] {return pos_a + pos_b; };
			auto check_code = [&](__time32_t const t)
				{
					if (is_constructive())
					{
						if (!con_start)
							con_start = t;
					}
					else if (is_destructive())
					{
						if (!de_start)
							de_start = t;
					}
					else
					{
						if (con_start)
							m_conTime += t - con_start;

						if (de_start)
							m_deTime += t - de_start;

						con_start = de_start = 0;
					}

					if (is_open_trade())
					{
						if (!in_time)
							in_time = t;
					}
					else
					{
						if (in_time)
						{
							m_inTime += t - in_time;
							++m_uTradeCount;
						}
						in_time = 0;
					}
				};

			auto it_a{ m_Positions.begin() };
			auto it_b{ oth.m_Positions.begin() };
			m_conTime = m_deTime = m_inTime = 0;

			Positions res;

			while (it_a != m_Positions.end() && it_b != oth.m_Positions.end())
				if (it_a->first > it_b->first)
				{
					pos_b += it_b->second;
					check_code(it_b->first);
					res[it_b->first] += pos_a + pos_b;
					++it_b;
				}
				else if (it_a->first < it_b->first)
				{
					pos_a += it_a->second;
					check_code(it_a->first);

					res[it_a->first] += pos_a + pos_b;
					++it_a;
				}
				else
				{
					pos_a += it_a->second;
					pos_b += it_b->second;
					check_code(it_a->first);

					res[it_a->first] += pos_a + pos_b;
					++it_a, ++it_b;
				}

			std::swap(m_Positions, res);
		}
	}
}