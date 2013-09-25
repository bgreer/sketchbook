#include "header.h"

// read the analog sensors and convert the readings to angles
void getState (arm_state *state)
{
	state->angle[0] = 0.0;
	state->angle[1] = 0.0;
	state->angle[2] = 0.0;
	state->angle[3] = 0.0;
}

/* calculate the distance between two states */
float dist (arm_state *st1, arm_state *st2)
{
	float ret = 0.0;
	ret += (st1->pos[0] - st2->pos[0]) * (st1->pos[0] - st2->pos[0]);
	ret += (st1->pos[1] - st2->pos[1]) * (st1->pos[1] - st2->pos[1]);
	ret += (st1->pos[2] - st2->pos[2]) * (st1->pos[2] - st2->pos[2]);
	return sqrt(ret);
}

// take an angle state and convert it into a position state
void FK (arm_state *state)
{
	// solve for the 3d position
	float r;

	r = L1*cos(state->angle[1]) + L2*cos(state->angle[1]+state->angle[2]) + L3*cos(state->angle[1]+state->angle[2]+state->angle[3]);
	state->pos[0] = r * cos(state->angle[0]);
	state->pos[1] = r * sin(state->angle[0]);
	state->pos[2] = L1*sin(state->angle[1]) + L2*sin(state->angle[1]+state->angle[2]) + L3*sin(state->angle[1]+state->angle[2]+state->angle[3]);
}

// check bounds on angle and/or position
// which = 0 : none     00
//         1 : angle    01
//         2 : position 10
//         3 : both     11
// returns 0,1,2,3 depending on what failed, same as 'which'
// also flags bits 00XXXX00 as 1 if that angle is out of bounds
uint8_t checkBounds (arm_state *state, uint8_t which)
{
	uint8_t ret = 0;
	uint8_t i;
	// check angle
	if (which & 0x01)
	{
		// begin angle boundaries
		for (i=0; i<4; i++)
		{
			if (state->angle[i] >= angleupperbound[i]) ret |= (0x01 | (0x01<<(i+2)));
			if (state->angle[i] <= anglelowerbound[i]) ret |= (0x01 | (0x01<<(i+2)));
		}
	}

	// check position
	if (which & 0x02)
	{
		// begin boundaries in 3d space
		if (state->pos[3] < 0.0) ret &= 0x02;
	}
	state->status = ret;
	return ret;
}

void printState (arm_state *state)
{
	printf("State: pos(%f, %f, %f), ang(%f, %f, %f, %f), stat=%d\n", state->pos[0], state->pos[1], state->pos[2], state->angle[0], state->angle[1], state->angle[2], state->angle[3], state->status);
}
