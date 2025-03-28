/*
-----------------------------------------------------------------------------------
File name		 : extractor.h

Author(s)		 : Graf Calvin et Kilian Demont

Description     : Déclaration de la classe et des méthodes permettant de représenter
 						les entités qui extraient et vendent des matières premières telles
 						que le cuivre, le sable et le pétrole.

-----------------------------------------------------------------------------------
*/

#ifndef EXTRACTOR_H
#define EXTRACTOR_H
#include <QTimer>
#include<pcosynchro/pcomutex.h>
#include "windowinterface.h"
#include "costs.h"
#include "seller.h"

/**
 * @brief La classe offrant l'implémentation d'une mine et ces fonctions de ventes.
 */
class Extractor : public Seller {
public:
    static void setInterface(WindowInterface* interface);

    /**
     * @brief Constructeur d'une mine
     * @param Fonds d'initialisation de la mine
     * @param Ressource minée
     */
    Extractor(int uniqueId, int fund, ItemType resourceExtracted);

    std::map<ItemType, int> getItemsForSale() override;
    int trade(ItemType it, int qty) override;

    /**
     * @brief Routine de minage de ressources (fonction threadée)
     */
    void run();

    /**
     * @brief Fonction permettant de savoir quelle ressources la mine possède
     * @return Le type de minerai minés
     */
    ItemType getResourceMined();

    /**
     * @brief Fonction permettant de savoir le coût de la ressources minée
     * @return Le coût de la ressource minée
     */
    int getMaterialCost();

    int getAmountPaidToMiners();

private:
    // Identifiant du type de ressourcee miné
    const ItemType resourceExtracted;
    // Compte le nombre d'employé payé
    int nbExtracted;

    static WindowInterface* interface;
};


// Si dessous sont les trois types de mines. Elle vont créer des mines en leur
// spécifiant leur identifiant.

class SandExtractor : public Extractor {
private:
public:
    SandExtractor(int uniqueId, int fund);
};


class CopperExtractor : public Extractor {
private:
public:
    CopperExtractor(int uniqueId, int fund);
};


class PetrolExtractor : public Extractor {
private:
public:
    PetrolExtractor(int uniqueId, int fund);
};




#endif // EXTRACTOR_H
