#ifndef LOCATION_H
#define LOCATION_H

struct location {
    int x;
    int y;
};

enum direction {
    NORTH=0,
    EAST,
    SOUTH,
    WEST
};

struct vector {
    struct location loc;
    int dir;
};

/* movement using direction */
//int turn_right(int dir);
/* movement using location */
/* movement using direction and location */
/** @brief Move n steps in the indicated direction
 *  @param new_loc [OUT] returned calculated location (must be valid)
 *  @param loc [IN] starting location (must be valid)
 *  @param dir [IN] direction to move
 *  @param steps [IN] number of steps to move
 **/
void go_direction(
        struct location * new_loc, 
        const struct location * loc,
        int dir,
        int steps);

/** @brief Move n steps in the indicated direction with bounds check
 *  @param new_loc [OUT] if non-NULL, returned calculated location
 *          even if movement is invalid
 *  @param loc [IN] starting location (must be valid)
 *  @param dir [IN] direction to move
 *  @param steps [IN] number of steps to move
 *  @param bound_x [IN] max x bound; must have 0 <= x < bound_x
 *  @param bound_y [IN] max y bound; must have 0 <= y < bound_y
 *  @return 1 if move was made, 0 if it hit a boundary
 **/
int go_direction_with_bounds(
        struct location * new_loc, 
        const struct location * loc,
        int dir,
        int steps,
        int bound_x,
        int bound_y);

#endif /* #ifndef LOCATION_H */

