#include <stdint.h>
#include <math.h>

#define L1 9.37
#define L2 11.03
#define L3 4.0

struct arm_state
{
	float angle[4];
	float pos[3];
	uint8_t status;
};

struct stack_entry
{
	struct arm_state state;
	struct stack_entry *next;
};

struct stack
{
	struct stack_entry *top;
};


// function prototypes

// forward.c
void getState (struct arm_state *state);
