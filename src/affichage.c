#include <clutter/clutter.h>
#include "affichage.h"
#include "list.h"
#include "struc.h"
#include <gdk/gdk.h>
#include <time.h>

#define GRID_SIZE 23
#define BACKGROUND_IMAGE_SRC "data/fond.jpg"
#define TETE_IMAGE_SRC "data/tetev1.png"
#define QUEUE_IMAGE_SRC "data/queuev1.png"
#define CORPS_IMAGE_SRC "data/corpsv1.png"
#define TURNLIGHT_IMAGE_SRC "data/corpsturnlightside.png"
#define TURNDARK_IMAGE_SRC "data/corpsturndarkside.png"






Bouf *bouf_new(int x, int y)
{
    srand(time(NULL));
    Bouf *new = malloc(sizeof(Bouf *));
    new->coord = coord_from_xy(rand()%x, rand()%y);
    return new;
}

void bouf_update(Bouf *bouf, int x, int y)
{
    bouf->coord = coord_from_xy(rand()%x, rand()%y);
}

void bouf_set_coord(Bouf *bouf, Coord *c)
{
    bouf->coord = coord_from_xy(c->x, c->y);
}

BoufActor *create_bouf_actor(ClutterActor *parent, Bouf *b, ClutterColor *color)
{
    BoufActor *res;
    ClutterActor *bouf_c_actor;

    bouf_c_actor = clutter_actor_new();
    clutter_actor_set_size(bouf_c_actor, GRID_SIZE, GRID_SIZE);
    clutter_actor_set_background_color(bouf_c_actor, color);
    clutter_actor_add_child(parent, bouf_c_actor);
    clutter_actor_set_position(bouf_c_actor, b->coord.x * GRID_SIZE, b->coord.y * GRID_SIZE);

    res = malloc(sizeof(BoufActor));
    res->parent = parent;
    res->bouf_c_actor = bouf_c_actor;
    res->color = color;
    res->bouf = b;

    return res;
}

void bouf_actor_update(BoufActor *ba)
{
    clutter_actor_set_position(ba->bouf_c_actor, ba->bouf->coord.x * GRID_SIZE, ba->bouf->coord.y * GRID_SIZE);
}

UpletActor uplet_actor_new(SnakeActor *a1, SnakeActor *a2, BoufActor *bouf)
{
    UpletActor new;
    new.sa = a1;
    new.sa_ia = a2;
    new.bouf = bouf;
    return(new);
}

gboolean zone_snake_key_press_cb(ClutterActor *actor, ClutterEvent *event, gpointer data)
{
    Snake *s = data;
    guint key_symbol = clutter_event_get_key_symbol(event);

    switch (key_symbol)
    {
        case CLUTTER_KEY_Up:
            if (snake_direction(s) != BAS)
                snake_set_direction(s, HAUT);
            break;
        case CLUTTER_KEY_Down:
            if (snake_direction(s) != HAUT)
                snake_set_direction(s, BAS);
            break;
        case CLUTTER_KEY_Left:
            if (snake_direction(s) != DROITE)
                snake_set_direction(s, GAUCHE);
            break;
        case CLUTTER_KEY_Right:
            if (snake_direction(s) != GAUCHE)
                snake_set_direction(s, DROITE);
        default:
            break;
    }

    return TRUE;
}


/**
 * @brief      Renvoie true si le snake est contre un mur et veux avancer dans le mur. Appelé dans la fonction timeout_tich_cb
 *
 * @param[in]  c1  coord à vérifier
 * @param[in]  c2  coord à vérifier
 *
 * @return      Renvoie 0 si le snake est contre un mur et veux avancer dans le mur, 1 sinon
 */
int snake_border_map(SnakeActor *sa)
{
    int res = 0, l_w, l_h;
    float w, h;

    Snake *s = sa->snake;

    clutter_actor_get_size(sa->parent, &w, &h);
    l_w = (int) w/GRID_SIZE;
    l_h = (int) h/GRID_SIZE;

    if(snake_direction(s) == HAUT && snake_pos(s).y == 0)
    {
        res = 1;
    }
    if(snake_direction(s) == GAUCHE && snake_pos(s).x == 0)
    {
        res = 1;
    }
    if(snake_direction(s) == BAS && snake_pos(s).y == l_h-1)
    {
        res = 1;
    }
    if(snake_direction(s) == DROITE && snake_pos(s).x == l_w-1)
    {
        res = 1;
    }

    return res;
}

