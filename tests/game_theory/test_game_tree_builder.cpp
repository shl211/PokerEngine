#include <gtest/gtest.h>

#include <algorithm>
#include "PokerEngine/game_theory/game_tree_builder.hpp"
#include "PokerEngine/core/factory/deck_factory.hpp"

using namespace PokerEngine::GameTheory;
using namespace PokerEngine::Core;
using namespace PokerEngine::Core::literals;

DecisionState createHeadsUpTurnScenario() {
    Range p1_range{};
    p1_range.addCombo("Ad"_c, "Kd"_c);
    Range p2_range{};
    p2_range.addCombo("Tc"_c, "Qc"_c);

    PlayerState p1 {.id{0}, .stack{Stack{100}}, .range{std::move(p1_range)}};
    PlayerState p2 {.id{1}, .stack{Stack{100}}, .range{std::move(p2_range)}};
    RoundState round {
        .street{ Street::TURN }, .currentPlayerIndex{0}, .firstToActIndex{0}
    };
    Pot pot;
    pot.addContribution(p1.id, 10);
    pot.addContribution(p2.id, 10);
    Board board{{"7h"_c, "2s"_c, "Jc"_c, "2c"_c}};
    
    Deck deck = Factory::DeckFactory::createStandardDeck();
    deck.remove(board.get());

    return DecisionState {
        .round = std::move(round),
        .players = {std::move(p1), std::move(p2)},
        .pot = std::move(pot),
        .board = std::move(board),
        .deck = std::move(deck)
    };
}

std::shared_ptr<GameTreeNode> findActionTypeInGameTreeChild(const std::shared_ptr<GameTreeNode>& root, ActionType type) {
    auto it = std::find_if(
        root->children.begin(), root->children.end(),
        [type](const ActionEdge& val) {return val.action == type; }
    );

    if (it != root->children.end()) {
        return it->next;
    }

    return nullptr;
}

TEST(GameTreeBuilder, TestOnlyValidChildrenOnFirstPlayer) {
    GameTreeBuilder builder(
        GameTreeBuilderConfig { .betFractions{0.5}, .maxDepth{10} }
    );

    DecisionState root_state = createHeadsUpTurnScenario();
    auto gametree = builder.buildTree(root_state);
    
    ASSERT_EQ(gametree->children.size(), 2);//p1 can CHECK or BET
    auto check_node = findActionTypeInGameTreeChild(gametree, ActionType::CHECK);
    auto bet_node = findActionTypeInGameTreeChild(gametree, ActionType::BET);
    auto call_node = findActionTypeInGameTreeChild(gametree, ActionType::CALL);
    auto fold_node = findActionTypeInGameTreeChild(gametree, ActionType::FOLD);
    auto raise_node = findActionTypeInGameTreeChild(gametree, ActionType::RAISE);
    auto deal_node = findActionTypeInGameTreeChild(gametree, ActionType::DEAL);
    ASSERT_TRUE(check_node);
    ASSERT_TRUE(bet_node);
    ASSERT_FALSE(call_node);
    ASSERT_FALSE(fold_node);
    ASSERT_FALSE(raise_node);
    ASSERT_FALSE(deal_node);
}

TEST(GameTreeBuilder, TestOnlyValidChildrenOnRaise) {
    GameTreeBuilder builder(
        GameTreeBuilderConfig { .betFractions{0.5}, .maxDepth{10} }
    );

    DecisionState root_state = createHeadsUpTurnScenario();
    auto gametree = builder.buildTree(root_state);
    
    ASSERT_EQ(gametree->children.size(), 2);//p1 can CHECK or BET
    auto p1_bet_node = findActionTypeInGameTreeChild(gametree, ActionType::BET);
    ASSERT_TRUE(p1_bet_node);

    //check actions, should only be CALL, RAISE, FOLD
    ASSERT_EQ(p1_bet_node->children.size(), 3);
    auto check_node = findActionTypeInGameTreeChild(p1_bet_node, ActionType::CHECK);
    auto bet_node = findActionTypeInGameTreeChild(p1_bet_node, ActionType::BET);
    auto call_node = findActionTypeInGameTreeChild(p1_bet_node, ActionType::CALL);
    auto fold_node = findActionTypeInGameTreeChild(p1_bet_node, ActionType::FOLD);
    auto raise_node = findActionTypeInGameTreeChild(p1_bet_node, ActionType::RAISE);
    auto deal_node = findActionTypeInGameTreeChild(p1_bet_node, ActionType::DEAL);
    ASSERT_TRUE(fold_node);
    ASSERT_TRUE(call_node);
    ASSERT_TRUE(raise_node);
    ASSERT_FALSE(check_node);
    ASSERT_FALSE(bet_node);
    ASSERT_FALSE(deal_node);
}

