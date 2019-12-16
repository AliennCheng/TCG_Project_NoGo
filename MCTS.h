//
// Created by Alienn Cheng on 2019/12/10.
// Reference: https://github.com/memo/ofxMSAmcts/tree/master
//

#ifndef NOGO_MCTS_H
#define NOGO_MCTS_H
#include <ctime>
#include "board.h"
#include "TreeNode.h"

class MCTS
{
private:
    int iterations;
    unsigned int maxIterations;
    unsigned int maxTime;
    struct timespec ts1, ts2;
    double time_elapsed;
    default_random_engine rng;

public:

    MCTS() :iterations(0),
            maxIterations(100000),
            maxTime(1000),
            time_elapsed(0)
    {
        rng = default_random_engine {};
//cout << "MCTS created" << endl;
    }

    double getTime() {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts2);
        time_elapsed = 1000.0*ts2.tv_sec + 1e-6*ts2.tv_nsec
                       - (1000.0*ts1.tv_sec + 1e-6*ts1.tv_nsec);
        return time_elapsed;
    }

    const int getIterations() {
        return iterations;
    }

    // implement UCT here
    TreeNode* getBestUCTChildren(TreeNode *parent) {
        if(parent->getChildren().size() == 0) return NULL;

        vector<TreeNode*> vChildren(parent->getChildren());
        shuffle(begin(vChildren), end(vChildren), rng);
        return vChildren.front();
    }

    TreeNode* getMostVisitedChild(vector<TreeNode*> children) {
        int maxVisits = -1;
        TreeNode* maxChild = NULL;

        for(int i = 0; i < children.size(); ++i) {
            TreeNode* child = children[i];
            if(child->getNumVisits() > maxVisits) {
                maxVisits = child->getNumVisits();
                maxChild = child;
            }
        }

        return maxChild;
    }

    int runMCTS(board *state) {
//cout << endl << "------------MCTS START---------------" << endl << endl;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1);

        TreeNode root(state);
        TreeNode* bestNode = NULL;
//cout << "root:" << endl;
//root.getState().showboard();

        while(true) {

            // SELECT. Start at root; dig down into Tree.
//cout << endl << "SELECT" << endl << endl;
            TreeNode* node = &root;
            while(!node->isTerminal() && node->isFullyExpanded()) {
                node = getBestUCTChildren(node);
            }
//cout << "selected node:" << endl;
//node->getState().showboard();

            // EXPAND by adding children (if not terminal or not fully expanded)
            if(!node->isFullyExpanded() && !node->isTerminal()) {
//cout << endl << "EXPAND" << endl << endl;
                node = node->expand();
//cout << "expanded node:" << endl;
//node->getState().showboard();
            }
            board state(node->getState());

            // SIMULATE (if not terminal)
//cout << endl << "SIMULATING---------------------------------------------------------------" << endl << endl;
            bool winner;
            if(!node->isTerminal()) {
                board *b = new board(state);
//b->showboard();
                while(1) {
                    if(b->isTerminal()) {
                        winner = !(b->whoseTurn());
                        break;
                    }
                    b->add(b->getLegalMoves().front());
//b->showboard();
                }
            }
//cout << "END---------------------------------------------------------------" << endl;
//cout << "simulate winner: " << (winner ? "BLACK" : "WHITE") << endl;

            // BACK PROPAGATION
//cout << endl << "PROPAGATE" << endl << endl;
            while(node) {
                node->update(winner == node->getState().whoseTurn());
                node = node->getParent();
            }

            // find most visited child
            // best_node = get_most_visited_child(&root_node);
            // get highest winrate
            vector<TreeNode*> children = root.getChildren();
            float maxWinrate = -1;
            for (int i = 0; i < children.size(); ++i) {
//cout << "winrate of " << i << " is " << children[i]->getWinrate() << endl;
                if (children[i]->getWinrate() > maxWinrate) {
                    bestNode = children[i];
                    maxWinrate = children[i]->getWinrate();
                }
            }
//cout << "max winrate: " << maxWinrate << " by:" << bestNode->getAction() << endl;
//bestNode->getState().showboard();

            // exit loop if current total run duration (since init) exceeds max_millis
            if(maxTime > 0 && getTime() > maxTime) {
//cout << "return due to time" << endl;
                break;
            }

            // exit loop if current iterations exceeds max_iterations
            if(maxIterations > 0 && iterations > maxIterations) {
//cout << "return due to it" << endl;
                break;
            }
            iterations++;
        }

        // return best node's action
        return bestNode->getAction();
    }
};

#endif //NOGO_MCTS_H
