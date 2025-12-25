#pragma once
//-----------------------------------------------------------------------------------------------
struct IntVec3
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	int x = 0;
	int y = 0;
	int z = 0;

public:
	// Construction/Destruction
	~IntVec3() {}												// destructor (do nothing)
	IntVec3() {}												// default constructor (do nothing)
	IntVec3(IntVec3 const& copyFrom);							// copy constructor (from another vec2)
	explicit IntVec3(int initialX, int initialY, int initialZ);		// explicit constructor (from x, y)




	void operator=(const IntVec3& copyFrom);				// vec2 = vec2	// float * vec2
	void SetFromText(char const* text = "0, 0, 0");

	bool		operator==(const IntVec3& compare) const;		// vec2 == vec2
	bool		operator!=(const IntVec3& compare) const;		// vec2 != vec2
	IntVec3     operator+(const IntVec3& add)const;

	bool operator<(const IntVec3& cmp) const noexcept;
};


