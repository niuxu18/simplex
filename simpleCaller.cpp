#include "dualSimplex.h"
using namespace std;

int main()
{
    ifstream matrixFile("/usr/info/code/cpp/LogMonitor/LogMonitor/simplex/matrix.txt", ios::in|ios::binary);   
    if(!matrixFile.is_open())
        cout << "error open file" << endl;

    vector<double> variables;
    dualSimplex mySimplex;
    // row = constraint + 1 (objective)
    mySimplex.readMatrix(19, 9, matrixFile);
    // mySimplex.outputMatrix();

    double optimized = mySimplex.solveMinProblemWithDual(2000, variables);
    cout << "optimization value is " << optimized << endl;
    cout << "variables is ";
    for(int i = 0; i < variables.size(); i++)
    {
        cout << variables[i] << "\t";
    }
    cout << endl;

    return 0;
}