/**
    Renvoie 0 si le snake essaie de se mordre la queu, 1 sinon
    Appelé dans la fonction timeout_tich_cb
*/
int snake_border_snake(SnakeActor *sa)
{
    int res = 0;
    Snake *s = sa->snake;
    ListeSnake ls = snake_premier(s);
    Coord c_tete = snake_pos(s);

    if(snake_direction(s) == HAUT)
    {
        c_tete.y -= 1;
        while( ls != NULL )
        {
            if(coord_egales(c_tete,liste_snake_coord(ls)))
            {
                res = 1;
            }
            ls = liste_snake_suivant(ls);
        }
    }

    if(snake_direction(s) == BAS)
    {
        c_tete.y += 1;
        while( ls != NULL )
        {
            if(coord_egales(c_tete,liste_snake_coord(ls)))
            {
                res = 1;
            }
            ls = liste_snake_suivant(ls);
        }
    }

    if(snake_direction(s) == GAUCHE)
    {
        c_tete.x -= 1;
        while( ls != NULL )
        {
            if(coord_egales(c_tete,liste_snake_coord(ls)))
            {
                res = 1;
            }
            ls = liste_snake_suivant(ls);
        }
    }

    if(snake_direction(s) == DROITE)
    {
        c_tete.x += 1;
        while( ls != NULL )
        {
            if(coord_egales(c_tete,liste_snake_coord(ls)))
            {
                res = 1;
            }
            ls = liste_snake_suivant(ls);
        }
    }

    
    return res;
}

/*
    renvoie 1 ssi s1 essay d'avancer alors qu'il y la queue de s2 devant
*/
int snake_border_other(SnakeActor *sa1, SnakeActor *sa2)
{
    int res = 0;
    Snake *s1 = sa1->snake;
    Snake *s2 = sa2->snake;

    ListeSnake ls_s2 = snake_premier(s2);
    Coord c_tete = snake_pos(s1);

    if(snake_direction(s1) == HAUT)
    {
        c_tete.y -= 1;
        while( ls_s2 != NULL )
        {
            if(coord_egales(c_tete,liste_snake_coord(ls_s2)))
            {
                res = 1;
            }
            ls_s2 = liste_snake_suivant(ls_s2);
        }
    }

    if(snake_direction(s1) == BAS)
    {
        c_tete.y += 1;
        while( ls_s2 != NULL )
        {
            if(coord_egales(c_tete,liste_snake_coord(ls_s2)))
            {
                res = 1;
            }
            ls_s2 = liste_snake_suivant(ls_s2);
        }
    }

    if(snake_direction(s1) == GAUCHE)
    {
        c_tete.x -= 1;
        while( ls_s2 != NULL )
        {
            if(coord_egales(c_tete,liste_snake_coord(ls_s2)))
            {
                res = 1;
            }
            ls_s2 = liste_snake_suivant(ls_s2);
        }
    }

    if(snake_direction(s1) == DROITE)
    {
        c_tete.x += 1;
        while( ls_s2 != NULL )
        {
            if(coord_egales(c_tete,liste_snake_coord(ls_s2)))
            {
                res = 1;
            }
            ls_s2 = liste_snake_suivant(ls_s2);
        }
    }

    return res;
}

