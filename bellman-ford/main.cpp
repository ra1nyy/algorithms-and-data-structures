#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include <string>
using namespace std;
double INF = numeric_limits<double>::infinity();

class WeightedGraph
{
private:
    struct Edge
    {
        int to_;
        double weight_;
        Edge(int to = 0, double weight = 0.) :
        to_(to),
        weight_(weight)
        {}
    };
    vector<list<Edge>> adjacencyLists_; // списки смежности с весами
    size_t size_; // размер ографа // рекурсивный вывод потомков корневой вершины дерева
    void printChild(const vector<list<int>>& tree, int index, int deep); // релаксация ребра
    void update(int from, Edge& edge, bool& isChanges, vector<double>& dist, vector<int>& paths); // провека на возможность релаксации реба
    bool isUpdateChangeDist(int from, Edge& edge, vector<double>& dist); // получение списка вершин, у которых есть ребра
    list<int> getVerticesWithEdges(); public:
    int to_;
    double weight_;
public:

    WeightedGraph(size_t size, istream& in);
    // алгоритм заполняет массив dist весами катчайших путей, из вершины V
    // также, восстанавливает кратчайшие пути, записывая их в paths
    bool BellmanFord(vector<double>& dist, int v, vector<int>& paths);
    
    size_t getSize() const;
    
    // вывод весов катчайших путей из вершины V
    void printShortestPathsWeights(vector<double>& dist, int v);
    
    // вывод дерева кратчайших путей
    void printShortestPathTree(vector<int>& paths, int vertex);
    
    // true -  если arr - массив весов кратчайших путей, из вершины V
    // false - если arr таковым не является
    bool isArrayOfShortestPathsWeights(vector <double> arr, int v);
};


WeightedGraph::WeightedGraph(size_t size, istream& in) :
    adjacencyLists_(size),
    size_(size)
{
    int from = 0;
    int to = 0;
    double weight = 0.;
    int i = 0;
    while (i != size) // заполнение взвешенного орграфа
    {
        in >> from >> to;
        if (to == -1)
        {
            i++;
            continue;
        }
        in >> weight;
        adjacencyLists_[i].push_back(Edge(to, weight)); // O(1) in >> to;
        while (to != -1)
        {
            in >> weight;
            adjacencyLists_[from].push_back(Edge(to, weight)); // O(1) in >> to;
            if (to == -1)
            {
                break;
                
            }
        }
        ++i;
        
    }
}


void WeightedGraph::printChild(const vector<list<int>>& tree, int index, int deep)
{
  for (auto child : tree[index])
  {
    for (int i = 0; i < deep; ++i)
    {
        cout << " ";
        
    }
    cout << child << endl;
    printChild(tree, child, deep + 1);
  }
}


void WeightedGraph::printShortestPathTree(vector<int>& paths, int vertex)
{
    cout << "The shortest-path tree:" << endl;
    vector<list<int>> tree(paths.size());
    
    for (int i = 0; i < paths.size(); ++i)
    {
        if (paths[i] != -1)
        {
            tree[paths[i]].push_back(i); // O(1)
        }
    }
    cout << vertex << endl;
    printChild(tree, vertex, 1);
}


void WeightedGraph::update(int from, Edge& edge, bool& isChanges, vector<double>& dist, vector<int>& paths)
{
  if (dist[from] < INF)
  {
    if (dist[edge.to_] > dist[from] + edge.weight_)
    {
        dist[edge.to_] = max(-INF, dist[from] + edge.weight_); // O(1)
        paths[edge.to_] = from; // O(1)
        isChanges = true; // O(1)
    }
  }
}


bool WeightedGraph::isUpdateChangeDist(int from, Edge& edge, vector<double>& dist)
{
    if (dist[edge.to_] > dist[from] + edge.weight_)
    {
        return true; // если возможна релаксация
    }
    return false;
}


list<int> WeightedGraph::getVerticesWithEdges()
{
    list<int> edgesIndexes;
    for (int i = 0; i < size_; i++) // O(V)
    {
        if (!adjacencyLists_[i].empty()) // O(1)
        {
          edgesIndexes.push_back(i); // O(1)
        }
    }
  return edgesIndexes;
}


