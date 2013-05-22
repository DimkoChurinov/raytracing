#ifndef GEOMETRY_SPAN_H
#define GEOMETRY_SPAN_H
	
#include <vector>

struct Span
{	
	explicit Span(float start = -1.f, float end = -1.f)
		: Start(start),
			End(end)
	{

	}

	float Start, End;
};

typedef std::vector<Span> Spans;

inline bool SpanIsToTheLeft(const Span& lh, const Span& rh)
{
	return lh.End < rh.Start;
}

inline bool SpanIsToTheRight(const Span& lh, const Span& rh)
{
	return lh.Start > rh.End;
}

inline bool operator<(const Span& lh, const Span& rh)
{
	return lh.Start <= rh.Start;
}

inline bool operator>(const Span& lh, const Span& rh)
{
	return lh.Start > rh.Start;
}

inline bool SpanContains(const Span& span, float t)
{
	return span.Start <= t && t <= span.End;
}

Spans SpanUnite(const Span& lh, const Span& rh);
Spans SpanUnite(const Spans& lh, const Spans& rh);
Spans SpanIntersect(const Span& lh, const Span& rh);
Spans SpanIntersect(const Spans& lh, const Spans& rh);
Spans SpanDifference(const Span& lh, const Span& rh);
Spans SpanDifference(const Spans& lh, const Spans& rh);

#endif