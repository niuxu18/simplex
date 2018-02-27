#include <iostream>
#include <fstream>
#include <tr1/unordered_map>
#include <vector>
#include <algorithm>
using namespace std;
using namespace std::tr1;
typedef unordered_map<int, int> int_int_map;

/**
    @param input input file stream
    @param edge_num number of edges
    @param capabilities 
    @param costs
    @return ..
*/
void analyzeEdge(ifstream &input, const int edge_num, unordered_map<int, int_int_map>& capabilities, unordered_map<int, int_int_map>& costs, unordered_map<int, int_int_map>& locations)
{
    char* line;
    for(int i = 0; i < edge_num; i++)
    {
        int start_node;
        input >> start_node;
        int end_node;
        input >> end_node;
        int capability;
        input >> capability;
        int cost;
        input >> cost;

        // update capabilities between start_node & end_node
        capabilities[start_node][end_node] = capability;
        // fetch cost map list of the given start node
        costs[start_node][end_node] = cost;
        // locations for the variable about (start_node, end_node)
        locations[start_node][end_node] = 2*i + 1; // from 1 
        
        // opisite edge
        capabilities[end_node][start_node] = capability;
        costs[end_node][start_node] = cost;
        // locations for the variable about (start_node, end_node)
        locations[end_node][start_node] = 2*i + 2;
    }
}

/**
    @param input input file stream
    @param consumer_number number of consumers
    @param demands 
    @return ..
*/
void analyzeConsumer(ifstream &input, const int consumer_number, unordered_map<int, int_int_map>& demands)
{
    char* line;
    for(int i = 0; i < consumer_number; i++)
    {
        // index for consumer
        int consumer_index;
        input >> consumer_index;
        // node which connect consumer
        int consumer_node;
        input >> consumer_node;
        int demand;
        input >> demand;

        // fetch demand map list of the given cosumer index
        demands[consumer_index][consumer_node] = demand;
    }
}

/**
    @param output output file stream
    @param key number
    @param map_instance unordered_map to output
    @return ..
*/
void outputUnorderedMap(ofstream &output, const int key_number, unordered_map<int, int_int_map>& map_instance)
{
    for(int i = 0; i < key_number; i++)
    {
        output << i << " : ";
        int_int_map now_map = map_instance[i];
        for (auto it = now_map.begin(); it != now_map.end(); ++it) // auto: dynamically judge the type
        {
            output << it->first << "\t" << it->second << "\t" ;
        }
        output << endl;
    }
}

/**
    @param ofstream output ofstream
    @param vector<int> vector to output
    @return ..
*/
void outputVector(ofstream & output, const vector<int>& vector_instance)
{
    int size = vector_instance.size();
    for(int i = 0; i < size; i++)
    {
        output << vector_instance[i] << " ";
    }
    output << "\n";
}

/** 
    @param vector<int> vector to initialize
    @value initialized value of elements in vector
    @return ..
*/
void initializeVector(vector<int>& vector_instance, int value)
{
    int size = vector_instance.size();
    for(int i = 0; i < size; i++)
    {
        vector_instance[i] = value;
    }
}