/**
    Renvoie 1 si et seulement si les coords de la te et de la queue sont égales
*/
int snake_eat(Snake *s, Bouf *b)
{
    if(coord_egales(snake_pos(s), b->coord))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief Fonction callback appelée à intervalles réguliers.
 * 
 * @param[in]    data   Le SnakeActor du snake.
 */
gboolean timeout_tick_cb(gpointer data)
{
    UpletActor *ua = data;
    SnakeActor *sa = ua->sa;
    SnakeActor *sa_ia = ua->sa_ia;
    BoufActor  *bouf = ua->bouf;

    float w, h;

    clutter_actor_get_size(sa->parent, &w, &h);
    int l_w = (int) w/GRID_SIZE;
    int l_h = (int) h/GRID_SIZE;


    if(!snake_border_map(sa) && !snake_border_snake(sa))
    {
        snake_forward(sa->snake);
        snake_actor_update(sa);
        snake_forward_ia1(sa_ia->snake, sa->snake, bouf->bouf->coord);
        snake_actor_update(sa_ia);
        if(snake_eat(sa->snake, bouf->bouf))
        {
            snake_increase(sa->snake);
            bouf_update(bouf->bouf, l_w, l_h);
            bouf_actor_update(bouf);
            printf("Snake eat!\n");
        }
        if(snake_eat(sa_ia->snake, bouf->bouf))
        {
            snake_increase(sa_ia->snake);
            bouf_update(bouf->bouf, l_w, l_h);
            bouf_actor_update(bouf);
            printf("Snake ia eat!\n");

        }
    }

    return G_SOURCE_CONTINUE;
}

/**
 * @brief   Fonction callback appelée lorsque la fenêtre est fermée.
 * 
 * @param[in]    data   Le SnakeActor du snake.
 * 
 * Ce prototype est défini par Clutter, cf. la documentation de Clutter.
 */
void stage_destroy_cb(ClutterActor *actor, gpointer data)
{
    SnakeActor *sa = data;

    free_snake_actor(sa);
    clutter_main_quit();
}


/**
 * @brief   Initialise un SnakeActor.
 * 
 * @param[in]    parent Le ClutterActor qui contiendra le snake.
 * @param[in]    s      Le snake qui sera affiché par le SnakeActor.
 *
 * @return  Le SnakeActor initialisé.
 */
SnakeActor *create_snake_actor(ClutterActor *parent, Snake *s,ClutterColor *color)
{
    SnakeActor *res;

    res = malloc(sizeof(struct _snake_actor));
    res->actors = create_list();
    res->parent = parent;
    res->cur_size = 0;
    res->color = color;
    res->snake = s;
    res->images = imgs;

    return res;
}


/**
 * Fonction appliquée à chaque élément de la liste des acteurs de SnakeActor
 * lors de la suppression de cette liste.
 */
static void free_clutter_actor_fn(void * elt)
{
    ClutterActor *actor = elt;

    clutter_actor_destroy(actor);
}


/**
 * Libère la mémoire consommée par un SnakeActor.
 *
 * @param[in]    sa     Le SnakeActor à libérer.
 */
void free_snake_actor(SnakeActor *sa)
{
    free_list_fn(sa->actors, free_clutter_actor_fn);
    free(sa);
}


/**
 * Met à jour la longueur et la position d'un SnakeActor.
 *
 * @param[in]    sa     Le SnakeActor à mettre à jour.
 */
void snake_actor_update(SnakeActor *sa)
{
    int delta;
    ClutterActor *actor;
    Node node_sa;
    ListeSnake node_s;
    Coord c;

    delta = snake_longueur(sa->snake) - sa->cur_size;

    if (delta > 0)
    {
        for (; delta > 0; delta--)
        {
            actor = clutter_actor_new();
            g_object_ref(actor);
            clutter_actor_set_size(actor, GRID_SIZE, GRID_SIZE);
           // clutter_actor_set_background_color(actor, sa->color);

            clutter_actor_set_easing_duration(actor, 0);
            //clutter_actor_set_easing_mode(actor,CLUTTER_EASE_IN_QUART);
            clutter_actor_add_child(sa->parent, actor);


            list_add_last(sa->actors, actor);
        }

        sa->cur_size = snake_longueur(sa->snake);
    }
    else if (delta < 0)
    {
        for (; delta < 0; delta++)
        {
            actor = list_pop_last(sa->actors);
            clutter_actor_destroy(actor);
        }

        sa->cur_size = snake_longueur(sa->snake);
    }

    node_s = snake_premier(sa->snake);
    for (node_sa = list_first_node(sa->actors);
         node_sa != NULL;
         node_sa = node_next(node_sa), node_s = liste_snake_suivant(node_s))
    {
        c = liste_snake_coord(node_s);
        actor = node_elt(node_sa);
        clutter_actor_set_position(
            actor,
            c.x * GRID_SIZE,
            c.y * GRID_SIZE
        );

        // TODO changer les images pendant le deplacement
        /** VERSION OPTIMALE On modifie que si il le faut
        if(node_sa == list_first_node(sa->actors) && clutter_actor_get_content(actor) == NULL)
        {
            clutter_actor_set_content(actor,sa->images->tete);
        }
        else if(node_sa == list_last_node(sa->actors) && clutter_actor_get_content(actor) == NULL)
        {
            if(clutter_actor_get_content(node_elt(node_prev(node_sa))) == sa->images->queue)
            {
                clutter_actor_set_content(node_elt(node_prev(node_sa)),sa->images->corps);
            }

            clutter_actor_set_content(actor,sa->images->queue);




        }else if(clutter_actor_get_content(actor) == NULL)
        {
            clutter_actor_set_content(actor,sa->images->corps);
        } */
        Coord cnext;
        Coord cprev;
        if(node_sa == list_first_node(sa->actors) )
        {
            Coord cnext = liste_snake_coord(liste_snake_suivant(node_s));
            clutter_actor_set_content(actor,sa->images->tete);
            clutter_actor_set_pivot_point (actor,0.5, 0.5 );
            clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,0);

            if(cnext.x == c.x && c.y < cnext.y)
            {
                // TODO tête droite (pas de rotation)

            }
            else if (cnext.x == c.x && c.y > cnext.y)
            {

                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,180);
            }
            else if (cnext.y == c.y && c.x > cnext.x )
            {

                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,90);
            }
            else if(cnext.y == c.y && c.x < cnext.x)
            {


                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,-90);

            }

        }
        else if(node_sa == list_last_node(sa->actors))
        {

            Coord cprev = liste_snake_coord(liste_snake_precedent(node_s));
            clutter_actor_set_content(actor,sa->images->queue);
            clutter_actor_set_pivot_point (actor,0.5, 0.5 );
            clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,0);
            if(cprev.x == c.x && c.y < cprev.y)
            {

                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,180);
            }
            else if (cprev.x == c.x && c.y > cprev.y)
            {


               // clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,-180);
            }
            else if (cprev.y == c.y && c.x > cprev.x )
            {

                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,-90);
            }
            else if(cprev.y == c.y && c.x < cprev.x)
            {

                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,90);
            }

        }else
        {

            Coord cnext = liste_snake_coord(liste_snake_suivant(node_s)); // vers la queue
            Coord cprev = liste_snake_coord(liste_snake_precedent(node_s)); // vers la tête
            clutter_actor_set_content(actor,sa->images->corps);

            clutter_actor_set_pivot_point (actor,0.5, 0.5 );
            clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,0);
            if(c.x == cnext.x && c.x == cprev.x && cnext.y > cprev.y)
            {
                // nothing

            }
            else if (c.x == cnext.x && c.x == cprev.x && cnext.y < cprev.y)
            {


                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,180);
            }
            else if (c.y == cnext.y && c.y == cprev.y && cprev.x < cnext.x)
            {
                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,-90);
            }
            else if(c.y == cnext.y && c.y == cprev.y && cprev.x > cnext.x)
            {


                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,90);
            }
            else if(c.x == cnext.x && c.y < cnext.y && c.y == cprev.y && c.x < cprev.x) // Les turnlight et turndark
            {

                clutter_actor_set_content(actor,sa->images->turnlight);

            }
            else if(c.x == cnext.x && c.y < cnext.y && c.y == cprev.y && c.x > cprev.x)
            {

                clutter_actor_set_content(actor, sa->images->turndark);

            }
            else if(c.x == cnext.x && c.y > cnext.y && c.y == cprev.y && c.x > cprev.x)
            {

                clutter_actor_set_content(actor,sa->images->turnlight);
                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,180);
            }
            else if(c.x == cnext.x && c.y > cnext.y && c.y == cprev.y && c.x < cprev.x)
            {

                clutter_actor_set_content(actor,sa->images->turndark);
                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,180);
            }
            else if(c.x == cprev.x && c.y > cprev.y && c.y == cnext.y && c.x < cnext.x)
            {

                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,-90);
                clutter_actor_set_content(actor,sa->images->turnlight);

            }
            else if(c.x == cprev.x && c.y > cprev.y && c.y == cnext.y && c.x > cnext.x)
            {

                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,90);
                clutter_actor_set_content(actor,sa->images->turndark);

            }
            else if(c.x == cprev.x && c.y < cprev.y && c.y == cnext.y && c.x > cnext.x)
            {

                clutter_actor_set_content(actor,sa->images->turnlight);
                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,90);

            }
            else if(c.x == cprev.x && c.y < cprev.y && c.y == cnext.y && c.x < cnext.x)
            {

                clutter_actor_set_content(actor,sa->images->turndark);
                clutter_actor_set_rotation_angle(actor,CLUTTER_Z_AXIS,-90);

            }
        }

    }
}

