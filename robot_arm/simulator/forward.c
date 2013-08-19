#include "header.h"

// read the analog sensors and convert the readings to angles
void getState (struct arm_state *state)
{
	state->angle[0] = 0.0;
	state->angle[1] = 0.0;
	state->angle[2] = 0.0;
	state->angle[3] = 0.0;
}

// take an angle state and convert it into a position state
void FK (struct arm_state *state)
{
	// solve for the 3d position
	float r;

	r = L1*cos(state->angle[1]) + L2*cos(state->angle[1]+state->angle[2]) + L3*cos(state->angle[1]+state->angle[2]+state->angle[3]);
	state->pos[0] = r * cos(state->angle[0]);
	state->pos[1] = r * sin(state->angle[0]);
	state->pos[2] = L1*sin(state->angle[1]) + L2*sin(state->angle[1]+state->angle[2]) + L3*sin(state->angle[1]+state->angle[2]+state->angle[3]);
}

// check bounds on angle and/or position
// which = 0 : none
//         1 : angle
//         2 : position
//         3 : both
// returns 0,1,2,3 depending on what failed, same as 'which'
uint8_t checkBounds (struct arm_state *state, uint8_t which)
{
	uint8_t ret = 0;
	// check angle
	if (which & 0x01)
	{
		// begin angle boundaries
		
	}

	// check position
	if (which & 0x02)
	{
		// begin boundaries in 3d space
		if (state->pos[3] < 0.0) ret &= 0x02;
	}

	return ret;
}
