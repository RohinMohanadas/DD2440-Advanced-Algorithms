#include <iostream>
#include <cmath>
#include <cstdlib>
#include <random> // std::default_random_engine
#include <ctime>

using namespace std;


int n;                                              //number of cities
int **adjacencyMatrix;                              //adjacency Matrix
int *curr_path, *new_path, *min_path, *greedy_path; //paths


int randomCityGenerator()
{
    return (rand() % (n));
}
double acceptanceProbability(int energy, int newEnergy, double temperature)
{
    if (newEnergy < energy)
    {
        return 1.0;
    }
    return exp((energy - newEnergy) / temperature);
}
void swap(int cityIndex1, int cityIndex2, int *path)
{
    int temp;
    temp = path[cityIndex1];
    path[cityIndex1] = path[cityIndex2];
    path[cityIndex2] = temp;

}
void shuffle(int *path, int city1, int city2)
{
    int temp;
    int b;
here:

    int size, i, j, k;

    if (city1 > city2)
    { 
        city1 = city1 + city2;
        city2 = city1 - city2;
        city1 = city1 - city2;
    }

    size = floor((city2 - city1 + 1) / 2);
    i = city1;
    j = city2;

    for (k = 0; k < size; k++)
        swap(i++, j--, path);

    path[n] = path[0]; //set the last element correctly after shuffle.
}
int cost(int *path)
{
    int cost = 0;
    for (int i = 0; i < n; i++) //for the last element path[i+1] will point back to initial  city
        cost += adjacencyMatrix[path[i]][path[i + 1]];
    return cost;
}
int main(int argc, char **argv)
{
    clock_t tStart = clock();
    clock_t tIter;

    srand(time(0)); //for shuffle
    int i, j = 0;

    double *x, *y; //coordinate arrays.

    int algorithm = 0; //0 = greedy + 2-opt 1 = greedy + SA
    int rand1, rand2;  //random cities.
    //randomness
    double randomNum;
    double probAcceptance;
    //temperature
    double temp = 10000;
    double coolingRate = 0.003;

    //read section
    cin >> n;

    //initialize adjacency Matrix with the number of cities.
    x = new double[n];
    y = new double[n];

    adjacencyMatrix = new int *[n];
    for (int i = 0; i < n; i++)
    {
        adjacencyMatrix[i] = new int[n];
    }

    //read section continued
    for (i = 0; i < n; i++)
        cin >> x[i] >> y[i];

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (i == j)
            {
                adjacencyMatrix[i][j] = 0;
            }
            else
            {
                double xDistance = std::abs(x[i] - x[j]);
                double yDistance = std::abs(y[i] - y[j]);
                double distance = sqrt(abs(xDistance * xDistance + yDistance * yDistance));
                adjacencyMatrix[i][j] = round(distance);
            }
        }
    }

    // 1. This is the nearest neighbour path finding section
    //Greedy path finder
    greedy_path = new int[n + 1];
    // visited = new bool[n];
    bool visited[n] = {false}; //none of the cities are visited.
    int best = 0;

    //Construction Algorithm: Nearest neighbour
    int startCity = randomCityGenerator();
    visited[startCity] = true;
    greedy_path[0] = startCity;
    int currRow = startCity;
    for (int i = 1; i < n; i++)
    {
        int max = 99999999;
        for (int j = 0; j < n; j++)
        {
            if (currRow != j && visited[j] == false && ((adjacencyMatrix[currRow][j] < max)))
            {
                max = adjacencyMatrix[currRow][j];
                best = j;
            }
        }

        greedy_path[i] = best;
        currRow = best;
        visited[best] = true;
    }

    greedy_path[n] = greedy_path[0]; //set last node = first node.
    int minPathCost = cost(greedy_path);

    //initialize the three paths.
    curr_path = new int[n + 1]; //so that the path consists of the initial city twice
    curr_path[0] = curr_path[n] = greedy_path[0];
    for (int i = 1; i < n; i++)
        curr_path[i] = greedy_path[i];
    new_path = new int[n + 1]; //so that the path consists of the initial city twice
    new_path[0] = new_path[n] = greedy_path[0];
    for (int i = 1; i < n; i++)
        new_path[i] = greedy_path[i];
    min_path = new int[n + 1]; //so that the path consists of the initial city twice
    min_path[0] = min_path[n] = greedy_path[0];
    for (int i = 1; i < n; i++)
        min_path[i] = greedy_path[i];


    int currCost = 999999999;
    int diff = 0;


    // 2. This is path optimization using 2-opt. (Find 2-adjacent paths and use path if better.)
    // Optimization Algorithm: 2-opt algorithm
    clock_t t2Opt = clock();
    int improve = 0;
    if (1 == 1)
    {
        while (improve < 100)
        {

            // n*(n-1)/2 pairs in the graph
            for (int i = 0; i < n - 1; i++)
            {
                for (int j = i + 1; j < n; j++)
                {

                    int e1End = new_path[i];
                    int e1Start = new_path[(i - 1) % n];
                    int e2Start = new_path[j];
                    int e2End = new_path[(j + 1) % n];
                    diff = adjacencyMatrix[e2Start][e2End] + adjacencyMatrix[e1Start][e1End] - (adjacencyMatrix[e2Start][e1Start] + adjacencyMatrix[e1End][e2End]);

                    if (diff <= 0)
                        continue;
                    shuffle(new_path, i, j);
                    {
                        //shuffle accepted.
                        for (int k = 0; k < n + 1; k++)
                        {
                            min_path[k] = new_path[k];
                        }
                        minPathCost = cost(min_path);
                        improve = 0;
                    }
                }
                
            }
            improve++;
        }
        for (int k = 0; k < n + 1; k++)
        {
            curr_path[k] = min_path[k];
        }
    }


    // 3. Simulated annealing: This works better on random path inputs.
    
    //Optimization Algorithm: Simulated annealing
    int currentCost = cost(curr_path);
    int newPathCost = 0;
    while (temp > 1)
    {
        int ctr = 1000;
        tIter = clock();
        while (--ctr)
        {
            //path, find new neighbour path.
            if ((double)(clock() - tStart) / CLOCKS_PER_SEC > 1.85)
                goto timeout;

            for (int i = 0; i < n + 1; i++)
            {
                new_path[i] = curr_path[i];
            }

            rand1 = randomCityGenerator();
            rand2 = randomCityGenerator();

            shuffle(new_path, rand1, rand2);

            newPathCost = cost(new_path);

            if (currentCost > newPathCost && minPathCost > newPathCost)
            {
                for (int i = 0; i < n + 1; i++)
                {
                    min_path[i] = new_path[i];
                    curr_path[i] = new_path[i];
                    currentCost = cost(curr_path);
                    minPathCost = currentCost;
                }
            }
            else
            {
                randomNum = (double)rand() / (double)RAND_MAX;
                probAcceptance = acceptanceProbability(currentCost, newPathCost, temp);
                if (probAcceptance > randomNum)
                { //annealing

                    for (int i = 0; i < n + 1; i++)
                        curr_path[i] = new_path[i];
                    currentCost = cost(curr_path);
                }
            }
        }

        temp *= 1 - coolingRate;
        if ((double)(clock() - tStart) / CLOCKS_PER_SEC > 1.8)
            break;
    }

timeout:
    for (int i = 0; i < n; i++)
        cout << min_path[i] << endl;
        
        // cout<<"Cost"<<cost(min_path);
    return 0;
}