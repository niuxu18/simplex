#include <fstream>
#include <iostream>
using namespace std;
const int M = 700;
const int N = 700;

int data[M][N];

int main()
{
    ifstream input("/usr/info/code/cpp/LogMonitor/LogMonitor/simplex/test.txt", ios::in | ios::binary);
    ofstream output("/usr/info/code/cpp/LogMonitor/LogMonitor/simplex/dual.txt", ios::out | ios::binary);
    if(!input.is_open() || !output.is_open())
    {
        cout << "check your inpur\n";
    }
    int m_less, m_more, m, n;
    input >> m_less;
    input >> m_more; 
    input >> n;
    m = m_less + m_more;
    output << n << "\t" << 0 << "\t" << m << endl;
    int temp;
    // first row 
    for(int j = 0; j < n; j++)
    {
        input >> data[0][j];
    }
    // transform less to more equality
    for(int i = 1; i < m_less; i++)
    {
        // read each row
        for(int j = 0; j < n; j++)
        {
            input >> temp;
            data[i][j] = -1 * temp;
        }
    }
    for(int i = m_less; i < m; i++)
    {
        // read each row
        for(int j = 0; j < n; j++)
        {
            input >> data[i][j];
        }
    }
    
    // objective row with minus 
    for(int j = 0; j < m; j++)
    {
        output << -1*data[j][0] << "\t";
    }
    output << endl;
    // constraint
    for(int i = 1; i < n; i++)
    {
        // write each row with corresponding column of data
        for(int j = 0; j < m; j++)
        {
            output << data[j][i] << "\t";
        }
        output << endl;
    }

}