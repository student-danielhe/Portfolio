#pragma once
struct IntRange {
public:
	int m_min=0;
	int m_max=0;
	IntRange(){}
	explicit IntRange(int min, int max);
	bool		operator==(IntRange const& compare) const;		
	bool		operator!=(IntRange const& compare) const;		
	void		operator=(IntRange const& copyFrom);
	bool IsOnRange(int compare) const;
	bool IsOverlappingWith(IntRange const& compare) const;
	static const IntRange ZERO;
	static const IntRange ONE;
	static const IntRange ZERO_TO_ONE;
};