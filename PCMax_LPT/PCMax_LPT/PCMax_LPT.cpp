#include "pch.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

class processor
{
	vector<int> tasks;							//pojemnik z kolejnymi czasami zadań
public:
	int sum;									//całkowity czas zadań procesora
	processor();								//konstruktor
	void add(int);								//dodawanie zadania do procesu
	void show();								//wyświetlanie sumy czasów i zadań procesora
};

void LPT(processor*, int, int);

int main()
{
	int m, x, i;
	ifstream file;
	file.open("tasks.txt");
	if (file.good())
	{
		file >> m;								//liczba procesorów
		cout << "Procesory: " << m << endl;

		file >> x;
		cout << "Programy: " << x << endl << endl;
		
		processor *tab = new processor[m];
		vector<int> all;

		while (!file.eof())						//czytanie pliku
		{
			file >> x;
			all.push_back(x);
		}

		sort(all.rbegin(), all.rend());			//sort

		for (i = 0; i < all.size(); i++)		//wywolywanie LPT
		{
			LPT(tab, all[i], m);
		}

		for (int i = 0; i < m; i++)				//wyświetlanie wszystkich procesorów
		{
			tab[i].show();
		}

		file.close();
		delete[] tab;
	}
	else
		cout << "Blad programu!";
	return 0;
}

processor::processor()
{
	sum = 0;
}

void processor::add(int a)
{
	sum += a;									//zwiększamy sumę o a
	tasks.push_back(a);							//dodajemy a do pojemnika danego procesora 
}

void processor::show()
{
	cout << "Zadania:\t";
	for (int i = 0; i < tasks.size(); i++)		//wyświetlamy po kolei wszystkie zadania (ich czasy) 
		cout << tasks[i] << "\t";
	cout << endl;
	cout << "Suma: " << sum << endl << endl;
}

void LPT(processor *tab, int a, int m)
{
	int suma = tab[0].sum, index = 0;			//na początek ustalamy dane pierwszego procesora w tablicy
	for (int i = 0; i < m; i++)					//szukanie procesora z najmniejszą sumą
	{
		if (tab[i].sum < suma)
		{
			suma = tab[i].sum;					//y przechowuje sumę danego procesu
			index = i;							//index to indeks procesu z najmniejszą sumą
		}
	}
	tab[index].add(a);							//dodajemy a do znalezionego procesu
}