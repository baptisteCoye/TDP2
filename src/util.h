#ifndef UTIL_H
#define UTIL_H

#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include"datatype.h"

/*!
 * \brief calcule la distance entre deux particules.
 *
 * \param A Une premiere particule
 * \param B Une seconde particule
 *
 * \return La distance entre A et B
 */
double distance(particule A, particule B);

/*!
 * \brief calcule l'interaction entre deux particules
 *
 * \param A La première particule
 * \param B La deuxième particule
 */
vecteur force_interaction(particule A, particule B);

/*!
 * \brief Lis les données sur les particules à l'état initial.
 *
 * \param[in] filename Le nom du fichier à ouvrir.
 * \param[out] data L'adresse du tableau à allouer et où stocker les données.
 *
 * \return Le nombre de particules contenues dans le fichier.
 */

particule* readData(char* filename, int* k);

/*!
 * \brief Cacule les forces induites par un groupe de particule sur lui-meme.
 * \details On ne vérifie i != j que dans cette fonction.
 * 
 * \param[in] data Les particules sur lesquelles nous calculons.
 * \param[in] N Le nombre de particules à considérer.
 * \param[out] force Les valeurs des forces sur les particules.
 */
void calcul_local(vecteur* force, particule* data, int N);

/*!
 * \brief Calcule les forces induites par un groupe de particules A sur un autre groupe B.
 * \details Les groupes de particules étant disjoints, on ne vérifie plus que les particules soient différentes.
 *
 * \param[in] buffer Les particules du groupe A
 * \param[in] data Les particules du groupe B
 * \param[in] N Le nombre de particules de chaque groupe.
 */
void calcul_lointain(vecteur* force, particule* buffer, particule* data, int N);

/*!
 * \brief Copie les informations d'un tableau de particule vers un autre.
 * \param[in] src Le tableau contenant les informations de départ.
 * \param[in] size La taille des deux tableaux.
 * \param[out] dst Le tableau dans lequel les informations sont copiées.

 */ 
void copier(particule* dst, particule* src, int size);

void move_particules();

void save_results();

#endif
