#include "hip.h"
#include "spline.h"
#include <iostream> // debug
#include <cstdio>
#include <cstdlib>
#include <vector>
//using namespace std; //debug

void StepHipTraj::reset() {
	t_ = 0;
    numPiece = 0;
    n = 4;
}

int StepHipTraj::Increment(float* traj_value ) {
	//*traj_value = max_hip_flexion_ * sin(t_ * 0.001); // Sample calculation

    if (numPiece < n-1 && t_ > X[numPiece+1]) numPiece++;

    *traj_value = s(t_) + walking_angle_;

	if (t_ >= 2*step_time_) {
        t_ -= 2*step_time_;
		return 1;
	}else{
		t_ ++; // Incrementing current time by one time unit
		return 0;
	}
}

#define PI 3.14159265

void StepHipTraj::init() // the generation of spline
{
    x[0] = 0;
    x[1] = max_hip_flexion_time_;
    x[2] = step_time_;
    x[3] = 2*step_time_;
    y[0] = swing_start_;
    y[1] = max_hip_flexion_;
    y[2] = 2 * asin(step_range_ / 2.0 / leglen) * 180 / PI;
    y[3] = swing_start_;

    splineInterpolate();
}

void StepHipTraj::splineInterpolate()
{
    X = std::vector<double>(8);
    Y = std::vector<double>(8);
    X[0] = double(x[0]); Y[0] = double(y[0]);
    X[5] = double(x[2]); Y[5] = double(y[2]);
    X[7] = double(x[3]); Y[7] = double(y[3]);
    X[1] = 0.667*x[0] + 0.333*x[1]; Y[1] = 0.667*y[0] + 0.333*y[1];
    X[4] = 0.333*x[1] + 0.667*x[2]; Y[4] = 0.15*y[1] + 0.85*y[2];
    X[6] = 0.667*x[2] + 0.333*x[3]; Y[6] = 0.667*y[2] + 0.333*y[3];
    // Next, add points next to x[1] to make its first derivative 0
    X[2] = double(x[1] - 0.01*step_time_); Y[2] = y[1];
    X[3] = double(x[1] + 0.01*step_time_); Y[3] = y[1];

    s.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0, false);
    s.set_points(X, Y);
}

void StepHipTraj::set_max_hip_flexion(float value) {
    max_hip_flexion_ = value;
}

void StepHipTraj::set_max_hip_flexion_time(float value) {
    max_hip_flexion_time_ = value; // in ms
}

void StepHipTraj::set_walking_angle(float value) {
	walking_angle_ = value;
}

void StepHipTraj::set_swing_start(float value) {
	swing_start_ = value;
}

void StepHipTraj::set_step_time(float value) {
	step_time_ = value; // in ms
}

void StepHipTraj::set_step_range(float value) {
    step_range_ = value; // in meters, need to be converted
}

void StepHipTraj::set_leg_length(float value) {
    leglen = value;
}