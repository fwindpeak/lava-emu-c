#include "key.h"

#include <emscripten/emscripten.h>
#include <string.h>

int keymapc = 0;
unsigned char KeyMap[LAVA_KEY_NUM][2];

static unsigned char key_queue[16];
static int queue_head = 0;
static int queue_tail = 0;
static int key_down = 0;

static unsigned char dequeue_key(void)
{
    if(queue_head == queue_tail)
        return 0;
    unsigned char key = key_queue[queue_head];
    queue_head = (queue_head + 1) % (int)(sizeof(key_queue));
    return key;
}

EMSCRIPTEN_KEEPALIVE void lava_enqueue_key(unsigned char key)
{
    int next_tail = (queue_tail + 1) % (int)(sizeof(key_queue));
    if(next_tail == queue_head)
        dequeue_key();
    key_queue[queue_tail] = key;
    queue_tail = next_tail;
}

void key_init(void)
{
    keymapc = 0;
    memset(KeyMap, 0, sizeof(KeyMap));
    queue_head = queue_tail = 0;
    key_down = 0;
}

unsigned char key_getmap(unsigned char key)
{
    for(int i = 0; i < keymapc; ++i)
    {
        if(KeyMap[i][0] == key)
            return KeyMap[i][1];
    }
    return key;
}

unsigned char key_getmap2(unsigned char key)
{
    for(int i = 0; i < keymapc; ++i)
    {
        if(KeyMap[i][1] == key)
            return KeyMap[i][0];
    }
    return key;
}

uint8_t key_check(uint8_t key)
{
    if(key < 128)
        return key_down == key ? key : 0;
    return (key_down != 0) ? key_down : 0;
}

uint8_t key_read(void)
{
    unsigned char key = dequeue_key();
    if(key)
    {
        key_down = key;
        return key;
    }
    key_down = 0;
    return 0;
}

uint8_t key_get(void)
{
    uint8_t key;
    while((key = key_read()) == 0)
    {
        emscripten_sleep(1);
    }
    return key;
}

void key_release(uint8_t key)
{
    (void)key;
    key_down = 0;
}
