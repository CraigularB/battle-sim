/*
    Copyright (C) 2012-2014 by Michael Kristofik <kristo605@gmail.com>
    Part of the battle-sim project.
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    or at your option any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY.
 
    See the COPYING.txt file for more details.
*/
#include "Pathfinder.h"
#include "algo.h"
#include <algorithm>
#include <cassert>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

struct AstarNode
{
    int prev;
    int costSoFar;
    int estTotalCost;
    bool visited;
};

typedef std::shared_ptr<AstarNode> AstarNodePtr;

struct BfsNode
{
    int id;
    int costSoFar;
};

namespace {
    AstarNodePtr make_astar_node(int prev, int costSoFar, int estTotalCost)
    {
        bool visited = false;
        return std::make_shared<AstarNode>(AstarNode{prev, costSoFar,
                                                     estTotalCost, visited});
    }
}

Pathfinder::Pathfinder()
    : neighbors_{[] (int) { return std::vector<int>(); }},
    goal_{[] (int) { return false; }},
    stepCost_{[] (int, int) { return 1; }},
    estimate_{[] (int) { return 0; }}
{
}

void Pathfinder::setNeighbors(std::function<std::vector<int> (int)> func)
{
    neighbors_ = func;
}

void Pathfinder::setGoal(int targetNode)
{
    goal_ = [=] (int node) { return node == targetNode; };
}

void Pathfinder::setGoal(std::function<bool (int)> func)
{
    goal_ = func;
}

void Pathfinder::setStepCost(std::function<int (int, int)> func)
{
    stepCost_ = func;
}

void Pathfinder::setEstimate(std::function<int (int)> func)
{
    estimate_ = func;
}

std::vector<int> Pathfinder::getPathFrom(int start) const
{
    if (goal_(start)) return {start};

    // Record shortest path costs for every node we examine.
    std::unordered_map<int, AstarNodePtr> nodes;
    // Maintain a heap of nodes to consider.
    std::vector<int> open;
    int goalLoc = -1;
    AstarNodePtr goalNode;

    // The heap functions confusingly use operator< to build a heap with the
    // *largest* element on top.  We want to get the node with the *least* cost,
    // so we have to order nodes in the opposite way.
    auto orderByCost = [&] (int lhs, int rhs)
    {
        return nodes[lhs]->estTotalCost > nodes[rhs]->estTotalCost;
    };

    nodes.emplace(start, make_astar_node(-1, 0, 0));
    open.push_back(start);

    // A* algorithm.  Decays to Dijkstra's if estimate function is always 0.
    while (!open.empty()) {
        auto loc = open.front();
        pop_heap(std::begin(open), std::end(open), orderByCost);
        open.pop_back();
        if (goal_(loc)) {
            goalLoc = loc;
            goalNode = nodes[loc];
            break;
        }

        auto &curNode = nodes[loc];
        curNode->visited = true;
        for (auto n : neighbors_(loc)) {
            auto nIter = nodes.find(n);
            auto step = stepCost_(loc, n);

            if (nIter != nodes.end()) {
                auto &nNode = nIter->second;
                if (nNode->visited) {
                    continue;
                }

                // Are we on a shorter path to the neighbor node than what
                // we've already seen?  If so, update the neighbor's node data.
                if (curNode->costSoFar + step < nNode->costSoFar) {
                    nNode->prev = loc;
                    nNode->costSoFar = curNode->costSoFar + step;
                    nNode->estTotalCost = nNode->costSoFar + estimate_(n);
                    make_heap(std::begin(open), std::end(open), orderByCost);
                }
            }
            else {
                // We haven't seen this node before.  Add it to the open list.
                nodes.emplace(n, make_astar_node(loc, curNode->costSoFar + step, 
                    curNode->costSoFar + step + estimate_(n)));
                open.push_back(n);
                push_heap(std::begin(open), std::end(open), orderByCost);
            }
        }
    }

    if (!goalNode) {
        return {};
    }

    // Build the path from the chain of nodes leading to the goal.
    std::vector<int> path = {goalLoc};
    auto n = goalNode;
    while (n->prev != -1) {
        path.push_back(n->prev);
        n = nodes[n->prev];
    }
    reverse(std::begin(path), std::end(path));
    assert(contains(path, start));
    return path;
}

std::vector<int> Pathfinder::getReachableNodes(int start, int maxDist) const
{
    std::queue<BfsNode> nodeQ;
    std::vector<int> reachable;

    nodeQ.emplace(BfsNode{start, 0});

    while (!nodeQ.empty()) {
        const auto &node = nodeQ.front();
        if (binary_search(std::begin(reachable), std::end(reachable), node.id)) {
            nodeQ.pop();
            continue;
        }

        reachable.push_back(node.id);
        sort(std::begin(reachable), std::end(reachable));

        for (auto nbr : neighbors_(node.id)) {
            auto cost = node.costSoFar + stepCost_(node.id, nbr);
            if (cost <= maxDist) {
                nodeQ.emplace(BfsNode{nbr, cost});
            }
        }

        nodeQ.pop();
    }

    // AI players use this function when computing possible moves.  All else
    // equal, it looks better if the AI prefers to stand still and attack an
    // adjacent unit rather than move and attack.  Thus, we ensure the unit's
    // current hex is the first in the list of possible moves.
    auto iter = lower_bound(std::begin(reachable), std::end(reachable), start);
    rotate(std::begin(reachable), iter, std::end(reachable));

    return reachable;
}
