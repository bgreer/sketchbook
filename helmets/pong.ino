// this is for the thomas helmet
float ballpos[2], ballvel[2], paddlepos[2];
uint8_t score[2];

void pong_init()
{
	uint8_t ii;
	float dir;

	// clear the screen
	for (ii=0; ii<40; ii++)
		pix[ii] = 0x00;

	score[0] = 0;
	score[1] = 0;

	random_start();

	shiftpos = 47;
}

void random_start()
{
	// set ball and paddles
	ballpos[0] = random(10)+15;
	ballpos[1] = random(4)+2;
	ballvel[0] = (random(5)*0.2+1.0) * (random(2)*2-1);
	ballvel[1] = (random(5)*0.2+1.0) * (random(2)*2-1);

	paddlepos[0] = random(3)+3;
	paddlepos[1] = random(3)+3;

}

void pong_step()
{
	uint8_t row, col;

	ballpos[0] += ballvel[0]*0.1;
	ballpos[1] += ballvel[1]*0.1;

	// move paddles
	if (ballpos[0] < 10.0 && ballvel[0] < 0.0)
		paddlepos[0] += 0.1*(ballpos[1]-paddlepos[0]);
	if (ballpos[0] > 30.0 && ballvel[0] > 0.0)
		paddlepos[1] += 0.1*(ballpos[1]-paddlepos[1]);
	
	// constrain paddle positions
	paddlepos[0] = constrain(paddlepos[0], 1, 6);
	paddlepos[1] = constrain(paddlepos[1], 1, 6);

	// check bounds and bounce
	if (ballpos[1] < 0.0) {ballpos[1] = 0.0; ballvel[1] *= -1.0;}
	if (ballpos[1] > 7.0) {ballpos[1] = 7.0; ballvel[1] *= -1.0;}

	// paddle hit, speed up ball
	if (ballpos[0] < 4.0 && ballpos[0] > 3.0 && ballpos[1] >= paddlepos[0]-1 && ballpos[1] <= paddlepos[0]+1)
		ballvel[0] *= -1.1;
	if (ballpos[0] > 35.0 && ballpos[0] < 36.0 && ballpos[1] >= paddlepos[1]-1 && ballpos[1] <= paddlepos[1]+1)
		ballvel[0] *= -1.1;

	// check for goal
	if (ballpos[0] < 0.0)
	{
		score[1] ++;
		random_start();
	} else if (ballpos[0] > 39.0) {
		score[0] ++;
		random_start();
	}

	// clear the screen
	for (ii=0; ii<40; ii++)
		pix[ii] = 0x00;

	// draw ball
	pix[(uint8_t)ballpos[0]] |= (0x01)<<((uint8_t)ballpos[1]);

	// draw paddles
	pix[3] |= (0x07)<<((uint8_t)paddlepos[0]-1);
	pix[36] |= (0x07)<<((uint8_t)paddlepos[1]-1);

	// draw score
	pix[0] |= reverse_byte(score[0]);
	pix[39] |= reverse_byte(score[1]);

	// call refresh
	refresh = 1;

}
