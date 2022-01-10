#include <iostream>
#include <vector>
#include <list>

using namespace std;

// граф храним в виде списков смежности
using Graph = vector <list <int>>;

void visit_tr(Graph g_tr, vector <bool>& used, int v, list <int>& order)
{
    used[v] = true;
    for (int w : g_tr[v])
    {
        if (!used[w])
        {
            visit_tr(g_tr, used, w, order);
        }
    }
    // Вершина заносится в начало списка
    order.push_front(v);
}


void dfs_tr(Graph& g_tr, list <int>& order)
{
    vector <bool> used(g_tr.size(), false);
    for (int u = 0; u < g_tr.size(); u++)
    {
        if (!used[u])
        {
            visit_tr(g_tr, used, u, order);
        }
    }
}


void visit(Graph& g, vector <bool>& used, int v, list <int>& component, vector<int>& numSCC, int curSCC)
{
    used[v] = true;
    component.push_back(v); // заполняем списки вершинами сск numSCC[v] = curSCC; // заполняем массив номерами сск
    for (int u : g[v])
    {
        if (!used[u])
        {
            visit(g, used, u, component, numSCC, curSCC);
        }
    }
}

int dfs(Graph& g, list <int>& order, vector <list <int>>& components, vector<int>& numSCC)
{
    vector <bool> used(g.size(), false); int curSCC = 0;
    for (auto u : order) {
        if (!used[u])
        {
            list <int> component;
            visit(g, used, u, component, numSCC, curSCC); curSCC++;
            // запись сск
            components.push_back(component);
        }
    }
    return curSCC; // возвращаем кол-во полученных сск
}


Graph get_transposed_graph(Graph& g)
{
    Graph transposed_graph(g.size());
    
    for (int vertex = 0; vertex < g.size(); ++vertex)
    {
        for (auto child : g[vertex])
        {
            transposed_graph[child].push_back(vertex);
                
        }
    }
    return transposed_graph;
}


int kosarayu_scc(Graph& g, vector <list <int>>& components, vector<int>& numSCC)
{
    Graph transposed_graph = get_transposed_graph(g); // получаем трансп. граф
    list <int> order; // список верщин в порядке убываниях их post-значениий
    dfs_tr(transposed_graph, order); // обход трансп. графа и заполнение order
    int curSCC = dfs(g, order, components, numSCC); // Нахождение компонент сильной связности
    return curSCC; // возвращаем кол-во сск
}


Graph get_metagraph(Graph& g, vector <list <int>>& components, vector<int>& numSCC)
{
    vector <bool> used(components.size(), false); Graph meta_graph(components.size());
    for (int i = 0; i < components.size(); i++) // по номерам сск
    {
        for (int v : components[i]) // по вершинам в сск
        {
            for (int e : g[v]) // по списку смежности вершины графа
            {
                if ((numSCC[v] != numSCC[e]) && !used[numSCC[e]])
                {
                    used[numSCC[e]] = true; // отмечаем, что с этой сск ребро уже провели
                    meta_graph[i].push_back(numSCC[e]); // проводим ребро
                }
                
                // нужно пройтись только по тем где назначили true
                for (int v : meta_graph[i])
                {
                    used[v] = false;
                }
            }
        }
    }
    return meta_graph;
}



void fill_graph(Graph& g)
{
    int v = 0;
    int e = 0;
    // Заполняем граф
    int i = 0;
    while (i != g.size())
    {
        cin >> v >> e;
        if (e == -1)
        {
            ++i;
            continue;
        }
        g[v].push_back(e); cin >> e;
        while (e != -1)
        {
            g[v].push_back(e);
            cin >> e;
        }
        ++i;
    }
}


int main(int argc, const char * argv[])
{
    int n = 0; // число вершин в графе
    std::cin >> n;
    Graph g(n); // Graph = vector <list <int>>
    fill_graph(g);
    vector <list <int>> scc_lists; // списки сск
    vector<int> numSCC(n); // массив номеров сск
    int num_of_scc = kosarayu_scc(g, scc_lists, numSCC);
    // Вывод сск
    for (int v = 0; v < num_of_scc; v++)
    {
        cout << endl << "SCC " << v << ": ";
        for (int e = 0; e < n; e++)
        {
            if (numSCC[e] == v) {
                cout << e << " ";
            }
        }
    }
        
    vector <list <int>> meta_graph = get_metagraph(g, scc_lists, numSCC);
    cout << endl << "Meta-graph:" << endl;
        
    for (size_t i = 0; i < meta_graph.size(); ++i)
    {
        cout << i;
        for (auto edge : meta_graph[i]) {
            cout << ' ' << edge;
        }
        cout << endl;
    }
    
    return 0;
}
