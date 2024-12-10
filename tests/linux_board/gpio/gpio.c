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
    char gpio_path[1024];
    enum gpio_mode mode;
    BoardComponentId id;
}virtual_gpio;

static virtual_gpio gpios[1024];

#define LOW '1'
#define HIGH '0'
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
    char gpios_name[] = "gpio_";
    strcat(path, gpios_name);
    sprintf(path + sizeof(gpios_name) -1, "%d", id);
    *(path + sizeof(gpios_name)) = '\0';
    FILE* new_pin_fd = fopen(path, "w");
    if (!new_pin_fd) {
        return -1;
    }
    char c = LOW;
    fwrite(&c, sizeof(c), sizeof(c), new_pin_fd);
    gpios[nex_free_gpio].id = id;
    memcpy(&gpios[nex_free_gpio].gpio_path, path, sizeof(path));
    gpios[nex_free_gpio].mode = Push_pull_output;
    nex_free_gpio++;
    fclose(new_pin_fd);

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
    if (status == LOW) {
        gpio_set_high(id);
        return 0;
    }else if (status == HIGH){
        gpio_set_low(id);
        return 0;
    }
    return 0;
}
int8_t gpio_read_state(const BoardComponentId id )
{
    for (uint32_t i=0;i<sizeof(gpios)/sizeof(gpios[0]);i++) {
        virtual_gpio* gpio = &gpios[id];
        if (gpio->id == id) {
            uint8_t content=0;
            FILE* f = fopen(gpio->gpio_path, "r");
            if(!fread(&content, sizeof(content), sizeof(content),f)){
                return -1;
            }
            fclose(f);
            return content;
        }
    }
    return -1;
}
int8_t gpio_set_high(const BoardComponentId id __attribute_maybe_unused__)
{
    FIND_GPIO_AND_APPLY(id, {
        char b = HIGH;
        FILE* f = fopen(gpio->gpio_path, "w");
        if(!fwrite(&b,sizeof(b),sizeof(b),f)){
            fprintf(stderr, "error writing set high\n");
            return -1;
        }
        fclose(f);
        return 0;
    })
    return -1;
}

int8_t gpio_set_low(const BoardComponentId id __attribute_maybe_unused__)
{
    FIND_GPIO_AND_APPLY(id, {
        char b = LOW;
        FILE* f = fopen(gpio->gpio_path, "w");
        if(!fwrite(&b,sizeof(b),sizeof(b),f)){
            fprintf(stderr, "error writing set high\n");
            return -1;
        }
        fclose(f);
        return 0;
    })
    return -1;
}
