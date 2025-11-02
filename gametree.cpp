#include <utility>
#include <iostream>

#include "PokerEngine/core/card.hpp"
#include "PokerEngine/core/factory/deck_factory.hpp"
#include "PokerEngine/game_theory/game_tree_builder.hpp"

using namespace PokerEngine::Core::literals;

int main() {
    PokerEngine::GameTheory::GameTreeBuilderConfig config {
        .betFractions {0.25, 0.5, 1.0}, .maxDepth {10}
    };
    
    PokerEngine::GameTheory::GameTreeBuilder builder{config};
    auto deck = PokerEngine::Core::Factory::DeckFactory::createStandardDeck();
    PokerEngine::Core::Board board{{"Ad"_c, "7h"_c, "5c"_c, "Tc"_c}};
    
    PokerEngine::Core::Range p1_range{};
    p1_range.addCombo("Kh"_c, "Qh"_c);
    PokerEngine::Core::Range p2_range{};
    p2_range.addCombo("8c"_c, "6c"_c);
    
    PokerEngine::GameTheory::PlayerState p1{
        .id = 0,
        .stack = PokerEngine::Core::Stack{100},
        .range {std::move(p1_range)},
        .currentBet = 0
    };
    PokerEngine::GameTheory::PlayerState p2{
        .id = 1,
        .stack = PokerEngine::Core::Stack{100},
        .range {std::move(p2_range)},
        .currentBet = 0
    };
    deck.remove(board.get());

    PokerEngine::GameTheory::RoundState round_state {
        .street = PokerEngine::GameTheory::Street::TURN,
    };
    
    PokerEngine::Core::Pot pot{};
    pot.addContribution(p1.id, 5);
    pot.addContribution(p2.id, 5);

    PokerEngine::GameTheory::DecisionState root_state{
        .round = round_state,
        .players{std::move(p1), std::move(p2)},
        .pot = std::move(pot),
        .board{std::move(board)},
        .deck{std::move(deck)}
    };

    root_state.deck.shuffle();
    auto game_tree = builder.buildTree(root_state);
}