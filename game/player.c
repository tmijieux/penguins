/**
 * @file player.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    for (int i = 0; i < player_count; i++) {
	score[i] = 0;
    }
}

/**
 * Initialisation du module player.
 */
void player_module_init(void)
{
    client_module_init();
    player_count = client_get_client_count();
    if (score == NULL) {
	score = calloc(player_count, sizeof(*score));
    } else {
	player_init_score();
    }
    if (penguin_count == NULL) {
	penguin_count = calloc(player_count, sizeof(*penguin_count));
    }
    if (state == NULL) {
	state = malloc(player_count * sizeof(*state));
    }
    for (int i = 0; i < player_count; i++) {
	state[i] = 1;
    }
}

/**
 * Libération de la mémoire occupée par le module player.
 */
void player_module_exit(void)
{
    player_count = 0;
    free(score);
    score = NULL;
    free(state);
    state = NULL;
    free(penguin_count);
    penguin_count = NULL;

    client_module_exit();
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
void player_play(int player_id, struct move *move)
{
    log_print(DEBUG_LOG__, "player_play: %d\n", player_id);

    Client *cli = client_get(player_id);
    cli->methods.play(move);
}

/**
 * Faire placer un pingouin à un joueur.
 * @param player - Identifiant du joueur.
 * @return int - Position du pingouin.
 */
int player_place_penguin(int player_id)
{
    Client *cli = client_get(player_id);
    penguin_count[player_id] ++;
    return cli->methods.place_penguin();
}

/**
 * Initialiser les joueurs.
 * @param player - Identifiant du joueur.
 * @param nb_tile - Nombre de tuiles
 */
void player_init_player(int player_id, int nb_tile)
{
    Client *cli = client_get(player_id);
    cli->methods.init(nb_tile);
}

/**
 * Envoyer le dernier coup joué à un joueur.
 * @param dt - Type de différence.
 * @param player - Identifiant du joueur.
 * @param orig - Origine du mouvement.
 * @param dest - Destination du mouvement ou placer le
 * pingouin.
 */
void player_send_diff(enum diff_type dt, int player_id,
		      int orig, int dest)
{
    Client *cli = client_get(player_id);
    cli->methods.send_diff(dt, orig, dest);
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


int cmp(const void* A, const void* B) {
    const int *a = A;
    const int *b = B;
    return *a < *b;
}

void player_display_board(void)
{
    int *sorted_score = calloc(player_count, sizeof(int));
    memcpy(sorted_score, score, player_count*sizeof(int));

    qsort(sorted_score, player_count, sizeof(int), cmp);

    for (int i = 0; i < player_count; i++)
    {
        for (int p = 0; p < player_count; ++p)
        {
            if (score[p] == sorted_score[i])
            {
                printf("%d points - Player %d - %s\n", score[p], p, player_get_name(p));
                break;
            }
        }
    }
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
