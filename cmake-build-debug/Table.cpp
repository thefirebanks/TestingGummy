//
// Created by Daniel Firebanks  on 11/29/17.
//

#include "Table.h"

/* Initialize a new table with 2 player and a new deck */
Table::Table(Player* p1, Player* p2, Deck* deck) {
    players.insert(p1);
    players.insert(p2);
    current_deck = deck;

}


/* Given the current state, get the melds of all the players
 * and add them to the set of melds of the table */
void Table::updateMeld() {
    melds.clear();

    //Iterate through the players

    Player *p = *std::next(players.begin(), 0);

    for (Meld meld : p->meld_list){
        melds.insert(meld);
    }

    Player *p1 = *std::next(players.begin(), 1);

    for (Meld meld : p1->meld_list){
        melds.insert(meld);
    }



}


void Table::evaluate_draw(Player* p) const{

    //Update player's draw_from
    if (evaluate_melds(p) == true){
        p->draw_from = "Discard";
    }
    else{
        p->draw_from = "Deck";
    }

}

// FIX SORTING COMPARATOR OMG WHY IS THAT ACE OF SPADES AT THE END WHAT THE FUCK

/* Updates possible new melds for player AFTER drawing */
void Table::evaluate_hand(Player* p) const {
    //p->possible_new_melds[0]->print_meld();

    /* CHECK HAND AGAINST MELD_LIST */

    //If there are no current possible melds
    if (p->possible_melds.empty()){
        //cout<<"BRO THIS IS EMPTY"<<endl;
        sort(p->hand.begin(), p->hand.end(), compare_value);

        //Check for melds in the format 6D, 6S, 6H
        for (int i = 0; i < p->hand.size(); i++){
            Card c1 = p->hand[i];
            Card c2 = p->hand[i+1];
            if (c1.get_value() == c2.get_value()){
                p->possible_melds.push_back(c1);
                p->possible_melds.push_back(c2);
            }
        }


        //Check for melds in the format 6S,7S,8S ---------- WHAT IS THIS?
        sort(p->hand.begin(), p->hand.end(), compare_suit);

        //Check for melds in the format 6D, 6S, 6H
        for (int i = 0; i < p->hand.size(); i++){
            Card c1 = p->hand[i];
            Card c2 = p->hand[i+1];

            if (c1.get_suit() == c2.get_suit()){
                if ((c1.get_value() + 1 == c2.get_value()) || (c1.get_value() + 2 == c2.get_value())){
                    p->possible_melds.push_back(c1);
                    p->possible_melds.push_back(c2);
//MARK DRAW FROM AS DISCARD?
                }
            }
        }

    }

    if (p->possible_melds.size() == 0)
        return;

    vector<Card> copy = p->possible_melds;

    for (int i = 0; i <= copy.size() - 2; i += 2){
        Card first = copy[i];
        Card second = copy[i+1];

        //For every card in hand, check if that card can complete any of the melds
        for (int j = 0; j < p->hand.size(); j++){
            // Case where we have 6D, 7D, and we are missing an 8D or a 5D
            if ((p->hand[j].get_suit() == first.get_suit() && (p->hand[j].get_value() + 1 == first.get_value()) && (p->hand[j].get_value() == second.get_value() - 2)) || (p->hand[j].get_suit() == second.get_suit() && (p->hand[j].get_value() - 1 == second.get_value())  && (p->hand[j].get_value() == first.get_value() + 2))){

                    //have 6D, 8D, need 7D
                if  (p->hand[j].get_value() == first.get_value()+1 && p->hand[j].get_value() == second.get_value()-1){
                }


                Meld* n1_meld = new Meld(first, second, p->hand[j]);
                //Meld* extra = &n1_meld;
                copy.erase(copy.begin() + i);
                copy.erase(copy.begin() + i);


                p->possible_new_melds.push_back(n1_meld);
                //p->possible_new_melds[0]->print_meld();
                //cout<<"PRINT BRO1"<<endl;

                delete(n1_meld);
            }

            // Case where we have 6D, 6H, and we see 6S or 6C
            else if ((first.get_value() == second.get_value()) && (p->hand[j].get_value() == second.get_value())){
                if(p->hand[j].get_suit() != first.get_suit() && p->hand[j].get_suit() != second.get_suit()){


                    Meld* n2_meld = new Meld(first, second, p->hand[j]);

                    copy.erase(p->possible_melds.begin() + i);
                    copy.erase(p->possible_melds.begin() + i);
                    //cout<<"PRINT BRO2"<<endl;
                    p->possible_new_melds.push_back(n2_meld);

                    delete(n2_meld);
                }
            }
        }

    }

}

/* Updates possible new melds for player BEFORE drawing
 * returns true if it is better to draw from discard pile */
