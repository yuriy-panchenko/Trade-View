#include "pch.h"
#include "DrawChart.h"

namespace Draw
{
	constexpr COLORREF
		color_combined{ RGB(180,180,0) },
		color_zero{ RGB(100,100,100) };

	void CummulativeChart::Add(TradeFile const& tf, COLORREF col)
	{
		lines.emplace_back(Scan::Timeline{ tf }, col);
		combined += tf;
	}

	void CummulativeChart::Draw(CDC& dc, CRect const& canvas)
	{
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, canvas.Width(), canvas.Height());
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		int const iSave = memDC.SaveDC();
		auto pOldBmp{ memDC.SelectObject(&bmp) };
		memDC.SetBkMode(TRANSPARENT);

		LINE_DATA cmb{ combined, color_combined };
		cmb.cumm /= lines.size();
		auto dim{ cmb.cumm.GetDim() };

		for (auto& line : lines)
			dim += line.cumm.GetDim();

		CPen zeroPen{ PS_DOT, 1, color_zero };
		memDC.SelectObject(&zeroPen);
		auto rect{ canvas };
		rect.OffsetRect(-rect.TopLeft());
		memDC.FillSolidRect(rect, RGB(20, 20, 20));
		auto const zeroY{ (int)(rect.Height() * dim.high / (dim.high - dim.low)) };
		memDC.MoveTo(rect.left, zeroY);
		memDC.LineTo(rect.right, zeroY);

		for (auto& line : lines)
			Draw(line, memDC, rect, dim);

		if (lines.size() > 1)
			Draw(cmb, memDC, rect, dim, 2, PS_SOLID);

		dc.BitBlt(canvas.left, canvas.top, canvas.Width(), canvas.Height(), &memDC, 0, 0, SRCCOPY);
		memDC.RestoreDC(iSave);
	}

	void CummulativeChart::Draw(LINE_DATA& data, CDC& dc, CRect const& canvas, Scan::CHART_DIM const& dim, int width, int style)
	{
		if (data.cumm.GetCount() < 2)
			return;

		auto const width_sec{ dim.finish - dim.start };
		auto const height_pts{ dim.high - dim.low };

		auto to_point = [&](auto const& p)->CPoint
			{
				return {
					canvas.left + (int)(canvas.Width() * (double)(p.first - dim.start) / width_sec),
					canvas.top + (int)(canvas.Height() * (dim.high - p.second) / height_pts) };
			};

		CPen pen{ style, width, data.col }, * pOldPen{ dc.SelectObject(&pen) };

		auto const& pts{ data.cumm.GetData() };

		auto iter{ pts.begin() };
		dc.MoveTo(to_point(*iter));

		for (++iter; iter != pts.end(); ++iter)
			dc.LineTo(to_point(*iter));

		dc.SelectObject(pOldPen);
	}
}
