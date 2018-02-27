#include "dualSimplex.h"
using namespace std;

void dualSimplex::setMatrix(int row, int vars, const double** data)
{
    num_row = row;
    num_variable = vars;

    num_col = 1 + num_variable + num_row - 1; // b + variable + relaxation
    cout << "reading from data matrix: " << row << ", col:" << num_col << endl;

    matrix = (double**)malloc(num_col*num_row*sizeof(double));
    for(int i = 0 ; i < num_row; i++)
    {
        for(int j = 0; j < num_col; j++)
        {
            matrix[i][j] = data[i][j];
        }
    }

    // add relexation
    addRelaxVars();
}

void dualSimplex::readMatrix(int row, int vars, std::ifstream & file)
{
    num_row = row;
    num_variable = vars;

    num_col = 1 + num_variable + num_row - 1; // b + variable + relaxation
    cout << "reading from file row: " << row << ", col:" << num_col << endl;
    
    matrix = new double*[num_row];
    //matrix = (double**)malloc(num_col*num_row*sizeof(double));
    for(int i = 0 ; i < num_row; i++)
    {
        // read b and variables
        matrix[i] = new double[num_col];
        for(int j = 0; j < num_variable + 1; j++)
        {
            file >> matrix[i][j];
        }
    }

    // add relexation
    addRelaxVars();
}

void dualSimplex::outputMatrix(std::ofstream & file)
{
    for(int i = 0 ; i < num_row; i++)
    {
        // read b and variables
        for(int j = 0; j < num_col; j++)
        {
            file << matrix[i][j] << "\t";
        }
        file << endl;
    }
    
}

void dualSimplex::outputMatrix()
{
    for(int i = 0 ; i < num_row; i++)
    {
        // read b and variables
        for(int j = 0; j < num_col; j++)
        {
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }
}

vector<double> dualSimplex::getVariableValue()
{
    return variableValue;
}

// add relax variables for less constraint
void dualSimplex::addRelaxVars()
{
    // num_relaxation + num_variable + 1 = num_col (num_row = num_relaxation + 1)
    // from the second row to last row: add relaxation (the first row is objective, all constraints is less)
    for(int i = 1; i < num_row; i++)
    {
        // from the column with index num_variable + 1, to num_col: add relaxation (the first colunm is b)
        for(int j = 1; j < num_row; j++)
        {
            matrix[i][num_variable + j] = (i==j);
        }
    }
}

// find the row to pivot (most negative row)
int dualSimplex::findPivotRow()
{
    // find the most negative one in b
    int lowest = 0;
    int pivot_row = -1;
    for(int i = 1; i < num_row; i++)
    {
        if(matrix[i][0] < lowest)
        {
            lowest = matrix[i][0];
            pivot_row = i;
        }
    }
    
    // promption
    if(pivot_row == -1)
    {
        cout << "can not find pivot row" << endl;
    }
    else
    {
        cout << "pivot row is " << pivot_row << " and its b value is " << lowest << endl;
    }

    return pivot_row;
}

// find the column to pivot (ratio with least abs value)
int dualSimplex::findPivotCol(int row)
{
    double highest = 0;
    int pivot_col = -1;
    double now_ratio = 0;
    // the first col is b
    for(int j = 1; j < num_col; j++)
    {
        // try to make b to be positive, so choose the negative col
        if(matrix[row][j] >=0 || matrix[0][j] == 0)
            continue;

        now_ratio = matrix[0][j] / matrix[row][j];
        // choose the negative col with min abs ratio, max negative
        if(now_ratio > highest || highest == 0)
        {
            highest = now_ratio;
            pivot_col = j;
        }
    }

    // promption
    if(pivot_col == -1)
    {
        cout << "can not find pivot col for row " << row << endl;
    }
    else
    {
        cout << "pivot col is " << pivot_col << " and its ratio value is " << highest << endl;
    }

    return pivot_col;
}

// execute pivotion
void dualSimplex::pivot(int row, int col)
{
    double pivot = matrix[row][col];

    // cast the cofficiency of pivot into 1
    for(int j = 0;j < num_col; j++)
        matrix[row][j] /= pivot;

    // cast cofficiency of pivot of the remain row into 0
    double multiplier;
    for(int i = 0; i < num_row; i++) { 

        if(i == row)
            continue;

        // update each row 
        multiplier = matrix[i][col];
        for(int j = 0; j < num_col; j++) 
        { 
            matrix[i][j] -= multiplier * matrix[row][j];
        }
    }
}

// get optimization variables and value
bool dualSimplex::getOptimization()
{
    int nowRow;
    bool isBasicVariable;
    // from colunm 1 to column num_variable is variable column 
    for(int j = 1; j < num_variable + 1; j++)
    {        
        // variables must among the basic variable 
        if(matrix[0][j] != 0)
        {
            variableValue.push_back(0);
            // return false;
        }
        
        // check is the basic variable
        nowRow = -1;
        isBasicVariable = true;
        for(int i = 1; i < num_row; i++)
        {
            if(matrix[i][j] == 0)
                continue;

            if(matrix[i][j] == 1)
            {
                if(nowRow == -1)
                {
                    nowRow = i;                    
                }
                else
                {
                    isBasicVariable = false;
                }
            }
        }

        // add to variable list
        if(!isBasicVariable)
        {
            variableValue.push_back(0);
            // return false;
        }
        else
        {
            variableValue.push_back(matrix[nowRow][0]);
        }

    }

    return true;
}

//  execute solve (after readMatrix or setMatrix)
double dualSimplex::solveMinProblemWithDual(int loop, vector<double>& variableValue)
{
    // iteration at most loop times
    int pivotRow, pivotCol;
    for(int i = 0; i < loop; i++)
    {
        // find pivot row
        pivotRow = findPivotRow();
        if(pivotRow == -1)
            break;

        pivotCol = findPivotCol(pivotRow);
        if(pivotCol == -1)
            break;
        
        // do pivot
        pivot(pivotRow, pivotCol);

        // outputMatrix();
    }

    // get optimization
    if(!getOptimization())
        cout << "have not find optimized value within loop " << loop << endl;
        
    // assign the vector any way    
    variableValue.assign(this->variableValue.begin(), this->variableValue.end());
    return matrix[0][0];
}