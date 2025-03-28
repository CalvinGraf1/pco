/*
-----------------------------------------------------------------------------------
File name		 : factory.cpp

Author(s)		 : Demont Kilian et Graf Calvin

Description     : Définition des méthodes permettant de représenter les usines qui
 						achètent et transforment les matières premières en produits
 						finis qu'elles vendent

-----------------------------------------------------------------------------------
*/

#include "factory.h"
#include "extractor.h"
#include "costs.h"
#include "wholesale.h"
#include <pcosynchro/pcothread.h>
#include <iostream>

WindowInterface* Factory::interface = nullptr;


Factory::Factory(int uniqueId, int fund, ItemType builtItem, std::vector<ItemType> resourcesNeeded)
    : Seller(fund, uniqueId), resourcesNeeded(resourcesNeeded), itemBuilt(builtItem), nbBuild(0)
{
    assert(builtItem == ItemType::Chip ||
           builtItem == ItemType::Plastic ||
           builtItem == ItemType::Robot);

    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Factory created");
}

void Factory::setWholesalers(std::vector<Wholesale *> wholesalers) {
    Factory::wholesalers = wholesalers;

    for(Seller* seller: wholesalers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

ItemType Factory::getItemBuilt() {
    return itemBuilt;
}

int Factory::getMaterialCost() {
    return getCostPerUnit(itemBuilt);
}

bool Factory::verifyResources() {
    for (auto item : resourcesNeeded) {
        if (stocks[item] == 0) {
            return false;
        }
    }
    return true;
}

void Factory::buildItem() {
    EmployeeType employee = getEmployeeThatProduces(getItemBuilt());

    mutex.lock();
    if(getEmployeeSalary(employee) <= money)
    {
        mutex.unlock();

        //Temps simulant l'assemblage d'un objet.
        PcoThread::usleep((rand() % 100) * 100000);

        nbBuild++;

        mutex.lock();

        // On paie l'employé
        this->money -= getEmployeeSalary(employee);

        // On dimminue le stock
        this->stocks.at(itemBuilt)++;
        for(auto it : resourcesNeeded) {
           this->stocks.find(it)->second -= 1;
        }
        mutex.unlock();
         interface->consoleAppendText(uniqueId, "Factory have build a new object");
    }
    else {
        mutex.unlock();
        interface->consoleAppendText(uniqueId, "Factory does not have the funds to build a new object");
    }
}

void Factory::orderResources() {
    /* 1. On récupère la première ressource qu'on on a besoin dont il n'y a plus de stock
     * 2. On cherche quelle grossiste possède la ressource
     * 3. On essaie de l'acheter */

    mutex.lock();
    for(auto it : resourcesNeeded) {
             if(this->stocks.find(it)->second == 0) {
                for(auto seller : this->wholesalers) {
                    for (auto item : seller->getItemsForSale()) {
                        if(it == item.first && item.second > 0) {
                            int qty = 1;
                            int totalCost = qty * getCostPerUnit(it);

                            // On vérifie si on a l'argent nécessaire
                            if(totalCost > money)  {
                                mutex.unlock();
                                return;
                            }
                            else if(seller->trade(it, qty) != 0) {
                                // On effectue l'achat
                                this->stocks.find(it)->second += qty;
                                this->money -= totalCost;
                            }
                            mutex.unlock();
                            return;
                        }
                    }
                }
             }
        }
    mutex.unlock();

    //Temps de pause pour éviter trop de demande
    PcoThread::usleep(10 * 100000);

}

void Factory::run() {
    if (wholesalers.empty()) {
        std::cerr << "You have to give to factories wholesalers to sales their resources" << std::endl;
        return;
    }
    interface->consoleAppendText(uniqueId, "[START] Factory routine");
    while (!PcoThread::thisThread()->stopRequested()) {
        if (verifyResources()) {
            buildItem();
        } else {
            orderResources();
        }
        mutex.lock();
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
        mutex.unlock();
    }
    interface->consoleAppendText(uniqueId, "[STOP] Factory routine");
}

std::map<ItemType, int> Factory::getItemsForSale() {
    return std::map<ItemType, int>({{itemBuilt, stocks[itemBuilt]}});
}

int Factory::trade(ItemType it, int qty) {
    mutex.lock();
    int stock = stocks[getItemBuilt()];

    // On vérifie que la vente peut se faire
    if (it != getItemBuilt()) {
        mutex.unlock();
        return 0;
    }
    else if (stock < 1)  {
        mutex.unlock();
        return 0;
    }
    else if (qty > stock) {
        mutex.unlock();
        return 0;
    }

    // On effectue la vente
    int totalCost = getCostPerUnit(it) * qty;
    this->stocks[it] -= qty;
    this->money += totalCost;
    mutex.unlock();
    return totalCost;
}

int Factory::getAmountPaidToWorkers() {
    return Factory::nbBuild * getEmployeeSalary(getEmployeeThatProduces(itemBuilt));
}

void Factory::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

PlasticFactory::PlasticFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Plastic, {ItemType::Petrol}) {}

ChipFactory::ChipFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Chip, {ItemType::Sand, ItemType::Copper}) {}

RobotFactory::RobotFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Robot, {ItemType::Chip, ItemType::Plastic}) {}
