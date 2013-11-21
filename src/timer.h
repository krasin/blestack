
typedef void (*timer_cb) (void *userdata);

uint16_t timer_start(uint32_t us, timer_cb cb, void *userdata);
