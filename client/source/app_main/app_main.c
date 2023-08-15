#include "app_main.h"
#include "app_config.h"
#include "app_client.h"
#include <stdio.h>
#include <stdlib.h>

void app_main_init(int argc, char *argv[])
{
    if(argc == 3)
    {
        app_client_init(argv[1], atoi(argv[2]));
    }
    else
    {
        printf("Not enough arguments.\n");
        exit(EXIT_FAILURE);
    }
}

void app_main_loop()
{
    app_client_handle();
}

void app_main_deinit()
{
    app_client_deinit();
}
