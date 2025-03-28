/*
-----------------------------------------------------------------------------------
File name		 : wholesale.cpp

Author(s)		 : Demont Kilian et Graf Calvin

Description     : Définition des méthodes permettant de représenter les grossistes
 						qui achètent des produits des usines ainsi que des matières
 						premières qu'ils revendent.

-----------------------------------------------------------------------------------
*/

#include "wholesale.h"
#include "factory.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>

WindowInterface* Wholesale::interface = nullptr;

Wholesale::Wholesale(int uniqueId, int fund)
    : Seller(fund, uniqueId)
{
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Wholesaler Created");

}

void Wholesale::setSellers(std::vector<Seller*> sellers) {
    this->sellers = sellers;

    for(Seller* seller: sellers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

void Wholesale::buyResources() {
    auto seller = Seller::chooseRandomSeller(sellers);
    mutex.lock();
    auto itToSell = seller->getItemsForSale();
    mutex.unlock();
    auto it = Seller::chooseRandomItem(itToSell);

    if (it == ItemType::Nothing) {
        /* Nothing to buy... */
        return;
    }

    int qty = rand() % 5 + 1;
    int price = qty * getCostPerUnit(it);

    interface->consoleAppendText(uniqueId, QString("I would like to buy %1 of ").arg(qty) %
                                 getItemName(it) % QString(" which would cost me %1").arg(price));
    mutex.lock();
    // On vérifie si l'achat peut se faire
    if (itToSell.find(it) != itToSell.end() && this->money >= price && seller->trade(it, qty) != 0){
        //  On effectue l'achat
        this->stocks[it] += qty;
        this->money -= price;
    }
    mutex.unlock();
}

void Wholesale::run() {

    if (sellers.empty()) {
        std::cerr << "You have to give factories and mines to a wholeseler before launching is routine" << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Wholesaler routine");
    while (!PcoThread::thisThread()->stopRequested()) {
        buyResources();
        mutex.lock();
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
        mutex.unlock();
        //Temps de pause pour espacer les demandes de ressources
        PcoThread::usleep((rand() % 10 + 1) * 100000);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Wholesaler routine");
}

std::map<ItemType, int> Wholesale::getItemsForSale() {
    return stocks;
}

int Wholesale::trade(ItemType it, int qty) {
    mutex.lock();

    // On vérifie que la vente peut se faire
    if (qty <= 0 || this->stocks.find(it) == this->getItemsForSale().end() || this->stocks.at(it) < qty){
        mutex.unlock();
        return 0;
    } else {
        // On effectue la vente
        this->stocks.at(it) -= qty;
        this->money += getCostPerUnit(it) * qty;
        mutex.unlock();
        return getCostPerUnit(it) * qty;
    }
}

void Wholesale::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}
