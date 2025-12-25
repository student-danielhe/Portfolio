#pragma once
#include "GameCommon.hpp"
#include "BlockDefinition.hpp"
class Block{
public:
    uint8_t m_type = 0;
    uint8_t m_lightData = 0;
    uint8_t m_lightFlag = 0;

    int GetIndoor();
    void    SetIndoor(int data);

	int GetOutdoor();
	void    SetOutdoor(int data);

    bool GetIsSky();
    void SetIsSky(bool input);

	bool GetIsLightDirty();
	void SetIsLightDirty(bool input);

	bool GetIsFullOpaque();
	void SetIsFullOpaque(bool input);

	bool GetIsSolid();
	void SetIsSolid(bool input);

	bool GetIsVisible();
	void SetIsVisible(bool input);

	bool SetNewTemperature(float input);
};