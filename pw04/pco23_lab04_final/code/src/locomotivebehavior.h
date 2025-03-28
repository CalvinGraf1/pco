/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 *
 *
 * File name :   locommotivebehavior.h
 *
 * Author(s) :   Demont Kilian et Graf Calvin
 *
 * Description : Déclaration de la classe et des méthodes permettant de
 *               représenter le comportement d'une locomotive dans le
 *               système ferroviaire.
 *
*/

#ifndef LOCOMOTIVEBEHAVIOR_H
#define LOCOMOTIVEBEHAVIOR_H

#include "locomotive.h"
#include "launchable.h"
#include "synchrointerface.h"

/**
 * @brief La classe LocomotiveBehavior représente le comportement d'une locomotive
 */
class LocomotiveBehavior : public Launchable
{
public:

    /*!
     * \brief locomotiveBehavior Constructeur de la classe
     * \param loco la locomotive dont on représente le comportement
     * \param sharedSection La section partagée
     * \param nbStation Numéro de la gare
     * \param nbStartSharedSection Numéro du début de la section partagée
     * \param nbEndSharedSection Numéro de la fin de la section partagée
     * \param switchSharedSection Indique si l'aiguillage doit changer ou non
     * \param trainSwitch Tableau à 2D qui indique quel aiguillage doit être modifiée et leur état actuel (DEVIE ou TOUT_DROIT)
     */
    LocomotiveBehavior(Locomotive& loco, std::shared_ptr<SynchroInterface> sharedSection, unsigned int nbStation, unsigned int nbStartSharedSection,
                       unsigned int nbEndSharedSection, bool switchSharedSection, unsigned int trainSwitch[2][3]) : loco(loco), sharedSection(sharedSection), nbStation(nbStation),
                       nbStartSharedSection(nbStartSharedSection), nbEndSharedSection(nbEndSharedSection), switchSharedSection(switchSharedSection)
    {
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                this->trainSwitch[i][j] = trainSwitch[i][j];
            }
        }
    }


protected:
    /*!
     * \brief run Fonction lancée par le thread, représente le comportement de la locomotive
     */
    void run() override;

    /*!
     * \brief printStartMessage Message affiché lors du démarrage du thread
     */
    void printStartMessage() override;

    /*!
     * \brief printCompletionMessage Message affiché lorsque le thread a terminé
     */
    void printCompletionMessage() override;

    /**
     * @brief loco La locomotive dont on représente le comportement
     */
    Locomotive& loco;

    /**
     * @brief sharedSection Pointeur sur la section partagée
     */
    std::shared_ptr<SynchroInterface> sharedSection;

    /**
     * @brief nbStation Numéro de la gare où la locomotive doit s'arrêter
     */
    unsigned int nbStation;

    /**
     * @brief nbStartSharedSection Numéro qui représente le début de la section partagée
     */
    unsigned int nbStartSharedSection;

    /**
     * @brief nbStartSharedSection Numéro qui représente la fin de la section partagée
     */
    unsigned int nbEndSharedSection;

    /**
     * @brief switchSharedSection Indique s'il faut changer les aiguillages
     */
    bool switchSharedSection;

    /**
     * @brief trainSwitch Contient les informations concernant les aiguillages (numéro, direction, temps d'alimentation)
     */
    unsigned int trainSwitch[2][3];

};

#endif // LOCOMOTIVEBEHAVIOR_H
