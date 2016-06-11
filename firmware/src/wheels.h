
/* w in [0,2] returns the position (deg) */
float wheel_position(int w);

/* w in [0,2] ; theta in deg  */
void set_target(int w, float theta);

void wheel_init();
void wheel_tick();