int main()
{
    /**
     **************************************** analyze input file to get neighbor info and consumer info
    */ 
    const int line_length = 100;
    char* line;
    ifstream input("/usr/info/code/cpp/LogMonitor/LogMonitor/simplex/case00.txt",ios::in | ios::binary);
    if(!input)
    {
        cout << "can not find input or output file" << endl;
        return 1;
    }   

    // analyze first line
    int node_number;
    input >> node_number;
    int edge_number;
    input >> edge_number;
    int consumer_number;
    input >> consumer_number;
    int server_cost;
    input >> server_cost;

    // initializetion
    unordered_map<int, int_int_map> capabilities(node_number);
    unordered_map<int, int_int_map> costs(node_number);
    unordered_map<int, int_int_map> locations(node_number);// location for variables in tableau
    unordered_map<int, int_int_map> demands(consumer_number);
   
     // ignore the first empty line
    input.getline(line, line_length);

    // analyze the edge info
    analyzeEdge(input, edge_number, capabilities, costs, locations);

    //ignore the second empty line
    input.getline(line, line_length);
    
    //analyze the consumer info
    analyzeConsumer(input, consumer_number, demands);

    //add consumers into graph 
    int consumer_node_index, consumer_node, consumer_index_delta = node_number;
    for(int i = 0; i < consumer_number; i++)
    {
        consumer_node_index = i + consumer_index_delta; // node index for consumers
        int_int_map now_map = demands[i];  
        for(auto it = now_map.begin(); it != now_map.end(); it++)
        {
            consumer_node = it->first; // node that connect to consumer
            locations[consumer_node][consumer_node_index] = 1 + edge_number*2; // column number for the variable of consumers
        }
    }

    input.close();
    // output.close();
    
    /**
    **************************************** transform into a linear programming problem
    */ 
    ofstream output("/usr/info/code/cpp/LogMonitor/LogMonitor/simplex/output.txt", ios::out | ios::binary);
    int variable_number = 2*edge_number + 1 + consumer_number;
    int server_number = 3;
    vector<int> constraint(variable_number);    
    vector<int> constraint_symmetry(variable_number);
    // constraints : consumer_number*2 +  edge_number*2 + node_number*2 - server_number
    // variables : consumer_number + edge_number*2 
    output << (node_number*2 - server_number + edge_number*2 + consumer_number*2)  << " " << variable_number << "\n";

    //////////////////////////////// objective(flow * cost + server * server_cost)
    // cost numbers 
    initializeVector(constraint, 0);
    int end_node; // i is the start_node
    for(int i = 0; i < node_number; i++)
    {   
        // from neighbors to compute input
        int_int_map now_map = costs[i];        
        for(auto it = now_map.begin(); it != now_map.end(); it++)
        {
            end_node = it->first; // end node
            constraint[locations[i][end_node]]  = costs[i][end_node]; // from i to end_node
            constraint[locations[end_node][i]]  = costs[end_node][i]; // from end_node to i
        }
    }
    // write constraint to file
    outputVector(output, constraint);

    /////////////////////////////// capbility constraints on edges (edge_number*2)
    for(int i = 0; i < node_number; i++)
    {
        int_int_map now_map = capabilities[i];        
        for(auto it = now_map.begin(); it != now_map.end(); it++)
        {
            end_node = it->first; // end node

            // compute constraints
            initializeVector(constraint, 0);

            constraint[0] = it->second; // capabilty limit
            constraint[locations[i][end_node]]  = 1; // on edge from i to end_node

            // write constraint to file
            outputVector(output, constraint);
        }
    }

    ////////////////////////////// network constraints on nodes (node_number*2 - server_number + consumer_number*2)
    vector<int> servers, non_servers;
    
    // consumer input = demand (consumer_number)
    for(int i = 0; i < consumer_number; i++)
    {
        consumer_node_index = i + consumer_index_delta; // node index for consumers
        int_int_map now_map = demands[i];        
        for(auto it = now_map.begin(); it != now_map.end(); it++)
        {
            consumer_node = it->first; // consumer_node
        }

        // compute constraints
        initializeVector(constraint, 0);
        initializeVector(constraint_symmetry, 0);

        constraint[0] = demands[i][consumer_node]; // demand limit
        constraint_symmetry[0] = -1 * demands[i][consumer_node];

        // only have one neighbor 
        constraint[locations[consumer_node][consumer_node_index]]  = 1; // input
        constraint_symmetry[locations[consumer_node][consumer_node_index]]  = -1; // input

        // write constraint to file
        outputVector(output, constraint);
        outputVector(output, constraint_symmetry);
    }

    // server input = 0
    servers.push_back(0);
    servers.push_back(3);
    servers.push_back(22);
    server_number = servers.size();
    int server_node;
    for(int i = 0; i < server_number; i++)
    {
        server_node = servers[i];
        // compute constraints
        initializeVector(constraint, 0);

        constraint[0] = 0; // demand limit

        // from neighbors to compute input
        int_int_map now_map = locations[server_node];        
        for(auto it = now_map.begin(); it != now_map.end(); it++)
        {
            end_node = it->first;// end node
            constraint[locations[end_node][server_node]]  = 1; // input
        }

        // write constraint to file
        outputVector(output, constraint);
    }

    // non-server input = output 
    for(int i = 0; i < node_number; i++) // i the non_server_node
    {   
        // ignore servers and consumers
        if(find(servers.begin(), servers.end(), i) != servers.end())
        {
            continue;
        }

        // compute constraints
        initializeVector(constraint, 0);
        initializeVector(constraint_symmetry, 0);

        // from neighbors to compute input
        int_int_map now_map = locations[i];        
        for(auto it = now_map.begin(); it != now_map.end(); it++)
        {
            end_node = it->first; // end node
            constraint[locations[end_node][i]]  = 1; // input
            constraint[locations[i][end_node]]  = -1; // output

            constraint_symmetry[locations[end_node][i]]  = -1; // input
            constraint_symmetry[locations[i][end_node]]  = 1; // output
        }

        // write constraint to file
        outputVector(output, constraint);
        outputVector(output, constraint_symmetry);
    }

    return 0;
}