ClutterContent *generate_image(char * filename)
{

    ClutterContent *image = clutter_image_new ();

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (filename, NULL);

    clutter_image_set_data (CLUTTER_IMAGE (image),
                            gdk_pixbuf_get_pixels (pixbuf),
                            gdk_pixbuf_get_has_alpha (pixbuf)
                              ? COGL_PIXEL_FORMAT_RGBA_8888
                              : COGL_PIXEL_FORMAT_RGB_888,
                            gdk_pixbuf_get_width (pixbuf),
                            gdk_pixbuf_get_height (pixbuf),
                            gdk_pixbuf_get_rowstride (pixbuf),
                            NULL);

    g_object_unref (pixbuf);
    return image;
}



SnakeImage *snake_generate_image()
{
    SnakeImage *res;
    res = malloc(sizeof(struct _snake_image));
    res->tete = generate_image(TETE_IMAGE_SRC);
    res->queue = generate_image(QUEUE_IMAGE_SRC);
    res->corps = generate_image(CORPS_IMAGE_SRC);
    res->turnlight = generate_image(TURNLIGHT_IMAGE_SRC);
    res->turndark = generate_image(TURNDARK_IMAGE_SRC);
    
    return res;
}

/**
 * @brief   Crée la fenêtre du snake.
 *
 * @param[in]    ui         Le fichier ui contenant la déclaration de
 *                          la fenêtre du Snake.
 * @param[in]    width      La largeur de la fenêtre en nombre de cases de la
 *                          grille.
 * @param[in]    height     La hauteur de la fenêtre.
 * @param[in]    direction  La direction de départ du snake.
 * @param[in]    size       La longueur du snake.
 * @param[in]    pos        La position de départ du snake.
 */
