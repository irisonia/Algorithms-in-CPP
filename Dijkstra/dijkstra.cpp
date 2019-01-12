#include <algorithm>
#include <iostream>
#include <climits>
#include <string>
#include <vector>
#include <deque>
#include <map>

using namespace std;

static void getCities(
        int num_cities,
        vector<string> &city_name_by_idx,
        map<string, int> &city_idx_by_name);

static void getAdjacencies(
        int num_cities,
        int num_adjacencies,
        const map<string, int> &city_idx_by_name,
        vector<vector<int>> &adjacency_matrix);

static void getAndSolveQuery(
        const vector<string> &city_name_by_idx,
        const map<string, int> &city_idx_by_name,
        const vector<vector<int>> &adjacency_matrix);

static void Dijkstra(
        const vector<string> &city_name_by_idx,
        const vector<vector<int>> &adjcency_mat,
        int from,
        int to);

int main()
{
    int num_cities;
    int num_edges;
    vector<string> city_name_by_idx;
    map<string, int> city_idx_by_name;
    vector<vector<int>> adjacency_matrix;

    /* get cities */
    cin >> num_cities;
    cin.ignore();
    adjacency_matrix.resize(num_cities);
    city_name_by_idx.resize(num_cities);
    getCities(num_cities, city_name_by_idx, city_idx_by_name);
    cin.ignore();

    /* get adjacencies */
    cin >> num_edges;
    cin.ignore();
    getAdjacencies(num_cities, num_edges, city_idx_by_name, adjacency_matrix);
    cin.ignore();

    /* get and solve query */
    getAndSolveQuery(city_name_by_idx, city_idx_by_name, adjacency_matrix);

    return 0;
}

static void getCities(
        int num_cities,
        vector<string> &city_name_by_idx,
        map<string, int> &city_idx_by_name)
{
    for (int i = 0; i < num_cities; ++i)
    {
        string city_name;

        getline(cin, city_name, '\n');
        city_idx_by_name[city_name] = i;
        city_name_by_idx[i] = city_name;
    }
}

static void getAdjacencies(
        int num_cities,
        int num_adjacencies,
        const map<string, int> &city_idx_by_name,
        vector<vector<int>> &adjacency_matrix)
{
    for (int i = 0; i < num_adjacencies; ++i)
    {
        string str;
        int from, to, dist;

        getline(cin, str, ',');
        from = city_idx_by_name.at(str);
        cin.ignore();
        getline(cin, str, ',');
        to = city_idx_by_name.at(str);
        cin >> dist;
        cin.ignore();

        if (adjacency_matrix[from].empty())
        {
                adjacency_matrix[from].resize(num_cities);
        }
        adjacency_matrix[from][to] = dist;
    }
}

static void getAndSolveQuery(
        const vector<string> &city_name_by_idx,
        const map<string, int> &city_idx_by_name,
        const vector<vector<int>> &adjacency_matrix)
{
    string str;
    int from, to;

    getline(cin, str, ',');
    from = city_idx_by_name.at(str);
    cin.ignore();
    getline(cin, str, '\n');
    to = city_idx_by_name.at(str);

    Dijkstra(city_name_by_idx, adjacency_matrix, from, to);
}

struct BestDistance
{
    int m_distance;
    int m_prev_city;

    explicit BestDistance(int dst, int prv) : m_distance(dst), m_prev_city(prv)
    {}
};
struct City
{
    int m_id;

    explicit City(int id): m_id(id)
    {}

    /* for sorting by distance */
    static const vector<BestDistance> *dists;
};
const vector<BestDistance> *City::dists = nullptr;

bool operator<(const City& city1, const City& city2)
{
    return (*City::dists)[city1.m_id].m_distance < (*City::dists)[city2.m_id].m_distance;
}

static void Dijkstra(
        const vector<string> &city_name_by_idx,
        const vector<vector<int>> &adjcency_mat,
        int from,
        int to)
{
    int cur_city_id = from;
    int num_cities = city_name_by_idx.size();
    vector<int> visited(num_cities, 0);
    vector<BestDistance> dists(num_cities, BestDistance(INT_MAX, to));
    deque<City> adjacencts_by_nearest;

    /* for sorting queued adjacents by distance */
    City::dists = &dists;

    /* initialize route with 'from' */
    dists[from].m_distance = 0;
    dists[from].m_prev_city = from;

    adjacencts_by_nearest.emplace_back(from);
    while (! adjacencts_by_nearest.empty())
    {
        /* choose the closest next city */
        sort(adjacencts_by_nearest.begin(), adjacencts_by_nearest.end());

        cur_city_id = adjacencts_by_nearest.front().m_id;
        adjacencts_by_nearest.pop_front();

        if (1 == visited[cur_city_id])
        {
            continue;
        }
        visited[cur_city_id] = 1;

        if (to == cur_city_id)
        {
            break;
        }

        /* update distances to adjacents of 'cur'.
         if an adjacent city distance has improved, add to queue */
        size_t dist_so_far = dists[cur_city_id].m_distance;
        vector<int>::const_iterator it = adjcency_mat[cur_city_id].begin();
        vector<int>::const_iterator it_end = adjcency_mat[cur_city_id].end();
        for (int adj_idx = 0; it != it_end; ++adj_idx, ++it)
        {
            /* explanation of the following 'if' statement:
            (*it != 0 ) --> the city which index is adj_idx is adjacent to cur city
            (0 == visited[adj_idx]) -->  and hasn't yet been added to route. */
            if ((*it != 0) && (0 == visited[adj_idx]))
            {
                int new_dist_to_adjecent = *it + dist_so_far;
                if (new_dist_to_adjecent < dists[adj_idx].m_distance)
                {
                    dists[adj_idx].m_distance = new_dist_to_adjecent;
                    dists[adj_idx].m_prev_city = cur_city_id;
                    adjacencts_by_nearest.push_back(City(adj_idx));
                }
            }
        }
    }
    City::dists = nullptr;

    if (cur_city_id != to)
    {
        cout << "No Route Found..." << endl;
        return;
    }

    /* recycle 'visited' to remember the route, as we back-trace it from 'to' to 'from' */
    int i;
    for (i = 0; cur_city_id != from; ++i)
    {
        visited[i] = cur_city_id;
        cur_city_id = dists[cur_city_id].m_prev_city;
    }
    visited[i] = from;

    /* print the route by going backwards in visied */
    unsigned long total_dist = 0;
    const string print_delim[] = {"\n", "--->"};
    while (i >= 0)
    {
        cur_city_id = visited[i];

        int added_dist = adjcency_mat[dists[cur_city_id].m_prev_city][cur_city_id];
        total_dist += added_dist;

        cout << city_name_by_idx[cur_city_id] << "(+" << added_dist << ")" << print_delim[!!i];
        --i;
    }
    cout << "Total distance : " << total_dist << endl;
}


