#include "Stdafx.h"
#include "Funcs.h"
#include "Globals.h"

void GenCombinations(const H& H, const std::vector<Branche>& branchList, Branche& sum, std::vector<int>& brancheMask,
                     int& curPos){
    if (curPos == branchList.size()) {
        auto hypernet = H;
        for (int i = 0; i < brancheMask.size(); i++) {
            Branche branche = branchList[i];
            if (brancheMask[i] == 0) {
                hypernet.RemoveBranch(branche.GetFirstNode(), branche.GetSecondNode());
            } else if (brancheMask[i] == 1) {
                hypernet.MakeReliableBranch(branche.GetFirstNode(), branche.GetSecondNode());
            }
        }

        if (hypernet.IsSNconnected()) {
            ReliableHypernetsCount++;
            Branche result = Branche::GetBranch(0);
            for (auto &item : brancheMask) {
                if (item == 1) {
                    result.SetPower(result.GetPower() + 1);
                } else if (item == 0) {
                    result.SetPower(result.GetPower() + 1);
                    result.GetC().insert(result.GetC().begin(), 0);
                    result.GetC().pop_back();
                }
            }

            if (result.GetPower() != branchList.size()) {
                throw "GenCombinations: strange result power";
            }
            sum = sum + result;
        } else {
            UnconnectedHypernentsCount++;
        }
    }
    else{
        brancheMask[curPos] = 0;
        int increasedPos = curPos + 1;
        GenCombinations(H, branchList, sum, brancheMask, increasedPos);
        brancheMask[curPos] = 1;
        GenCombinations(H, branchList, sum, brancheMask, increasedPos);
    }
}