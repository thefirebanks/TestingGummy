//
// Created by Daniel Firebanks  on 11/29/17.
//
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include "card.h"

#ifndef GINRUMMY_DECK_H
#define GINRUMMY_DECK_H

using namespace std;

class Deck {
    int total_cards = 52;
    int curr_size = total_cards;

    string suits[4] = {"Spades", "Hearts", "Clubs", "Diamonds"};
    vector<Card> deck;


public:
    vector<Card> discard_pile;
    Deck();
    void shuffle();
    int get_size();
    vector<Card> get_deck();
    Card draw();
    Card draw_discard();
    void discard(Card card);
    void print_deck();

};


#endif //GINRUMMY_DECK_H
