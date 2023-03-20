
#include <iostream>
#include <fstream>
#include <omp.h>
#include <chrono>
#include "image.h"
#include "filters.h"

using namespace std;

int main()
{
	vector<string> names = { "300x300", "400x400", "500x500", "600x600", "950x950", "2400x2400" };
	string format = ".png";
	
	vector<double> aveTimes(names.size(), 0.0);
	int iterations = 100;

	fstream file("");

	for (int i = 0; i < names.size(); i++) 
	{
		string nameFile = names[i];
		Image img;
		
		file << "Open file: " + nameFile + format + " ";
		cout << "Open file: " + nameFile + format + " ";

		if (!img.loadFromFile("imageInput\\" + nameFile + format))
		{
			file << "success" << endl;
			file << "CalculationOfAverageTime: its-" << iterations << " -> ";
			cout << "success" << endl;
			cout << "CalculationOfAverageTime: its-" << iterations << " -> ";

			for (int j = 0; j < iterations; j++)
			{
				auto start = chrono::steady_clock::now();

				//filterNegative(img);
				//filterNegativeVec(img);
				//filterMedian(img, 16);
				//filterMedianVec(img, 16);
				
				auto end = chrono::steady_clock::now();
				double seconds = chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000000.0;

				aveTimes[i] += seconds;
			}

			file << "aveTimes-" << aveTimes[i] / (double)iterations << " -> fullTime-" << aveTimes[i] << endl;
			cout << "aveTimes-" << aveTimes[i] / (double)iterations << " -> fullTime-" << aveTimes[i] << endl;
		}
		else 
		{
			file << "fail" << endl;
			cout << "fail" << endl;
		}
		file << endl;
		cout << endl;
	}
}