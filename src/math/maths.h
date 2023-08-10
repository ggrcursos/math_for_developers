#ifndef MATHS_H
#define MATHS_H
float Approach(float flGoal, float flCurrent, float dt)
{
	float flDifference = flGoal - flCurrent;

	if (flDifference > dt)
		return flCurrent + dt;
	if (flDifference < -dt)
		return flCurrent - dt;

	return flGoal;
}

#endif