bool Table::evaluate_melds(Player* p) const{

    //Base case for when possible melds is empty
    if (p->possible_melds.size()==0){
        evaluate_hand(p); // Is this the only place it gets called?
        if (p->possible_melds.empty()){
            return false;
        }
    }
    //cout<<"GOT HERE BRO"<<endl;


    //Else, check with discard pile
    if (current_deck->discard_pile.size() == 0)
        return false;

    vector<Card> discard = current_deck->discard_pile;

    Card top_discard = discard.back();
    //cout<<"Discarded card is:"<<endl;
    //top_discard.print_card();
    //cout<<"GOT HERE BRO"<<endl;

    //cout<<"TOP DISCARD IS "<<endl;
    //top_discard.print_card();
    bool added = false;

    vector<Card> copy = p->possible_melds;
    //cout<<"BRO"<<copy.size()<<endl;

    for (int i = 0; i <= copy.size() - 2; i += 2){
        //cout<<"GOT HERE BRO"<<endl;

        Card first = copy.at(i);
        Card second = copy.at(i+1);

        // Case where we have 6D, 7D, and we are missing an 8D or a 5D
        if ((top_discard.get_suit() == first.get_suit() && (top_discard.get_value() + 1 == first.get_value()) && (top_discard.get_value() == second.get_value() - 2)) || (top_discard.get_suit() == second.get_suit() && (top_discard.get_value() - 1 == second.get_value())  && (top_discard.get_value() == first.get_value() + 2))){
            //Create the meld

            //if you have 6D 8D need 7D
            if  (p->hand[i].get_value() == first.get_value()+1 && p->hand[i].get_value() == second.get_value()-1){
                //cout<<"PRINT BRO"<<endl;
            }


            Meld new_meld(first, second, top_discard);

            //Delete it from possible melds
            copy.erase(copy.begin() + i);
            copy.erase(copy.begin() + i);

            //Add it to possible NEW melds
            //cout<<"PRINT BRO3"<<endl;
            p->possible_new_melds.push_back(&new_meld);
            added = true;
        }

        // Case where we have 6D, 6H, and we see 6S or 6C
        else if ((first.get_value() == second.get_value()) && (top_discard.get_value() == second.get_value())){
            //Create the meld
            Meld new_meld(first, second, top_discard);

            //Delete it from possible melds
            copy.erase(p->possible_melds.begin() + i);
            copy.erase(p->possible_melds.begin() + i);

            //Add it to possible NEW melds
            //cout<<"PRINT BRO4"<<endl;
            p->possible_new_melds.push_back(&new_meld);
            added = true;
        }
    }

    return added;
}

void Table::evaluate_discard(Player* p) const{
    //clear useless cards
    p->useless_cards.clear();

    //fill useless_cards in order
    sort(p->hand.begin(), p->hand.end(), compare_value);
    for (int j = 0; j < p->hand.size(); j++) {
        bool contains = false;
        Card c = p->hand[j];
        //Card hold = Card(c.get_value(), c.get_suit(), c.get_score());
        //cout<<p->possible_new_melds.size()<<endl;
        //p->possible_new_melds[0]->print_meld();
        if (p->possible_new_melds.size() == 0) {
            for (int i = 0; i < p->hand.size(); i++) {
                p->discard_index.push_back(i);
            }
            //cout<<"TEST BRO"<<endl;
        }
        else{
            for (int i = 0; i < p->possible_new_melds.size(); i++) {
                //p->possible_new_melds[i]->print_meld();
                //cout << "Size of card is" << endl;

                //cout<<"Size of meld is "<<p->possible_new_melds[i]->print_meld()<<endl;

                //p->possible_new_melds[i]->print_meld();
                for (Card k : p->possible_new_melds[i]->get_meld()) {
                    //cout<<"Size of card is"<<endl;

                    //TODO CHANGE TO STRCMP!!!!!!!
                    if (k.get_value() == c.get_value() && k.get_suit() == c.get_suit()) { //if possible melds contains c
                        //cout<<"GOT HERE BRO"<<endl;
                        contains = true;
                        p->no_discard_index.push_back(j);
                    }
                }
            }
            if (!contains) {

                //hold->print_card();

                //cout<<"Size of vector is "<<p->useless_cards.size()<<endl;
                p->useless_cards.push_back(c);

                //Add index to reference later and discard
                p->discard_index.push_back(j);
            }
        }
    }
}


bool Table::compare_suit(Card c1, Card c2) {
    if (c1.get_suit() == c2.get_suit())
        return c1.get_value() < c2.get_value();
    else
        return c1.get_suit() < c2.get_suit();
}

bool Table::compare_value(Card c1, Card c2) {
    return c1.get_value() < c2.get_value();
}

void Table::play_meld(Player *p) {
    if (p->possible_new_melds.size() != 0){ // If melds to play is not size zero
        for (Meld* m : p->possible_new_melds){ //play melds to table
            melds.insert(*m);
            p->meld_list.push_back(*m);

            for (int j = 0; j < m->meld.size(); j++) { // erase from hand
                for (int i = 0; i < p->hand.size(); i++){
                    if (p->hand[i].get_value() == m->meld[j].get_value() && p->hand[i].get_suit() == m->meld[j].get_suit()) {
                        p->hand.erase(p->hand.begin()+i);
                        i--;
                    }
                }
            }
            p->possible_new_melds.erase(p->possible_new_melds.begin());
        }
    }
}

