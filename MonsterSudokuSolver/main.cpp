#include <iostream>
#include "sudokureader.h"
#include "sudokusolver.h"
#include <cmath>
using namespace std;
/* Comments are testing lines */
int main(int argc, char *argv[]) {
//    char* inputFiles[61] = {
//        "input.txt",
//        "PE1.txt",
//        "PE2.txt",
//        "PE3.txt",
//        "PE4.txt",
//        "PE5.txt",
//        "PE6.txt",
//        "PE7.txt",
//        "PE8.txt",
//        "PE9.txt",
//        "PE10.txt",
//        "PE11.txt",
//        "PE12.txt",
//        "PE13.txt",
//        "PE14.txt",
//        "PE15.txt",
//        "PE16.txt",
//        "PE17.txt",
//        "PE18.txt",
//        "PE19.txt",
//        "PE20.txt",
//        "PE21.txt",
//        "PE22.txt",
//        "PE23.txt",
//        "PE24.txt",
//        "PE25.txt",
//        "PE26.txt",
//        "PE27.txt",
//        "PE28.txt",
//        "PE29.txt",
//        "PE30.txt",
//        "PE31.txt",
//        "PE32.txt",
//        "PE33.txt",
//        "PE34.txt",
//        "PE35.txt",
//        "PE36.txt",
//        "PE37.txt",
//        "PE38.txt",
//        "PE39.txt",
//        "PE40.txt",
//        "PE41.txt",
//        "PE42.txt",
//        "PE43.txt",
//        "PE44.txt",
//        "PE45.txt",
//        "PE46.txt",
//        "PE47.txt",
//        "PE48.txt",
//        "PE49.txt",
//        "PE50.txt",
//        "PH1.txt",
//        "PH2.txt",
//        "PH3.txt",
//        "PH4.txt",
//        "PH5.txt",
//        "PM1.txt",
//        "PM2.txt",
//        "PM3.txt",
//        "PM4.txt",
//        "PM5.txt"
//    };
    long timeout = 300;
    char* inputFileName = "input.txt";
    char* outputFileName = "output.txt";
    vector<char*> options;
    if (argc >= 4) {
        inputFileName = argv[1];
        outputFileName = argv[2];
        timeout = atol(argv[3]);
        int opNum = argc - 4;
        for (std::size_t i = 0; (std::size_t)i < (std::size_t)opNum; ++i) {
            options[i] = argv[4 + i];
        }
    }
    SudokuSolver S;
    for (std::size_t i = 0; i < (std::size_t)options.size(); ++i) {
        if (options[i] == "FC"){
            S.setFlag(HeuristicFlag::kFC,true);
        }
        else if (options[i] == "MRV"){
            S.setFlag(HeuristicFlag::kMRV,true);
        }
        else if (options[i] == "DH") {
            S.setFlag(HeuristicFlag::kDH,true);
        }
        else if (options[i] == "LCV") {
            S.setFlag(HeuristicFlag::kLCV,true);
        }
        else if (options[i] == "MAC") {
            S.setFlag(HeuristicFlag::kMAC,true);
        }
        else if (options[i] == "ACP") {
            S.setFlag(HeuristicFlag::kACP,true);
        }
        else {

        }
    }
//    S.setFlag(HeuristicFlag::kMRV,true);
//    S.setFlag(HeuristicFlag::kDH,true);
    S.setFlag(HeuristicFlag::kACP,true);
//    S.setFlag(HeuristicFlag::kLCV,true);
//    S.setFlag(HeuristicFlag::kMAC,true);
//    S.setFlag(HeuristicFlag::kFC,true);
    int m=17,n=9,p=3,q=3;
    char* test[10]= {
        "0O.txt",
        "1O.txt",
        "2O.txt",
        "3O.txt",
        "4O.txt",
        "5O.txt",
        "6O.txt",
        "7O.txt",
        "8O.txt",
        "9O.txt"
    };

//    cout << "Generate?" << endl;
    char x = 'x';
//    int stop = 10;
//    char* inputs[5] = {
//        "PH1.txt",
//        "PH2.txt",
//        "PH3.txt",
//        "PH4.txt",
//        "PH5.txt"
//    };
    int stop = 10;
//    cin >> x;
//    cin.clear();
//    int puzzles = 27;
//    int puzzles = 1;
    int puzzles = 12;
//    int puzzles = 10;
    int trials = 14;
//    int ma[puzzles] = {m,34,53,60,76,94,104,135,171,184,212,241,288,7,14,21,29,30,32,34,36,37,39,43,50,57,64};
//    int na[puzzles] = {n,12,15,16,18,20,21,24,27,28,30,32,35,12,12,12,12,12,12,12,12,12,12,12,12,12,12};
//    int pa[puzzles] = {p,3,3,4,3,4,3,4,3,4,5,4,5,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
//    int qa[puzzles] = {q,4,5,4,6,5,7,6,9,7,6,8,7,4,4,4,4,4,4,4,4,4,4,4,4,4,4};
    int s3m[12] = {36,56,63,80,99,109,142,180,194,222,253,303};
    int s3n[12] = {12,15,16,18,20,21,24,27,28,30,32,35};
    int s3p[12] = {3,3,4,3,4,3,4,3,4,5,4,5};
    int s3q[12] = {4,5,4,6,5,7,6,9,7,6,8,7};
    int s3[14] = {4,8,12,16,17,18,19,20,21,22,24,28,32,36};
    int s4m[14] = {11,22,33,45,47,50,53,56,58,61,67,78,89,100};
    int s5m[14] = {4,8,12,16,17,18,19,20,21,22,24,28,32,36};
    double sumtime = 0, sumnode = 0, sumstd = 0;
    int times[10];
    int timeouts = 0;
    for(int j = 8; j < trials; ++j) {
        cout << "----------Test=" << j << "----------" << endl;
//        if (x=='y') {
//                m=s3[j],n=9,p=3,q=3;
//                m=s3m[j],n=s3n[j],p=s3p[j],q=s3q[j];
        m=s4m[j],n=15,p=5,q=3;
                SudokuGenerator G(m,n,p,q);
                for (int i = 0; i < 10; ++i) {
                    char** sudoku = G.generateSudoku(timeout);
                    SudokuReader R("",test[i]);
                    R.saveSudoku(m,n,p,q,sudoku,'0');
                }
                cout << "-----generated m=" << m << ",n=" << n << ",p=" << p << ",q=" << q <<endl;

//        }
//    cout << ma[j] << ", " << na[j] << ", " << pa[j] << ", " << qa[j] << endl;
        for (int i = 0; i < stop; ++i) {
            SudokuReader R(test[i],"");
//            SudokuReader R(inputs[i],"");
//            SudokuReader R(inputFileName,outputFileName);
            char** sudoku = R.loadSudoku(n,p,q,timeout);
            cout << "-----Puzzle=" << i << "-----" <<  endl;
    //            SudokuReader R(inputFiles[i],"");
    //        SudokuReader R(input,"");
    //            char** sudoku = R.loadSudoku(n,p,q,timeout);
                char e = '0';
//                cout << inputs[i] << endl;
                SudokuPuzzle P (sudoku,m,n,p,q);
//                cout << "Initial Puzzle~\n";
//                P.display ();
                LogBundle B = S.solve(P,timeout,e);
                Logger L(B);
                L.logInfo(outputFileName);
                sumnode += L.getBundle()._nodes;
                sumtime += L.getBundle()._solutionTime;
                times[i] = L.getBundle()._solutionTime;
                times[i] = L.getBundle()._solutionTime;
                if(L.getBundle()._solutionTime >= timeout) {
                    ++timeouts;
                }
//                else {
//                    sumnode += L.getBundle()._nodes;
//                    sumtime += L.getBundle()._solutionTime;

//                }
//                cout << "Finished Puzzle~\n";
//                P.display ();
                cout << "-----Output Info~\n";
                L.printInfo();
                cout << endl;
        }
        cout << "-----Calculating..." << endl;
        double avgnode = sumnode / (double) stop;
        double avgtime = sumtime / (double) stop;
        for (int z = 0; z < stop; ++z) {
            sumstd += pow((times[z] - avgtime),2);
//            if (times[z] < timeout){
//                sumstd += pow((times[z] - avgtime),2);
//            }
        }
        double stdtime = sqrt(sumstd / (double) stop);
        double solvable = ((stop - timeouts) / (double) stop) * 100;
        sumnode = 0; sumtime = 0; sumstd = 0; timeouts = 0;
        cout << "-----Calculation Info~\n";
        cout << "Average Nodes=" << avgnode << endl
             << "Average Times=" << avgtime << endl
             << "Standard Deviation Time=" << stdtime << endl
             << "% Solvable=" << solvable << "%" << endl;
        cin.get();
    }


    return 0;
}