void init_view(ClutterScript *ui, int width, int height, Direction direction, int size, Coord pos)
{
    ClutterActor *zone_snake;
    ClutterActor *stage;
    Snake *snk, *snk_ia;
    SnakeActor *sa, *sa_ia;
    Bouf *bouf;
    BoufActor *ba;


    stage = CLUTTER_ACTOR(clutter_script_get_object(ui, "stage"));
    clutter_actor_set_size(stage, width * GRID_SIZE, height * GRID_SIZE);
    //clutter_actor_set_position (stage, 600,500);
    
    snk = create_snake(
        size,
        pos,
        direction
    );

    snk_ia = create_snake(
        size,
        coord_from_xy(pos.x, pos.y+5),//L'ia commence 5 case en dessous du snake de base
        direction
    );


    bouf = bouf_new(width, height);

    zone_snake = CLUTTER_ACTOR(clutter_script_get_object(ui, "zone_snake"));
    clutter_stage_set_key_focus(CLUTTER_STAGE(stage), zone_snake);
    SnakeImage *simages = snake_generate_image();
    sa = create_snake_actor(zone_snake, snk, clutter_color_new (0, 0, 255, 255),simages);
    sa_ia = create_snake_actor(zone_snake, snk_ia, clutter_color_new (255, 0,0, 255),simages);
    ba = create_bouf_actor(zone_snake, bouf, clutter_color_new (0, 255, 0, 255));

    snake_actor_update(sa);
    snake_actor_update(sa_ia);
    bouf_actor_update(ba);

    g_signal_connect(zone_snake, "key-press-event", G_CALLBACK(zone_snake_key_press_cb), snk);
    g_signal_connect(stage, "destroy", G_CALLBACK(stage_destroy_cb), sa);

    UpletActor ua = uplet_actor_new(sa, sa_ia, ba);
    g_timeout_add(150, timeout_tick_cb, &ua);

    ClutterContent *image = generate_image(BACKGROUND_IMAGE_SRC );
    clutter_actor_set_content(zone_snake,image);

    clutter_actor_show(stage);

    clutter_main();

    free_snake(snk);
}
