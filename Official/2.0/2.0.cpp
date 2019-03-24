#include "pch.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
using namespace std;

const int Population_Size = 100;    //algorytm dostosowany do parzystych
const int Generation_Reps = 99999;   //powtorzenia algorytmu
const double Mutation_Prob = 0.1;   //szansa na mutacje 
const double Crossover_Prob = 0.65; //szansa na krzyzowanie 
const string plik = "m25.txt";		//nazwa przetwarzanego pliku

int procs;	//liczba procesorow
int tasks;	//liczba instancji
int *tasktab; // tablica z zadaniami

class Solution 
{
public:
	vector<int> tab;		//ciąg procesorów
	vector<int> quantity;	//czasy zadan na procesorach
	int max;				//maksymalny czas w osobniku	
	void findmax();
	Solution();
	Solution(int); 
};


int draw(int);					//losowanie 
void mutation(vector<Solution> &);
void crossover(vector<Solution> &, vector<Solution> &);

int main()
{
	srand(static_cast<unsigned>(time(NULL)));
	fstream file;
	file.open(plik);
	int tmp_int, Generation = 0, licz;
	clock_t start, end;

	file >> procs;
	file >> tasks;

	tasktab = new int[tasks];

	for (int i = 0; i < tasks; i++) 
	{
		file >> tmp_int;
		tasktab[i] = tmp_int;
	}
	file.close();

	vector<int> tmp_vec;			//vector tymczasowy potrzebny do tworzenia osobnikow
	tmp_vec.resize(tasks);

	vector<Solution> Temp_Pop;		//vector tymczasowy potrzebny do porownywania wynikow po mutacji/crossoverze
	Temp_Pop.resize(Population_Size);

	for (int i = 0; i < tasks; i++)	//wypelnianie vectora indeksami 0 - (procs-1)
		tmp_vec[i] = i % procs;
	
	vector<Solution> Pop;			//pojemnik na wszystkich osobnikow
	Pop.reserve(Population_Size);

	vector<Solution> First_Gen;		//pojemnik na pierwsze losowanie osobnikow
	First_Gen.reserve(2 * Population_Size);

	for (int i = 0; i < 2 * Population_Size; ++i)		//tworzenie osobnikow
	{
		random_shuffle(tmp_vec.begin(), tmp_vec.end());
		First_Gen.push_back(Solution(666));			//wywolanie konstruktora
		First_Gen[i].tab = tmp_vec;
		First_Gen[i].findmax();
	}
	tmp_vec.shrink_to_fit();			//usuwanie pomocniczego 

	sort(First_Gen.begin(), First_Gen.end(),							//sortowanie vectora sumami rosnaco
		[](const Solution& left, const Solution& right)
		{return left.max < right.max; });
	
	for (int i = 0; i < Population_Size; ++i)		//przepisanie polowy najlepszych osobnikow
	{
		Pop.push_back(Solution(666));
		Pop[i].tab = First_Gen[i].tab;
		Pop[i].max = First_Gen[i].max;
	}

	First_Gen.shrink_to_fit();	

	cout << "Top 20 wynikow" << endl;
	for (int i = 0; i < 20; i++)
		cout << Pop[i].max << endl;

	start = clock();
	while (true)
	{
		if (((double)clock() - start) / CLOCKS_PER_SEC >= 180 || Generation == Generation_Reps)
		{
			end = ((double)clock() - start) / CLOCKS_PER_SEC;
			break;
		}
		
		++Generation;
		Temp_Pop = Pop;			
		
		crossover(Pop, Temp_Pop);	
		mutation(Temp_Pop);
		
		for (int i = 0; i < Population_Size; ++i)		//wyznaczenie maksymalnych czasow
			Temp_Pop[i].findmax();

		sort(Temp_Pop.begin(), Temp_Pop.end(),						//sortowanie vectora sumami rosnaco
			[](const Solution& left, const Solution& right)
			{return left.max < right.max; });

		licz = 0;
		for (int i = 0; i < Population_Size; ++i)		//selekcja
		{
			if (Pop[i].max > Temp_Pop[licz].max)
			{
				Pop[i].tab = Temp_Pop[licz].tab;
				Pop[i].max = Temp_Pop[licz].max;
				licz++;
			}
		}
	}

	cout << endl << endl << "Top 20 wynikow" << endl;
	for (int i = 0; i < 20; i++)
		cout << Pop[i].max << endl;

	cout << endl << "Program wykonal "<< Generation << " generacji w czasie "<< end << "s" << endl;
}

Solution::Solution()
{
}

Solution::Solution(int xD)
{
	quantity.resize(procs);
	tab.resize(tasks);
	fill(tab.begin(), tab.end(), 0);
	max = 0;
}

void Solution::findmax()
{
	fill(quantity.begin(), quantity.end(), 0);	//wypelnienie zerami
	max = 0;
	for (int i = 0; i < tasks; i++)
	{
		quantity[tab[i]] += tasktab[i];
	}
	max = *max_element(quantity.begin(), quantity.end());
}

int draw(int high)
{
	static mt19937 engine((random_device()()));
	static uniform_int_distribution<int> dist;
	uniform_int_distribution<int>::param_type p(0, high - 1);
	return dist(engine, p);
}

void mutation(vector<Solution> &Temp)
{
	int x, y;
	for (int i = 0; i < Population_Size; i++)
	{
		if ((draw(100) + 1) > Mutation_Prob * 100)
			continue;

		x = draw(tasks);
		y = draw(tasks);
		while (y == x)
			y = draw(tasks);

		swap(Temp[i].tab[x], Temp[i].tab[y]);
	}
}

void crossover(vector<Solution> &Pop, vector<Solution> &Temp)
{
	int parent1, parent2, n, m;
	for (int i = 0.2 * Population_Size; i < Population_Size; i+=2)
	{
		if ((draw(100) + 1) > Crossover_Prob * 100)
			continue;

		parent1 = draw(0.2 * Population_Size);
		parent2 = draw(Population_Size - 0.2 * Population_Size) + 0.2 * Population_Size;
			
		n = draw(tasks / 2);		
		while (n == 0)
			n = draw(tasks / 2);
		m = draw(tasks);
		while (m <= n)
			m = draw(tasks);

		for (int j = 0; j < tasks; j++)
		{
			if (j < n || j >= m)
			{
				Temp[i].tab[j] = Pop[parent1].tab[j];
				Temp[i + 1].tab[j] = Pop[parent2].tab[j];
			}
			else
			{
				Temp[i].tab[j] = Pop[parent2].tab[j];
				Temp[i + 1].tab[j] = Pop[parent1].tab[j];
			}
		}
	}
}
