#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

volatile int DISTANCE_CALCULATIONS = 0;
volatile int INNER_DISTANCE_CALCULATIONS = 0;
int RECURSIVE_CALLS = 0;

struct Point
{
	Point(int x, int y) : x{x}, y{y} 
	{}

	int x;
	int y;
};

Point getRandomPoint()
{
	return {rand(), rand()};
}

enum Algorithm
{
	BRUTE,
	DIVIDE,
	BOTH
};

float euclidDist( Point* a, Point* b)
{
	DISTANCE_CALCULATIONS += 1;
	return sqrt( pow(a->x - b->x, 2) + pow(a->y - b->y, 2) );
}

float bruteForceClosestPair( vector<Point>& points, pair<Point*, Point*>& closestPair)
{
	float dist = FLT_MAX;
	
	for(int i = 0; i < points.size()-1; i++)
	{
		for(int j = i+1; j < points.size(); j++)
		{
			float temp = euclidDist(&points[i], &points[j]);
			if( temp < dist )
			{
				dist = temp;

				closestPair.first = &points[i];
				closestPair.second = &points[j];
			}
		}
	}

	return dist;
}

float bruteForceClosestPair( const vector<Point*>& points, pair<Point*, Point*>& closestPair)
{
	float dist = FLT_MAX;
	
	for(int i = 0; i < points.size()-1; i++)
	{
		for(int j = i+1; j < points.size(); j++)
		{
			float temp = euclidDist(points[i], points[j]);
			if( temp < dist )
			{
				dist = temp;

				closestPair.first = points[i];
				closestPair.second = points[j];
			}
		}
	}

	return dist;
}

// Q SORTING
void swap(vector<pair<Point*, int>>& points, int a, int b)
{
	pair<Point*, int> temp = points[a];
	points[a] = points[b];
	points[b] = temp;
}

int partition(vector<pair<Point*, int>>& points, int l, int r)
{
	int p = points[r].first->y;
	int i = (l-1);

	for( int j = l; j <= r-1; j++)
	{
		if(points[j].first->y < p)
		{
			i++;
			swap(points, i, j);
		}
	}

	swap(points, i+1, r);
	return (i+1);
}

void quicksort_y(vector<pair<Point*, int>>& points, int l, int r)
{
	if( l < r)
	{
		int s = partition(points, l, r);
		quicksort_y(points, l, s-1);
		quicksort_y(points, s+1, r);
	}
}


// P SORTING

void swap(vector<Point*>& points, int a, int b)
{
	Point* temp = points[a];
	points[a] = points[b];
	points[b] = temp;
}

int partition(vector<Point*>& points, int l, int r)
{
	int p = points[r]->x;
	int i = l-1;

	for(int j = l; j <= r-1; j++)
	{
		if(points[j]->x < p)
		{
			i++;
			swap(points, i, j);
		}
	}
	swap(points, i+1, r);
	return i+1;
}

void quicksort_x(vector<Point*>& points, int l, int r)
{
	if( l < r)
	{
		int s = partition(points, l, r);
		quicksort_x(points, l, s-1);
		quicksort_x(points, s+1, r);
	}
}

float divideClosetPointSearch(vector< Point* >& P, vector<pair<Point*, int>>& Q, pair<Point*, Point*>& closest)
{
	RECURSIVE_CALLS++;

	// P is small enough, just bruteforce it
	if(P.size()<= 3)
	{
		return bruteForceClosestPair(P, closest);
	}
	else
	{
		// Points on the left
		vector<Point*> PL;
		for(int i = 0; i < P.size()/2; i++)
		{
			PL.push_back(P[i]);
		}

		// Points on the right
		vector<Point*> PR;
		for(int i = P.size()/2; i < P.size(); i++)
		{
			PR.push_back(P[i]);
		}

		vector<pair<Point*, int>> QL;
		vector<pair<Point*, int>> QR;
		// Put points into a Q based on their x position
		for(int i = 0; i < Q.size(); i++)
		{
			if( Q[i].second < P.size()/2)
				QL.push_back(Q[i]);
			else
				QR.push_back(Q[i]);
		}

		// Find the closest pair on the left
		pair<Point*, Point*> cl;
		float dl = divideClosetPointSearch(PL, QL, cl);

		// Find the closest pair on the right
		pair<Point*, Point*> cr;
		float dr = divideClosetPointSearch(PR, QR, cr);

		// Find the closest of the two
		float d;
		if( dl < dr)
		{
			closest = cl;
			d = dl;
		}
		else
		{
			closest = cr;
			d = dr;
		}

		// Generate the strip
		int mid = P[ceil(P.size()/2)-1]->x;
		vector< pair<Point*, int>* > S;
		for( auto& q : Q)
		{
			if(abs(q.first->x - mid) < d)
			{
				S.push_back(&q);
			}
		}

		// Used so we don't have to do sqrt inside the loop
		float dminsq = d * d;

		int size = S.size();
		for( int i = 0; i <= size-2; i++)
		{
			int k = i+1;
			while(k <= size-1 && (pow(S[i]->first->y - S[k]->first->y, 2) < dminsq  ))
			{
				DISTANCE_CALCULATIONS += 1;
				float dist = pow(S[i]->first->x - S[k]->first->x,2) + pow(S[i]->first->y - S[k]->first->y, 2);
				
				if( dist < dminsq)
				{
					dminsq = dist;
					closest.first = S[i]->first;
					closest.second = S[k]->first;
				}

				k++;
			}
		}
		

		return sqrt(dminsq);
	}
}

