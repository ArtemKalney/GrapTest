#include "Hypernet.h"
#include "globals.h"

void H::PrintHyperNet() {
    std::cout << "FN:" << std::endl;
    for (auto &row : this->_FN) {
        for (auto &cell : row) {
            cell.PrintBranche();
        }
    }
    std::cout << "F:" << std::endl;
    for (auto &row : this->_F) {
        for (auto &cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
}

/*возможно нужно придумать метод для получения списка путей соответсвующих данному ребру*/
bool H::EqualEdgeNodes(const std::vector<int>& F, const int& firstNode, const int& secondNode) {
    auto sameNodes = false, reversedNodes = false;
    try {
        sameNodes = F.front() == firstNode && F.back() == secondNode;
        reversedNodes = F.front() == secondNode && F.back() == firstNode;
    }
    catch (char *str) {
        std::cout << str << std::endl;
    }
    return sameNodes || reversedNodes;
}

bool H::IsSNconnected() {
    for (auto &row : this->_F) {
        if (H::EqualEdgeNodes(row, 0, 1)) {
            return true;
        }
    }

    return false;
}

int H::GetBranchSaturation (const Branche& branche) const {
    int count = 0;
    for (auto &row : this->_F) {
        auto prevCell = row.front();
        for (auto &cell : row) {
            if (Branche::EqualNodes(branche, cell, prevCell)) {
                count++;
            }
            prevCell = cell;
        }
    }

    return count;
}

void H::RemoveEmptyBranches() {
    auto FN = this->GetFN();
    for (int i = 0; i < FN.size(); i++) {
        for (int j = 0; j < FN[i].size(); j++) {
            auto saturation = this->GetBranchSaturation(FN[i][j]);
            if (i < j && saturation == 0) {
                ~FN[i][j];
                ~FN[j][i];
            }
        }
    }
}

void H::RemoveBranch(const Branche& branche) {
    auto FN = this->GetFN();
    int firstNode = branche.GetFirstNode(), secondNode = branche.GetSecondNode();
    ~FN[firstNode][secondNode];
    ~FN[secondNode][firstNode];

    /*преобразования F при удалении ветви*/
    for (int i = 0; i < _F.size(); i++) {
        auto prevCell = _F[i].front();
        for (int j = 0; j < _F.size(); j++) {
            if (Branche::EqualNodes(branche, _F[i][j], prevCell)) {
                _F.erase(_F.begin() + i);
            }
            prevCell = _F[i][j];
        }
    }
    /*возможно неудачная мысль*/
    this->RemoveEmptyBranches();
}

void H::RemoveNode(const int& node) {
    auto FN = this->GetFN();
    FN.erase(FN.begin() + node);
    for (int i = 0; i < FN.size(); i++) {
        FN[i].erase(FN[i].begin() + node);
    }
    /*возможно неудачная мысль*/
    this->RemoveEmptyBranches();
}

void H::MakeReliableBranch(const Branche& branche) {
    auto FN = this->GetFN();
    int firstNode = branche.GetFirstNode(), secondNode = branche.GetSecondNode();
    FN[firstNode][secondNode].SetIsReliable(true);
}

bool H::hasReliablePath() {
    auto FN = this->GetFN();
    for (auto &row : this->_F) {
        if (H::EqualEdgeNodes(row, 0, 1)) {
            auto isReliablePath = true;
            auto prevCell = row.front();
            for (auto &cell : row) {
                if (!FN[cell][prevCell].GetIsReliable()) {
                    isReliablePath = false;
                    break;
                }
                prevCell = cell;
            }
            if (isReliablePath) {
                return true;
            }
        }
    }

    return false;
}