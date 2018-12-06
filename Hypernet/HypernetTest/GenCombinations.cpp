#include "Stdafx.h"
#include "Funcs.h"
#include "Globals.h"

void GenCombinations(const H& H, const std::vector<Branch>& branchList, Branch& sum, std::vector<bool>& brancheMask,
                     int& curPos){
    if (curPos == branchList.size()) {
        auto hypernet = H;
        for (int i = 0; i < brancheMask.size(); i++) {
            Branch branche = branchList[i];
            if (!brancheMask[i]) {
                hypernet.RemoveBranch(branche);
            } else {
                hypernet.MakeReliableBranch(branche);
            }
        }

        if (hypernet.IsSNconnected()) {
            ReliableHypernetsCount++;
            Branch result = Branch::GetBranch(0);
            for (bool item : brancheMask) {
                if (item) {
                    result.SetPower(result.GetPower() + 1);
                } else {
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
            UnconnectedHypernetsCount++;
        }
    }
    else{
        brancheMask[curPos] = false;
        int increasedPos = curPos + 1;
        GenCombinations(H, branchList, sum, brancheMask, increasedPos);
        brancheMask[curPos] = true;
        GenCombinations(H, branchList, sum, brancheMask, increasedPos);
    }
}