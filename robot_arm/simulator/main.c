
/*
	the purpose of this code is to simulate the IK solver stack that will be implemented on Arduino hardware
	the arm has a state (angles) and a stack of 3d positions to get to
	if there is something in the stack, the IK solver finds the appropriate state for it
	before moving the motors to get there, we must get a good guess for the path the hand will take
	if the hand ever exits the volumetric boundaries (like going into the floor), we need to make a new target
		take the out of bounds point and find the closest valid point
		add that to the stack and invalidate the IK solve for everything else
		go through the process again of doing the IK solve for the top of the stack
	
	in this way, the arm can (in theory) move itself to any valid point given any obstacles
*/

#include "header.h"

int main ()
{

}
