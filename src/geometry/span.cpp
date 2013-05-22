//-------------------------------------------------------------------
// File: span.cpp
// 
// Spans basic operations implementation
//
//  
//-------------------------------------------------------------------

#include <xutility>

#include "precision.h"

#include "span.h"

Spans SpanUnite(const Span& lh, const Span& rh)
{
	Spans res;
	if (SpanIsToTheLeft(lh, rh))
	{
		res.push_back(lh);
		res.push_back(rh);
		return res;
	}
	if (SpanIsToTheRight(lh, rh))
	{
		res.push_back(rh);
		res.push_back(lh);
	}

	res.push_back(Span(std::min(lh.Start, rh.Start), std::max(lh.End, rh.End)));
	return res;
}

Spans SpanUnite(const Spans& lh, const Spans& rh)
{
	Spans res;
	int resSize = 0;
	for each (auto lSpan in lh)
	{
		for each (auto rSpan in rh)
		{
			Spans united = SpanUnite(lSpan, rSpan);
			resSize = res.size();
			res.resize(resSize + united.size());
			std::copy(united.begin(), united.end(), res.begin() + resSize);
		}
	}
	return res;
}

Spans SpanIntersect(const Span& lh, const Span& rh)
{
	if (SpanIsToTheLeft(lh, rh) || SpanIsToTheRight(lh, rh))
	{
		return Spans();
	}

	Spans res;
	res.push_back(Span(std::max(lh.Start, rh.Start), std::min(lh.End, rh.End)));
	return res;
}

Spans SpanIntersect(const Spans& lh, const Spans& rh)
{
	Spans res;
	int   resSize = 0;
	for each (auto lSpan in lh)
	{
		for each (auto rSpan in rh)
		{
			Spans intersected = SpanIntersect(lSpan, rSpan);
			resSize = res.size();
			res.resize(resSize + intersected.size());
			std::copy(intersected.begin(), intersected.end(), res.begin() + resSize);
		}
	}
	return res;
}

Spans SpanDifference(const Span& lh, const Span& rh)
{
	if (SpanIsToTheRight(rh, lh) || SpanIsToTheLeft(rh, lh))
	{
		return Spans(1, lh);
	}

	// Fit negative to positive
	Span rhFit = Span(std::max(lh.Start, rh.Start), std::min(lh.End, rh.End));

	Spans res;

	if (fabs(lh.Start - rhFit.Start) > FLOAT_ZERO)
	{
		res.push_back(Span(lh.Start, rhFit.Start));
		res.push_back(Span(rhFit.End, lh.End));
		return res;
	}
	
	res.push_back(Span(rhFit.End, lh.End));

	return res;

	/*if (lh.Start - rhFit.Start > FLOAT_ZERO)
	{
		res.push_back(Span(lh.Start, rhFit.Start));
	}
	if (rhFit.End - lh.End > FLOAT_ZERO)
	{
		res.push_back(Span(rhFit.End, lh.End));
	}*/
	//return res;
}

Spans SpanDifference(const Spans& lh, const Spans& rh)
{
	Spans res;
	int   resSize = 0;
	for each (auto lSpan in lh)
	{
		for each (auto rSpan in rh)
		{
			Spans diff = SpanDifference(lSpan, rSpan);
			resSize = res.size();
			res.resize(resSize + diff.size());
			std::copy(diff.begin(), diff.end(), res.begin() + resSize);
		}
	}
	return res;
}