#pragma once

enum class PlayerState
{
	INVALID,
	IDLE,
	WALKING,
	MELEE_ATTACKING,
	RANGED_ATTACKING,
	DASHING,
	FALLING,
	STUNNED,
	DIE,
};

enum class BugState
{
	INVALID,
	DEAD,
	IDLE,
	MOVING,
	MOVING_BACK,
	ATTACKING,
	PARASITE
};