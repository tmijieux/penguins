/**
 * @file player.c
 */

#include <stdlib.h>

#include <server/client.h>
#include <server/player.h>
#include <utils/log.h>

static int player_count = 0;
static int *score = NULL;
static int *state = NULL;
static int *penguin_count = NULL;

/**
 * Initialisation des scores.
 */
static void player_init_score(void)
{
    for (int i = 0; i < player_count; i++)
	score[i] = 0;
}

/**
 * Initialisation du module player.
 */
void player_init(void)
{
    client_init();
    player_count = client_get_client_count();
    if (score == NULL)
	score = calloc(player_count, sizeof(*score));
    else
	player_init_score();
    if (penguin_count == NULL)
	penguin_count = calloc(player_count, sizeof(*penguin_count));
    if(state == NULL)
	state = malloc(player_count * sizeof(*state));
    for (int i = 0; i < player_count; i++)
	state[i] = 1;
}

/**
 * Libération de la mémoire occupée par le module player.
 */
void player_exit(void)
{
    player_count = 0;
    free(score);
    score = NULL;
    free(state);
    state = NULL;
    free(penguin_count);
    penguin_count = NULL;
    client_exit();
}

/**
 * Obtenir le nombre de joueurs.
 * @return int - Nombre de joueurs.
 */
int player_get_player_count(void)
{
    return player_count;
}

/**
 * Obtenir le score d'un joueur.
 * @param player - Identifiant du joueur.
 * @return int - Score du joueur.
 */
int player_get_player_score(int player)
{
    return score[player];
}

/**
 * Obtenir le nombre de pingouins d'un joueur.
 * @param player - Identifiant du joueur.
 * @return int - Nombre de pingouins du joueur.
 */
int player_get_penguin_count(int player)
{
    return penguin_count[player];
}

/**
 * Savoir si le joueur peut jouer.
 * @param player - Identifiant du joueur.
 * @return int - 1, il peut jouer, 0 sinon.
 */
int player_can_play(int player)
{
    return state[player];
}

/**
 * Incrémenter le score d'un joueur.
 * @param player - Identifiant du joueur.
 * @param fish - Nombre de poisson à ajouter.
 */
void player_score_add(int player, int fish)
{
    score[player] += fish;
}

/**
 * Faire jouer un joueur.
 * @param player - Identifiant du joueur.
 * @param move - Mouvement à remplir.
 */
void player_play(int player, struct move *move)
{
    log_print(DEBUG_LOG__, "player_play: %d\n", player);
    void (*play__) (struct move*) =
	client_get_method(player, "client_play");
    play__(move);
}

/**
 * Faire placer un pingouin à un joueur.
 * @param player - Identifiant du joueur.
 * @return int - Position du pingouin.
 */
int player_place_penguin(int player)
{
    int (*place_penguin__) (void) =
	client_get_method(player, "client_place_penguin");
    penguin_count[player] ++;
    return place_penguin__();
}

/**
 * Initialiser les joueurs.
 * @param player - Identifiant du joueur.
 * @param nb_tile - Nombre de tuiles 
 */
void player_init_player(int player, int nb_tile)
{
    void (*init__) (int) =
	client_get_method(player, "client_init");
    init__(nb_tile);
}

/**
 * Envoyer le dernier coup joué à un joueur.
 * @param dt - Type de différence.
 * @param player - Identifiant du joueur.
 * @param orig - Origine du mouvement.
 * @param dest - Destination du mouvement ou placer le
 * pingouin.
 */
void player_send_diff(enum diff_type dt, int player,
		      int orig, int dest)
{
    void (*send_diff__) (enum diff_type, int, int)
	= client_get_method(player, "send_diff");
    send_diff__(dt, orig, dest);
}

/**
 * Ré-initialisation du score d'un joueur.
 * @param player - Identifiant du joueur.
 */
void player_score_reset(int player)
{
    score[player] = 0;
}

/**
 * Jeter un joueur de la partie.
 * @param player - Identifiant du joueur.
 */
void player_kick_player(int player)
{
    state[player] = 0;
}

/**
 * Supprimer un pingouin d'un joueur.
 * @param player - Identifiant du joueur.
 * @param place_phase - Si on est en phase de placement.
 */
void player_remove_penguin(int player, int place_phase)
{
    penguin_count[player] --;
    if (penguin_count[player] <= 0 && !place_phase) {
	if (!player_can_play(player))
	    return;
	player_kick_player(player);
	log_print(INFO_LOG__, "player %d kicked because "
		  "he/she has no penguins left\n", player);
    }
}

/**
 * Obtenir le joueur gagnant la partie.
 * @return int - Identifiant du gagnant.
 */
int player_get_winner(void)
{
    int winner = -1, max_score = -1;
    for (int i = 0; i < player_count; i++) {
	if (score[i] > max_score) {
	    winner = i;
	    max_score = score[i];
	}
    }
    return winner;
}

/**
 * Obtenir le nom d'un joueur.
 * @param player - Identifiant du joueur.
 * @return const char * - Nom du joueur. 
 */
const char *player_get_name(int player)
{
    return client_get_name(player);
}
