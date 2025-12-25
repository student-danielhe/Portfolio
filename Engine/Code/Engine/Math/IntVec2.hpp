#pragma once
//-----------------------------------------------------------------------------------------------
struct IntVec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	int x = 0;
	int y = 0;

public:
	// Construction/Destruction
	~IntVec2() {}												// destructor (do nothing)
	IntVec2() {}												// default constructor (do nothing)
	IntVec2(IntVec2 const& copyFrom);							// copy constructor (from another vec2)
	explicit IntVec2(int initialX, int initialY);		// explicit constructor (from x, y)

	//Accessors
	float GetLength()const;
	int GetTaxicabLength()const;
	int GetLengthSquared()const;
	float GetOrientationRadians()const;
	float GetOrientationDegrees()const;
	IntVec2 const GetRotated90Degrees()const;
	IntVec2 const GetRotatedMinus90Degrees()const;
	

	//Mutators
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	void operator=(const IntVec2& copyFrom);				// vec2 = vec2	// float * vec2
	void SetFromText(char const* text = "0,0");

	bool		operator==(const IntVec2& compare) const;		// vec2 == vec2
	bool		operator!=(const IntVec2& compare) const;		// vec2 != vec2
	IntVec2     operator+(const IntVec2& add)const;
	
	bool operator<(const IntVec2& cmp) const noexcept
	{
		if (y < cmp.y) {
			return true;
		}
		else if (y == cmp.y && x < cmp.x) {
			return true;
		}
		return false;
	};
};
