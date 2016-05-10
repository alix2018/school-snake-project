#include "config.h"
#include <stdlib.h>

#include <gdk/gdk.h>

Config * init_default_config()
{
    Config * res;
    res = malloc(sizeof(Config));
    res->nb_snakes = DEFAULT_NB_SNAKES;
    res->nb_bonus = DEFAULT_NB_BONUS;
    res->taille_snake = DEFAULT_TAILLE_SNAKE;
    res->taille_bot = DEFAULT_TAILLE_BOT;
    res->height = DEFAULT_HEIGHT;
    res->width = DEFAULT_WIDTH;
    res->type_partie = DEFAULT_TYPE_PARTIE;
    res->grid_size = DEFAULT_GRID_SIZE;
    res->collision = DEFAULT_COLLISION;
    res->advanced_bonus = DEFAULT_ADVANCED_BONUS;
    res->interval = DEFAULT_INTERVAL;
    res->screen_height = DEFAULT_SCREEN_HEIGHT;
    res->screen_width = DEFAULT_SCREEN_WIDTH;
    return res;

}


void free_config(Config * c)
{
    free(c);
}

void check_screen_size(int cur_screen_height,int cur_screen_width,int cur_grid_size,int cur_height, int cur_width)
{
    /**GdkScreen * gs =  gdk_screen_get_default();
    if(gs != NULL)
    {

        printf("%i\n",gdk_screen_is_composited(gs));
    }
     **/
    if(cur_screen_height < cur_grid_size*cur_height)
    {
        perror("[ATTENTION] La taille du plateau risque d'être trop grand par rapport à la taille de l'écran (hauteur) \n");

    }
    if(cur_screen_width < cur_grid_size*cur_width)
    {
        perror("[ATTENTION] La taille du plateau risque d'être trop grand par rapport à la taille de l'écran  (largueur) \n");

    }

}

void config_grid_from_screen_and_window(Config * config)
{
    int gs1 = config->screen_height/config->height;
    int gs2 = config->screen_width/config->width;
    int gs;
    if(gs1 != gs2) // résolution pas conforme ? modification personnel ou ajuste avec le min
    {

        if(gs1 > gs2)
        {
            gs = gs2;
        }
        else
        {
            gs = gs1;
        }
    }
    else
    {
        gs = gs1;
    }

    printf("gs : %i\n",gs);
    config->grid_size = gs;

}
Config * init_config()
{
    Config * res = init_default_config();
    //res->interval = 50;
    config_grid_from_screen_and_window(res);
    check_screen_size(res->screen_height,res->screen_width,res->grid_size,res->height,res->width);

    //TODO changer les params en fct de ce qu'on veut
    return res;
}