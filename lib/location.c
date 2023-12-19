#include "location.h"

static const struct location move[4] = {
    /* NORTH */ { 0, -1 },
    /* EAST  */ { 1, 0 },
    /* SOUTH */ { 0, 1 },
    /* WEST  */ { -1, 0 }
};

void go_direction(
        struct location * new_loc, 
        const struct location * loc,
        int dir,
        int steps)
{
    new_loc->x = loc->x + move[dir].x * steps;
    new_loc->y = loc->y + move[dir].y * steps;
}

int go_direction_with_bounds(
        struct location * new_loc, 
        const struct location * loc,
        int dir,
        int steps,
        int bound_x,
        int bound_y)
{
    struct location local_new_loc;
    go_direction(&local_new_loc, loc, dir, steps);
    if(new_loc) {
        new_loc->x = local_new_loc.x;
        new_loc->y = local_new_loc.y;
    }
    if((local_new_loc.x >= bound_x)
            || (local_new_loc.y >= bound_y)
            || (local_new_loc.x < 0)
            || (local_new_loc.y < 0))
        return 0;
    return 1;
}

