#ifndef ALPHA_SNAKE_CONFIG_H
#define ALPHA_SNAKE_CONFIG_H

#define DEFAULT_NB_SNAKES 5
#define DEFAULT_NB_BONUS 5
#define DEFAULT_TAILLE_SNAKE 5
#define DEFAULT_TAILLE_BOT 5
#define DEFAULT_HEIGHT 50
#define DEFAULT_WIDTH 50
#define DEFAULT_TYPE_PARTIE 0
#define DEFAULT_GRID_SIZE 25
#define DEFAULT_COLLISION 1
#define DEFAULT_ADVANCED_BONUS 0
#define DEFAULT_INTERVAL 100
#define DEFAULT_SCREEN_WIDTH 1260
#define DEFAULT_SCREEN_HEIGHT 800

typedef struct _config Config;

/**
 * @brief      Les configurations général et d'une partie
 */
struct _config
{
    int nb_snakes;
    int nb_bonus;
    int taille_snake;
    int taille_bot;
    int height;
    int width;
    int type_partie; // 0 pour partie par défaut 1 pour classique  2 pour invasion et 3 pour reseau
    int grid_size;
    int collision; // TODO
    int advanced_bonus; //TODO
    int interval;
    int screen_width;
    int screen_height;

};

Config * init_default_config();
void free_config(Config *c);

Config * init_config();


#endif //ALPHA_SNAKE_CONFIG_H