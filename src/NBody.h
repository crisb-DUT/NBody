#pragma once

// this file contains the simulation parameters.


// epsilon as damping factor
const double eps = 0.1;

// gravity factor
const double G = 1e-7;

// cursor weight
const double cursor_weight = 500;

// reflection factor
// lose speed if collide to the boundary
const double collision_damping = 0.6;