bool WeightedGraph::BellmanFord(vector<double>& dist, int v, vector<int>& paths)
{
    // выполним предобработку, для того чтобы можно было обойти все ребра за O(E)
    list<int> edgesIndexes = getVerticesWithEdges(); // O(V)
    dist[v] = 0;
    bool isSomethingChanged = false;
    
    for (int i = 0; i < size_ - 1; ++i) // O (V*E)
    {
        isSomethingChanged = false;
        for (auto from : edgesIndexes) // проходимся только по ребрам - O(E)
        {
            for (auto& to : adjacencyLists_[from])
            {
                update(from, to, isSomethingChanged, dist, paths); // O(1)
            }
        }
        if (!isSomethingChanged) break; // если изменений не произошло, заканчиваем поиск
    }
    
    if (dist[v] != 0)
    {
        return false;
    }
    
    for (auto& from : edgesIndexes) // проверяем на наличие цикла отрицательного веса - O(E)
    {
        for (auto to : adjacencyLists_[from])
        {
            if (isUpdateChangeDist(from, to, dist))
            {
                return false;
            }
        }
    }
    
    return true;
}


size_t WeightedGraph::getSize() const
{
  return size_;
}


void WeightedGraph::printShortestPathsWeights(vector<double>& dist, int vertex)
{
    cout << "The weight of a shortest path from vertex " << vertex << " to vertex:" << endl;
    for (size_t i = 0; i < dist.size(); ++i)
    {
        cout << i << " is " << dist[i] << endl;
    }
    cout << endl;
}


bool WeightedGraph:: isArrayOfShortestPathsWeights(vector <double> arr, int v)
{
    if (arr[v] != 0)
    {
        return false;
    }
    for (int v = 0; v < adjacencyLists_.size(); ++v) // O(E + V), list.size() - O(1)!
    {
        for (auto& edge : adjacencyLists_[v])
        {
            if (isUpdateChangeDist(v, edge, arr))
            {
                return false; // если возможна релаксация
            }
        }
    }
    return true; // если изменений не произошло
}


void testBellmanFord()
{
    int size = 0;
    cin >> size;
    
    WeightedGraph graph(size, cin);
    int pivotVertex = 0;
    cin >> pivotVertex;
    
    vector <double> dist(graph.getSize(), INF);
    vector <int> paths(graph.getSize(), -1);
    
    if (graph.BellmanFord(dist, pivotVertex, paths) == false)
    {
        cout << endl << "A negative-weight cycle is reachable from the source vertex" << endl;
    }
    else
    {
        cout << endl;
        graph.printShortestPathsWeights(dist, pivotVertex);
        graph.printShortestPathTree(paths, pivotVertex);
    }
}

void testVerifyShortestsPathsWeights()
{
    int size = 0;
    cin >> size;
    
    WeightedGraph graph(size, cin);
    int pivotVertex = 0;
    cin >> pivotVertex;
    
    vector<double> arrToCheck;
    string vStr;
    double vDouble;
    
    for (int i = 0; i < size; i++)
    {
        cin >> vStr;
        if (vStr == "inf")
        {
            vDouble = INF;
        }
        else
        {
            vDouble = stod(vStr);
        }
        arrToCheck.push_back(vDouble);
    }
    
    if (graph.isArrayOfShortestPathsWeights(arrToCheck, pivotVertex))
    {
        cout << "Entered array is an array of shortest parts" << endl;
    }
    else
    {
        cout << "Entered array isn't an array of shortest parts";
    }
}


int main ()
{
    int taskNumber = 0;
    cout << "Choose which task you need to project(5 or 6): "; cin >> taskNumber;
    switch (taskNumber)
    {
    case 5:
            testBellmanFord();
            break;
    case 6:
            testVerifyShortestsPathsWeights();
            break;
    default:
            cout << "Task should be 5 or 6!";
    }
    return 0;
}