TEST(GameTreeBuilder, TestDepthEnforced) {
    GameTreeBuilder builder(
        GameTreeBuilderConfig { .betFractions{0.5}, .maxDepth{1} }
    );

    DecisionState root_state = createHeadsUpTurnScenario();
    auto gametree = builder.buildTree(root_state);

    ASSERT_EQ(gametree->children.size(), 2);
    for(const auto& child : gametree->children) {
        const auto& next_node = child.next;
        ASSERT_TRUE(next_node->children.empty());//NO MORE THAN DEPTH OF 1
    }
}

TEST(GameTreeBuilder, TestMultipleBetFractions) {
    GameTreeBuilder builder(
        GameTreeBuilderConfig { .betFractions{0.5, 1}, .maxDepth{1} }
    );

    DecisionState root_state = createHeadsUpTurnScenario();
    auto gametree = builder.buildTree(root_state);

    ASSERT_EQ(gametree->children.size(), 3);//CHECK, BET 0.5x, BET 1x pot
}

TEST(GameTreeBuilder, TestTurnHeadsUpScenario1) {
    GameTreeBuilder builder(
        GameTreeBuilderConfig { .betFractions{0.5}, .maxDepth{10} }
    );

    DecisionState root_state = createHeadsUpTurnScenario();
    auto gametree = builder.buildTree(root_state);
    
    //First line of action should be CHECK CHECK DEAL CHECK CHECK TERMINAL
    ASSERT_EQ(gametree->children.size(), 2);//p1 can CHECK or BET
    auto p1_turn_check_node = findActionTypeInGameTreeChild(gametree, ActionType::CHECK);
    ASSERT_TRUE(p1_turn_check_node);
    ASSERT_EQ(p1_turn_check_node->state.round.street, Street::TURN);
    
    ASSERT_EQ(p1_turn_check_node->children.size(), 2);//p2 can CHECK or BET
    auto p2_turn_check_node = findActionTypeInGameTreeChild(p1_turn_check_node, ActionType::CHECK);
    ASSERT_TRUE(p2_turn_check_node);
    ASSERT_EQ(p2_turn_check_node->state.round.street, Street::TURN);
    
    ASSERT_EQ(p2_turn_check_node->children.size(), p2_turn_check_node->state.deck.size());//Cards are now dealt
    auto river_deal = findActionTypeInGameTreeChild(p2_turn_check_node, ActionType::DEAL);
    ASSERT_TRUE(river_deal);
    ASSERT_EQ(river_deal->state.round.street, Street::RIVER);

    ASSERT_EQ(river_deal->children.size(), 2);//p1 can CHECK or BET
    auto p1_river_check_node = findActionTypeInGameTreeChild(river_deal, ActionType::CHECK);
    ASSERT_TRUE(p1_river_check_node);
    ASSERT_EQ(p1_river_check_node->state.round.street, Street::RIVER);

    ASSERT_EQ(p1_river_check_node->children.size(), 2);//p2 can CHECK or BET
    auto p2_river_check_node = findActionTypeInGameTreeChild(p1_river_check_node, ActionType::CHECK);
    ASSERT_TRUE(p2_river_check_node);
    ASSERT_EQ(p2_river_check_node->state.round.street, Street::RIVER);

    ASSERT_TRUE(p2_river_check_node->state.isTerminal());
    ASSERT_TRUE(p2_river_check_node->children.empty());
}

TEST(GameTreeBuilder, TestTurnHeadsUpScenario2) {
    GameTreeBuilder builder(
        GameTreeBuilderConfig { .betFractions{0.5}, .maxDepth{10} }
    );

    DecisionState root_state = createHeadsUpTurnScenario();
    auto gametree = builder.buildTree(root_state);
    
    //First line of action should be BET FOLD
    ASSERT_EQ(gametree->children.size(), 2);//p1 can CHECK or BET
    auto p1_turn_raise_node = findActionTypeInGameTreeChild(gametree, ActionType::BET);
    ASSERT_TRUE(p1_turn_raise_node);
    ASSERT_EQ(p1_turn_raise_node->state.round.street, Street::TURN);
    
    ASSERT_EQ(p1_turn_raise_node->children.size(), 3);//p2 can CALL, RAISE, or FOLD
    auto p2_fold_node = findActionTypeInGameTreeChild(p1_turn_raise_node, ActionType::FOLD);
    ASSERT_TRUE(p2_fold_node);
    ASSERT_EQ(p2_fold_node->state.round.street, Street::TURN);
    
    ASSERT_TRUE(p2_fold_node->state.isTerminal());
    ASSERT_TRUE(p2_fold_node->children.empty());
}