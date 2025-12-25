#include "GameCommon.hpp"


int  GetConnectedControllerID() {
	for (int i = 0; i < 4; i++) {
		if (g_theInput->GetController(i).IsConnected()) {
			return i;
		}
	}
	return 0;
}
