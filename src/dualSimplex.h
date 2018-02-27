#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include<malloc.h>

class dualSimplex
{
public:
    dualSimplex(){}
    ~dualSimplex()
    {
        for(int i = 0; i < num_row; i++)
        {
           
            delete[] matrix[i];
          
        }
        delete[] matrix;
    }

    void setMatrix(int row, int vars, const double** data);
    void readMatrix(int row,  int vars, std::ifstream & file);
    void outputMatrix(std::ofstream & file);
    void outputMatrix();

    //  execute solve
    double solveMinProblemWithDual(int loop, std::vector<double>& variableValue);
    std::vector<double> getVariableValue();

private:
    // add relax variables for less constraint
    void addRelaxVars();
    // find the row to pivot (most negative row)
    int findPivotRow();
    // find the column to pivot (ratio with least abs value)
    int findPivotCol(int row);
    // execute pivotion
    void pivot(int row, int col);
    // get optimization variables and value
    bool getOptimization();
    
    int num_variable;// variables
    int num_row;// row = constraints + objectives
    int num_col;// col = variables + relaxation + b
    double ** matrix; // matrix for data
    std::vector<double> variableValue; // vector for variables
};