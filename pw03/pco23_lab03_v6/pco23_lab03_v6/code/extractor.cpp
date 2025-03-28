/*
-----------------------------------------------------------------------------------
File name		 : extractor.cpp

Author(s)		 : Demont Kilian et Graf Calvin

Description     : Définition des méthodes permettant de représenter les entités
 						qui extraient et vendent des matières premières telles que le
 						cuivre, le sable et le pétrole.

-----------------------------------------------------------------------------------
*/

#include "extractor.h"
#include "costs.h"
#include <pcosynchro/pcothread.h>
#include<pcosynchro/pcomutex.h>


WindowInterface* Extractor::interface = nullptr;

Extractor::Extractor(int uniqueId, int fund, ItemType resourceExtracted)
    : Seller(fund, uniqueId), resourceExtracted(resourceExtracted), nbExtracted(0)
{
    assert(resourceExtracted == ItemType::Copper ||
           resourceExtracted == ItemType::Sand ||
           resourceExtracted == ItemType::Petrol);
    interface->consoleAppendText(uniqueId, QString("Mine Created"));
    interface->updateFund(uniqueId, fund);
}

std::map<ItemType, int> Extractor::getItemsForSale() {
    return stocks;
}

int Extractor::trade(ItemType it, int qty) {
   mutex.lock();
   // On vérifie si la vente peut se faire
   if (qty <= 0 || this->stocks.find(it) == this->getItemsForSale().end() || this->stocks.at(it) < qty) {
       mutex.unlock();
       return 0;
   }
   else {
       // On effectue la vente
       this->stocks.at(it) -= qty;
       this->money += getCostPerUnit(it) * qty;
       mutex.unlock();
       return getCostPerUnit(it) * qty;
   }
}

void Extractor::run() {
    interface->consoleAppendText(uniqueId, "[START] Mine routine");

    while (!PcoThread::thisThread()->stopRequested()) {
        int minerCost = getEmployeeSalary(getEmployeeThatProduces(resourceExtracted));

        mutex.lock();
        if (money < minerCost) {
            mutex.unlock();
            /* Pas assez d'argent */
            /* Attend des jours meilleurs */
            PcoThread::usleep(1000U);
            continue;
        }

        /* On peut payer un mineur */
        money -= minerCost;
        mutex.unlock();

        /* Temps aléatoire borné qui simule le mineur qui mine */
        PcoThread::usleep((rand() % 100 + 1) * 10000);

        /* Statistiques */
        nbExtracted++;

        /* Incrément des stocks */
        mutex.lock();
        stocks[resourceExtracted] += 1;
        mutex.unlock();

        /* Message dans l'interface graphique */
        interface->consoleAppendText(uniqueId, QString("1 ") % getItemName(resourceExtracted) %
                                     " has been mined");

        /* Update de l'interface graphique */
        mutex.lock();
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
        mutex.unlock();
    }
    interface->consoleAppendText(uniqueId, "[STOP] Mine routine");
}

int Extractor::getMaterialCost() {
    return getCostPerUnit(resourceExtracted);
}

ItemType Extractor::getResourceMined() {
    return resourceExtracted;
}

int Extractor::getAmountPaidToMiners() {
    return nbExtracted * getEmployeeSalary(getEmployeeThatProduces(resourceExtracted));
}

void Extractor::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

SandExtractor::SandExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Sand) {}

CopperExtractor::CopperExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Copper) {}

PetrolExtractor::PetrolExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Petrol) {}
