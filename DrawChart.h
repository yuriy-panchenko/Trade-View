#pragma once
#include "ScanDefines.h"

namespace Draw
{
	struct LINE_DATA
	{
		Scan::Cummulative cumm;
		COLORREF col;
	};

	class CummulativeChart
	{
		std::vector<LINE_DATA> lines;
		Scan::Timeline combined;

	public:
		CummulativeChart() = default;
		void Add(TradeFile const&, COLORREF);
		void Draw(CDC&, CRect const&);
	private:
		void Draw(LINE_DATA& data, CDC&, CRect const&, Scan::CHART_DIM const&, int width = 1, int style = PS_DOT);
	};
}