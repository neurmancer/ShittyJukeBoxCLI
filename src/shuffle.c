#include "shuffle.h"
#include <errno.h>
#include <sys/random.h>

static int random_below(size_t bound, size_t *result)
{
    size_t threshold = (size_t)(0 - bound) % bound;
    size_t value;
    
    do {
    
        size_t filled = 0;
    
        while (filled < sizeof value) {
            ssize_t received = getrandom((unsigned char *)&value + filled, sizeof value - filled, 0);
    
            if (received < 0 && errno == EINTR) { continue; }
            if (received < 0) { return(-1); }
            if (!received) { errno = EIO; return(-1); }
    
            filled += (size_t)received;
        }
    } while (value < threshold);
    
    *result = value % bound;
    
    return(0);
}

int shuffle_order(size_t *order, size_t count, size_t current)
{
    if (!count) { return(0); }
    if (!order || current >= count) { errno = EINVAL; return(-1); }
    
    for (size_t i = 0; i < count; ++i) { order[i] = i; }
    
    order[0] = current;
    order[current] = 0;
    
    for (size_t remaining = count - 1; remaining > 1; --remaining) {
        size_t index;
        if (random_below(remaining, &index) < 0) { return(-1); }
        ++index;
        size_t swap = order[remaining];
        order[remaining] = order[index];
        order[index] = swap;
    }
    return(0);
}
