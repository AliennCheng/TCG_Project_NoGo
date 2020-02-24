//
// Created by Alienn Cheng on 2019/12/10.
// Reference: https://github.com/memo/ofxMSAmcts/tree/master
//

#ifndef MCTS_H
#define MCTS_H

#define BETA 0.5
#include <ctime>
#include "board.h"
#include "TreeNode.h"
#include "RAVE.h"

class MCTS
{
private:
//float QStarMax;
    int iterations;
    unsigned int maxIterations;
    unsigned int maxTime;
    struct timespec ts1, ts2;
    double time_elapsed;
    default_random_engine rng;
    RAVE *rave;

public:

    MCTS() : iterations(0),
             maxIterations(1000000),
             time_elapsed(0)
    {
        maxTime = 5000;
        rng = default_random_engine {};
        rave = new RAVE();
//cout << "MCTS created" << endl;
    }

    ~MCTS() {
        delete rave;
    }

    void clear() {
        delete rave;
        rave = new RAVE();
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
    TreeNode* getBestChildren(TreeNode *parent) {
// cout << endl << "--------------start---------------" << endl;
        vector<TreeNode*> vChildren(parent->getChildren());
        if (vChildren.size() == 0) { cout << "empty children!!" << endl; exit(0); }
        
        float Qmax = -1, Q, Qtilde, Qstar;
        TreeNode *best = nullptr;
        for (int i = 0; i < vChildren.size(); ++i) {
            Q = vChildren[i]->getUCT();
            Qtilde = rave->getRaveWinrate(vChildren[i]->getAction());
            Qstar = Q + (Qtilde/2);
            if (Qstar > Qmax) {
                Qmax = Qstar;
                best = vChildren[i];
            }
// cout << "action: " << vChildren[i]->getAction() << " Q:" << Q << " Qtilde:" << Qtilde << endl;
        }
// cout << endl << "MCTS best child: " << best->getAction() << endl;
// cout << "----------------------------------" << endl;
//QStarMax = Qmax;
        return best;
    }

    // TreeNode* getMostVisitedChild(TreeNode *parent) {
    //     int maxVisits = -1;
    //     TreeNode* maxChild = NULL;
    //     vector<TreeNode*> children(parent->getChildren());

    //     for(int i = 0; i < children.size(); ++i) {
    //         TreeNode* child = children[i];
    //         if(child->getNumVisits() > maxVisits) {
    //             maxVisits = child->getNumVisits();
    //             maxChild = child;
    //         }
    //     }

    //     return maxChild;
    // }

    int runMCTS(board *state) {
// cout << endl << "------------MCTS START---------------" << endl << endl;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1);

        TreeNode root(*state);
        TreeNode* bestNode = nullptr;

        TreeNode tmp(*state);
        tmp.visiting();
        if(!tmp.isFullyExpanded() && !tmp.isTerminal())
            tmp.expand();
        vector<TreeNode*> children = tmp.getChildren();
        if (children.size() > 1) maxTime = 1000 * (children.size() - 1);
        else maxTime = 1;
        children.clear();
        children.shrink_to_fit();

        while(true) {
            ++iterations;
            // rave->addIteration();

            // SELECT. Start at root; dig down into Tree.
// cout << "SELECTING" << endl;
            TreeNode* node = &root;
            node->visiting();
            while(!node->isTerminal() && node->isFullyExpanded()) {
                TreeNode* nodeTmp = getBestChildren(node);
                if (!nodeTmp) break;
                else node = nodeTmp;
                node->visiting();
                rave->raveVisiting(node->getAction());
            }

            // EXPAND by adding children (if not terminal or not fully expanded)
// cout << "EXPANDING" << endl;
            if(!node->isFullyExpanded() && !node->isTerminal()) {
                node = node->expand();
                if (node == nullptr) continue;
                node->visiting();
                rave->raveVisiting(node->getAction());
            }
            board state = node->getState();

            // SIMULATE (if not terminal)
// cout << "SIMULATING" << endl;
            bool winner;
            int counter = 81;
            if(!node->isTerminal()) {
                board b = state;
                while(--counter > 0) {
                    if(b.isTerminal()) {
                        winner = !(b.whoseTurn());
                        // b->clear();
                        // b->deleteBoard();
                        // delete b;
                        // free(b);
                        break;
                    }
                    vector<int> legalMoves = b.getLegalMoves();
                    b.add(legalMoves.front());
                    legalMoves.clear();
                    legalMoves.shrink_to_fit();
                }
            }

            // BACK PROPAGATION
// cout << "PROPAGATING" << endl;
            if (node == nullptr) continue;
            while(node && node->getAction() >= 0) {
// cout << "win node: " << node->getAction() << endl;
                board b = node->getState();
                node->update(winner == b.whoseTurn(), rave);
                node = node->getParent();
            }

            // find most visited child
            // best_node = get_most_visited_child(&root_node);
            // get highest winrate
            // bestNode = getMostVisitedChild(&root);
// cout << "before bestNode" << endl;
            bestNode = getBestChildren(&root);

////////////////
// cout << "best: " << bestNode->getAction() << " UCT: " << bestNode-> getUCT() << " Qtilde: " << rave->getRaveWinrate(bestNode->getAction()) << endl;

            // exit loop if current total run duration (since init) exceeds max_millis
            if(maxTime > 0 && getTime() > maxTime) {
                break;
            }

            // exit loop if current iterations exceeds max_iterations
            if(maxIterations > 0 && iterations > maxIterations) {
                break;
            }
        }

        // return best node's action
        return bestNode->getAction();
    }
};

#endif //MCTS_H
