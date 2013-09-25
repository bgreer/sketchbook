#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define L1 9.37
#define L2 11.03
#define L3 4.0

float anglelowerbound[4];
float angleupperbound[4];

typedef struct stack stack;
typedef struct arm_state arm_state;
typedef struct stack_entry stack_entry;

struct arm_state
{
	float angle[4];
	float pos[3];
	uint8_t status;
};

struct stack_entry
{
	arm_state *state;
	stack_entry *next;
};

struct stack
{
	stack_entry *top;
};


// function prototypes

// forward.c
void getState (arm_state *state);
void printState (arm_state *state);

// stacker.c
stack_entry* addEntry(arm_state *st);
arm_state* createState_ang(float a0, float a1, float a2, float a3);
arm_state* createState_pos(float x, float y, float z);
arm_state* createState(float a0, float a1, float a2, float a3, float x, float y, float z);
void free_chain (stack *st);