float divideClosestPoint( vector<Point>& points, pair<Point*, Point*>& closestPair )
{
	//copy points into P
	vector< Point*> P;
	for( auto& p : points)
		P.push_back( &p );

	//sort P by X
	quicksort_x(P, 0, P.size()-1);

	
	//copy points from P into Q with the index of the point in P
	vector<pair<Point*, int>> Q;
	for( int i = 0; i < P.size(); i++)
		Q.emplace_back(P[i], i);

	//sort Q by Y
	quicksort_y(Q, 0, Q.size()-1);
	
	cout << "P Size: " << P.size() << "\n";
	cout << "Q Size: " << Q.size() << "\n";
	// Do the actual search
	return divideClosetPointSearch(P, Q, closestPair);
}

bool equalIC(const string& a, const string& b)
{
	// if they are identical, the language can handle that
	if( a == b ) return true;

	bool areEqual = true;

	// must be same length
	if( a.length() != b.length() )
		areEqual = false;

	// compare each char
	for( int i = 0; i < a.length() && areEqual; i++)
	{
		// if they are not the same, we are done
		if( toupper(a[i]) != toupper(b[i]))
			areEqual = false;
	}

	return areEqual;
}



int getNextInt()
{
	int input;

	while( !( cin >> input ) )
	{
		cout << "Bad input" << endl;
		cin.clear();
		string bad;
		getline(cin, bad);
	}

	return input;
}

unsigned getNextPos()
{
	bool valid = false;
	int input;

	while( !valid )
	{
		while( !( cin >> input ) )
		{
			cout << "Bad input" << endl;
			cin.clear();
			string bad;
			getline(cin, bad);
		}

		if(input < 0)
			cout << "Must be positive." << endl;
		else
			valid = true;
	}

	return input;
}

Algorithm getAlgorithm()
{
	string algorithm;

	while( true )
	{
		cout << "Please choose an algorithm (BRUTE, DIVIDE, BOTH): ";
		getline(cin, algorithm);

		if( equalIC(algorithm, "BRUTE"))
		{
			cout << "Brute force algorithm selected." << endl;
			return BRUTE;
		}
		if( equalIC(algorithm, "DIVIDE"))
		{
			cout << "Divide and Conquer algorithm selected." << endl;
			return DIVIDE;
		}
		if( equalIC(algorithm, "BOTH"))
		{
			cout << "Both algorithms will be used." << endl;
			return BOTH;
		}

		cout << "Unrecognized algorithm.\n\n";
	}
}


vector<Point> points;
Algorithm selected_algorithm;

int main()
{
	// Accept a commandline argument
	selected_algorithm = getAlgorithm();

	// Take in the number of points
	cout << "Enter point count: ";
	unsigned count = getNextPos();

//	cout << "Enter iterations: ";
//	unsigned howMany = getNextPos();
//
//	int total = 0;
//
//	for(int i = 0; i < howMany; i++)
//	{
		points.clear();
		// Loop and get the points
		for(int p = 0; p < count; p++)
		{
			int x = getNextInt();
			int y = getNextInt();

			points.emplace_back( x, y );

//					points.emplace_back( getRandomPoint() );
		}


		// Punch out the sorts
		if( points.size() >= 2)
		{
			if( selected_algorithm == DIVIDE || selected_algorithm ==  BOTH)
			{
				DISTANCE_CALCULATIONS = 0;
				cout << "Algorithm: Divide and Conquer\n\n";

				cout << "N: " << points.size() << "\n\n";

				pair<Point*, Point*> closest{&points[0], &points[1]};

				float distance = divideClosestPoint(points, closest);

//				pair<Point*, Point*> closest2{&points[0], &points[1]};
//				if( distance != bruteForceClosestPair(points, closest2))
//					cout << "*********RESULTS DIVERGE!************" << endl;

				cout << "Point 1: (" << closest.first->x << ", " << closest.first->y << ")\n";
				cout << "Point 2: (" << closest.second->x << ", " << closest.second->y << ")\n\n";

				cout << "Distance squared: " << round(distance*distance) << "\n\n";

				cout << "Distance: " << distance << "\n\n";

				cout << "Number of distance calcs: " << DISTANCE_CALCULATIONS << endl;
				cout << "Number of Inner distance calcs: " << INNER_DISTANCE_CALCULATIONS << endl;
				cout << "Number of calls: " << RECURSIVE_CALLS << endl;

//				total += DISTANCE_CALCULATIONS;
			}

			if( selected_algorithm == BOTH )
				cout << "\n\n";

			if(selected_algorithm == BRUTE || selected_algorithm == BOTH)
			{
				DISTANCE_CALCULATIONS = 0;

				cout << "Algorithm: Brute Force\n\n";

				cout << "N: " << points.size() << "\n\n";
	
				pair<Point*, Point*> closest{&points[0], &points[1]};
				float distance = bruteForceClosestPair(points, closest);

				cout << "Point 1: (" << closest.first->x << ", " << closest.first->y << ")\n";
				cout << "Point 2: (" << closest.second->x << ", " << closest.second->y << ")\n\n";

				cout << "Distance squared: " << round(distance*distance) << "\n\n";

				cout << "Distance: " << distance << "\n\n";

				cout << "Number of distance calcs: " << DISTANCE_CALCULATIONS << endl;
			}
		}
		else
		{
			cout << "Error: n = " << points.size() << ". Should be >= 2" << endl;
		}
//	}

	
	
//	cout << "Average: " << (total/howMany) << "\n";
	char a;
	cin >> a;
	return 0;
}