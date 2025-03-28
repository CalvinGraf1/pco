/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 *
 *
 * File name :   locommotivebehavior.cpp
 *
 * Author(s) :   Demont Kilian et Graf Calvin
 *
 * Description : Définition des méthodes permettant de représenter
 *               le comportement d'une locomotive dans le système
 *               ferroviaire.
 *
*/


#include "locomotivebehavior.h"
#include "ctrain_handler.h"

void LocomotiveBehavior::run()
{
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    while(true) {
        attendre_contact(nbStation);
        loco.afficherMessage("Je suis arrivée à la gare.");
        sharedSection->stopAtStation(loco);

        attendre_contact(nbStartSharedSection);
        loco.afficherMessage("Je suis arrivée au début de la section partagée.");
        sharedSection->access(loco);

        if(switchSharedSection) {
            for(int i = 0; i < 2; ++i)
                diriger_aiguillage(trainSwitch[i][0], trainSwitch[i][1], trainSwitch[i][2]);
        }

        attendre_contact(nbEndSharedSection);
        loco.afficherMessage("Je suis arrivée à la fin de la section partagée");
        sharedSection->leave(loco);

        if(switchSharedSection) {
            for(int i = 0; i < 2; ++i)
                diriger_aiguillage(trainSwitch[i][0], !trainSwitch[i][1], trainSwitch[i][2]);
        }
    }
}

void LocomotiveBehavior::printStartMessage()
{
    qDebug() << "[START] Thread de la loco" << loco.numero() << "lancé";
    loco.afficherMessage("Je suis lancée !");
}

void LocomotiveBehavior::printCompletionMessage()
{
    qDebug() << "[STOP] Thread de la loco" << loco.numero() << "a terminé correctement";
    loco.afficherMessage("J'ai terminé");
}
