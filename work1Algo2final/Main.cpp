#include <iostream>
#include <queue>
#include <fstream>
#include <math.h>
#include <thread>
#include <string.h>

#define v 1000                                             //create perminent variable "v" that save the number of vertecs (1000)                                     
#define numOfGraph 500                                      //create perminent variable "numOfGraph" that save the number of graphs (500)

using namespace std;

vector<vector<int>> build_random_graph(double );           //create new graph with 1000 vertecs and the probability is P
int connectivity(vector<vector<int>>);                     //check if the graph is connected
int Is_Isolted(vector<vector<int>>);                       //check if the graph including an isolted vertex
int diameter(vector<vector<int>>vecG);                     //return the maximum diameter of the graph
vector<int> BFS(int start, vector<vector<int>> vecG);      //BFS function
void printGraph(vector<vector<int>>);                      //print the neighbors of each vertex
void printFront(double p[]);                               //write the probability before the results
void printEnd(int totalResult);                            //write the results 

ofstream fileTotal;                                        //create global pointer to the file 
                                              
int main()
{
	fileTotal.open("FinalResults.csv");
	vector<vector<int>> graph;
	int countDiam = 0, TotalCountDiam = 0;
	int countIso = 0, TotalCountIso = 0;
	int countCon = 0, TotalCountCon = 0;

	double P1[10] = { 0.0015,0.00255,0.0038,0.0058,0.0065, /* 0.0069 */ 0.008,0.0085,0.012,0.014,0.016 };
	double P2[10] = { 0.09,0.095,0.1,0.11,0.115 /* 0.1175394 */ ,0.118,0.12,0.13,0.14,0.15 };
	double P3[10] = { 0.003,0.0035,0.004,0.0055,0.006, /* 0.0069 */ 0.008,0.0085,0.013,0.018,0.02 };
	
	
	//////////// Connectivity ////////////
	fileTotal << "Connectivity"<<endl;
	printFront(P1);
	for (int j = 0; j < 10; j++)
	{
		countCon = 0;
		for (int i = 0; i < numOfGraph; i++)
		{
			cout << "Connectivity number:" << j * numOfGraph + i + 1 << endl;
			graph = build_random_graph(P1[j]);
			if (connectivity(graph))
				countCon++;
		}
		fileTotal << countCon / (double)numOfGraph << ",";
		TotalCountCon += countCon;
	}
	printEnd(TotalCountCon);


	//////////// Diameter ////////////
	fileTotal << "Diameter" << endl;
	printFront(P2);
	for (int j = 0; j < 10; j++)
	{
		countDiam = 0;
#pragma omp parallel for
		for (int i = 0; i < numOfGraph; i++)
		{
			cout << "Diameter number:" << j * numOfGraph + i + 1 << endl;
			vector<vector<int>> graph = build_random_graph(P2[j]);
			if (diameter(graph) <= 2)
				countDiam++;
		}
		fileTotal << (double)countDiam / (double)numOfGraph << ",";
		TotalCountDiam += countDiam;
	}
	printEnd(TotalCountDiam);


	//////////// Isolated Vertix ////////////
	fileTotal << "Isolated Vertix" << endl;
	printFront(P3);
	for (int j = 0; j < 10; j++) {
		countIso = 0;
		for (int i = 0; i < numOfGraph; i++)
		{
			cout << "Isolated Vertix:" << j * numOfGraph + i + 1 << endl;
			graph = build_random_graph(P3[j]);
			countIso += Is_Isolted(graph);
		}
		fileTotal << countIso / (double)numOfGraph << ",";
		TotalCountIso += countIso;
	}
	printEnd(TotalCountIso);
}


//write the probability before the results to the file
void printFront(double p[])
{
	fileTotal  <<"Probability,";
	for (int j = 0; j < 10; j++)
		fileTotal << p[j] << ",";
	fileTotal << endl << "Results:,";
}


//write the results to the file
void printEnd(int totalResult)
{
	fileTotal << endl << endl << "Condition:True," << totalResult << endl;
	fileTotal << "Condition:False," << 10 * numOfGraph - totalResult << endl;
	fileTotal << "Total Probability:," << totalResult / (double)(10.0 * numOfGraph) << endl << endl << endl;
	for (int j = 0; j <= 10; j++)
		fileTotal << "-----------------,";
	fileTotal << endl << endl << endl;
}


//crete new graph with 1000 vertecs and the probability is P
vector<vector<int>> build_random_graph(double p)
{
	vector<vector<int>> vecG(v);
	for (int i = 0; i < v; i++)
	{
		for (int j = 0; j < i; j++)
			if (p >= ((double)rand() / RAND_MAX * 1.0 - 0.0)) {
				vecG[i].push_back(j);
				vecG[j].push_back(i);
			}
	}
	return vecG;
}


//print the neighbors of each vertex
void printGraph(vector<vector<int>> vecG)
{
	cout << endl;
	for (unsigned int i = 0; i < vecG.size(); i++)
	{
		cout << "V-" << i << ":    ";
		for (auto k : vecG[i])
			cout << k << ", ";
		cout << endl;
	}
	cout << endl;
}


//check if the graph is connected
int connectivity(vector<vector<int>> vecG)
{
	if (Is_Isolted(vecG))
		return 0;
	vector<int> d(vecG.size(), -1);
	d = BFS(1, vecG);
	for (auto x : d) {
		if (x == -1)
			return 0;
	}
	return 1;
}


//check if the graph including an isolted vertex
int Is_Isolted(vector<vector<int>> vecG)
{
	for (unsigned int i = 0; i < vecG.size(); i++)
		if (vecG[i].empty())
			return 1;
	return 0;
}


//BFS function
vector<int> BFS(int start, vector<vector<int>> vecG)
{
	int head;
	queue<int> q;
	vector<int> distance(vecG.size(), -1);
	distance[start] = 0;
	q.push(start);
	while (!q.empty()) {
		head = q.front();
		q.pop();
		for (auto i = vecG[head].begin(); i != vecG[head].end(); i++)
			if (distance[*i] == -1) {
				q.push(*i);
				distance[*i] = distance[head];
				distance[*i]++;
			}
	}
	
	return distance;
}


//return the maximum diameter of the graph
int diameter(vector<vector<int>> vecG)
{
	int maxDiam = 0;
	if (!connectivity(vecG))
		return -1;
	vector<int> distance(vecG.size());
	for (int i = 0; i < (signed)vecG.size(); i++)
	{
		distance = BFS(i, vecG);
		for (auto x : distance)
			if (x > maxDiam)
				maxDiam = x;
	}
	return maxDiam;
}