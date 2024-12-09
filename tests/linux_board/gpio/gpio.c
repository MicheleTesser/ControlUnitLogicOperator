#include "./raceup_board/raceup_board.h"
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/cdefs.h>

enum gpio_mode{
    Push_pull_output,
    //TODO: add all general modes, or at least the one used in the CULO
};

typedef struct{
    int fd;
    enum gpio_mode mode;
    BoardComponentId id;
}virtual_gpio;

static virtual_gpio gpios[1024];

#define LOW 0u;
#define HIGH 1u;
#define FIND_GPIO_AND_APPLY(id,exp)\
    for (uint32_t i=0;i<sizeof(gpios)/sizeof(gpios[0]);i++) {\
        virtual_gpio* gpio = &gpios[id];\
        if (gpio->id == id) {\
            exp;\
        }\
    }

int8_t hardware_init_gpio(const BoardComponentId id)
{
    static uint8_t nex_free_gpio = 0;
    char path[1024] = {0};
    char* cursor = strcat("./virtual_gpios/", path);
    sprintf(cursor, "%u", id);
    int new_pin_fd = open(path, O_CREAT | O_RDWR, S_IWUSR );
    if (new_pin_fd < 0) {
        return -1;
    }
    gpios[nex_free_gpio].id = id;
    gpios[nex_free_gpio].fd = new_pin_fd;
    gpios[nex_free_gpio].mode = Push_pull_output;
    nex_free_gpio++;

    return 0;
}

int8_t gpio_set_pin_mode(const BoardComponentId id, uint8_t mode )
{
    FIND_GPIO_AND_APPLY(id, {
        gpio->mode = mode;
        return 0;
    });
    return 0;
}
int8_t gpio_toggle(const BoardComponentId id __attribute_maybe_unused__)
{
    int8_t status = gpio_read_state(id);
    if (status) {
        gpio_set_high(id);
        return 0;
    }else{
        gpio_set_low(id);
        return 0;
    }
    return 0;
}
int8_t gpio_read_state(const BoardComponentId id )
{
    FIND_GPIO_AND_APPLY(id, {
        uint8_t content=0;
        read(gpio->fd, &content, 1);
        return content;
    });
    return 0;
}
int8_t gpio_set_high(const BoardComponentId id __attribute_maybe_unused__)
{
    FIND_GPIO_AND_APPLY(id, {
        uint8_t data = LOW;
        write(gpio->fd, &data, 0);
        return 0;
    })
    return 0;
}

int8_t gpio_set_low(const BoardComponentId id __attribute_maybe_unused__)
{
    FIND_GPIO_AND_APPLY(id, {
        uint8_t data = HIGH;
        write(gpio->fd, &data, 0);
        return 0;
    })
    return 0;
}
