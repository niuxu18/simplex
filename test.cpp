#include<string>  
#include<iostream>  
#include<tr1/unordered_map>  
using namespace std;
using namespace std::tr1;  

struct edge
{
    edge(int capability, int cost)
    {
        capability = capability;
        cost = cost;
    }

    int capability;
    int cost; 
};


int main()  
{  

    typedef unordered_map <int, int> int_int_map;
    unordered_map<int, int_int_map> m;  
    int_int_map a;
    a[0] = 20;
    a[1] = 30;
    m[0] = a;
      
    int n = m.size();
    for(int i = 0; i < n; i++)  
    {  
        int size = m[i].size();
        for(int j = 0; j < size; j++)
        {
            cout << m[i][j] << "\t";
        }
        cout << endl;
    }  
      
    return 0;  
}