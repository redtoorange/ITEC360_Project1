#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <climits>
using namespace std;

//! Global to count the number of times we run the distance calculation
volatile int DISTANCE_CALCULATIONS = 0;

//! Global to count the number of recursive calls the sorting algorithm makes
int RECURSIVE_CALLS = 0;

//! A simple data type that contains an x and a y.
struct Point
{
	Point(int x, int y) : x{x}, y{y} 
	{}

	int x;
	int y;
};

//! Simple enum used to store which algorithm the program should use.
enum Algorithm
{
	BRUTE,
	DIVIDE,
	BOTH
};


/**
 *	@brief	Generate a random point and return it.
 *	
 *	@return A random point with an X and a Y between 0 and 20000
 */
Point getRandomPoint()
{
	return {rand()%2000, rand()%2000};
}



/**
 *	@brief		Calculate the Euclidean Distance between two points.
 *	
 *	@param a	Point A
 *	@param b	Point B
 *	
 *	@return The Euclidean Distance between A and B
 */
double euclidDist( Point a, Point b)
{
	DISTANCE_CALCULATIONS += 1;
	return sqrt( pow(a.x - b.x, 2) + pow(a.y - b.y, 2) );
}

/**
 *	@brief	Using brute force, calculate the two closest points and their distance.  
 *	
 *	@param points		Vector of points to find the closest pair in.
 *	@param closestPair	Will contain a copy of the two closest points.
 *	
 *	@return Int. The Euclidean distance between the two closest points in the data set.
 */
double bruteForceClosestPair( vector<Point>& points, pair<Point, Point>& closestPair)
{
	double dist = INT_MAX;
	
	for(int i = 0; i < points.size(); i++)
	{
		for(int j = i+1; j < points.size(); j++)
		{
			double temp = euclidDist(points[i], points[j]);
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
/**
 *	@brief		Merge the points from B and C back into A sorted.
 *	
 *	@param B	left vector of sorted points
 *	@param C	right vector of sorted points
 *	@param A	vector to merge the two halves into.
 *	
 *	@return Void.
 */
void merge(vector<pair<Point, Point*>>& B, vector<pair<Point, Point*>>& C, vector<pair<Point, Point*>>& A)
{
	int p = B.size();
	int q = C.size();

	int i = 0;
	int j = 0;
	int k = 0;

	// Merge B and C into A until one vector is empty
	while( i < p && j < q )
	{
		if(B[i].first.y <= C[j].first.y)
		{
			A[k] = B[i];
			i++;
		}
		else
		{
			A[k] = C[j];
			j++;
		}
		k++;
	}

	// Merge back remaining points
	if( i == p)
	{
		while (j < q)
		{
			A[k] = C[j];
			j++;
			k++;
		}
	}
	else
	{
		while (i < p)
		{
			A[k] = B[i];
			i++;
			k++;
		}
	}    
}

/**
 *	@brief	Mergesort the points inside A in order of their Y coordinate
 *	
 *	@param A	vector of points to sort.
 *	
 *	@return Void.
 */
void merge_sort(vector<pair<Point, Point*>>& A)
{
	if(A.size() > 1)
	{
		// Copy left and right points of A into B and C
		vector<pair<Point, Point*>> B, C;
		for(int i = 0; i < A.size()/2; i++)
			B.push_back(A[i]);

		for(int j = A.size()/2; j < A.size(); j++)
			C.push_back(A[j]);

		// sort each half
		merge_sort(B);
		merge_sort(C);

		// merge all back into A
		merge(B, C, A);
	}
}


// P SORTING
/**
 *	@brief		Merge the points from B and C back into A sorted.
 *	
 *	@param B	left vector of sorted points
 *	@param C	right vector of sorted points
 *	@param A	vector to merge the two halves into.
 *	
 *	@return Void.
 */
void merge(vector<Point>& B, vector<Point>& C, vector<Point>& A)
{
	int p = B.size();
	int q = C.size();

	int i = 0;
	int j = 0;
	int k = 0;

	// Merge B and C into A until one vector is empty
	while( i < p && j < q )
	{
		if(B[i].x <= C[j].x)
		{
			A[k] = B[i];
			i++;
		}
		else
		{
			A[k] = C[j];
			j++;
		}
		k++;
	}

	// Merge remaining points into A
	if( i == p)
	{
		while (j < q)
		{
			A[k] = C[j];
			j++;
			k++;
		}
	}
	else
	{
		while (i < p)
		{
			A[k] = B[i];
			i++;
			k++;
		}
	}    
}

/**
 *	@brief	Mergesort the points inside A in order of their X coordinate
 *	
 *	@param A	vector of points to sort.
 *	
 *	@return Void.
 */
void merge_sort(vector<Point>& A)
{
	if(A.size() > 1)
	{
		// Copy out left and right of A into B and C
		vector<Point> B, C;
		for(int i = 0; i < A.size()/2; i++)
			B.push_back(A[i]);

		for(int j = A.size()/2; j < A.size(); j++)
			C.push_back(A[j]);

		// Sort each half
		merge_sort(B);
		merge_sort(C);

		// Merge both into A
		merge(B, C, A);
	}
}


/**
 *	@brief		Calculate the Euclidean Distance between two points.
 *	
 *	@param a	Point A
 *	@param b	Point B
 *	
 *	@return The Euclidean Distance between A and B
 */
double divideClosetPointSearch(vector< Point >& P, vector<pair<Point, Point*>>& Q, pair<Point, Point>& closest)
{
	RECURSIVE_CALLS++;

	// P is small enough, just bruteforce it
	if(P.size() <= 3)
	{
		return bruteForceClosestPair(P, closest);
	}
	else
	{
		// Generate the strip
		int mid = P[P.size()/2].x;

		// Points on the left of mid
		vector<Point> PL;
		for(int i = 0; i < int(P.size()/2); i++)
		{
			PL.push_back(P[i]);
		}

		// Points on the right of mid
		vector<Point> PR;
		for(int i = int(P.size()/2); i < int(P.size()); i++)
		{
			PR.push_back(P[i]);
		}

		// Copy the same points of PL and PR, but maintain the Y sort order
		vector<pair<Point, Point*>> QL;
		vector<pair<Point, Point*>> QR;
		for(auto& q : Q)
		{
			if( q.first.x < mid)
				QL.push_back(q);
			else
				QR.push_back(q);
		}

		// Find the closest pair on the left
		pair<Point, Point> cl{PL[0], PL[1]};
		double dl = divideClosetPointSearch(PL, QL, cl);

		// Find the closest pair on the right
		pair<Point, Point> cr{PR[0], PR[1]};
		double dr = divideClosetPointSearch(PR, QR, cr);

		// Find the closest of the two
		double d;
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

		// Copy all points within d of the middle into S, this forms the strips
		vector< pair<Point, Point*> > S;
		int size = 0;
		for( auto& q : Q)
		{
			if(abs(q.first.x - mid) <= d)
			{
				S.push_back(q);
				size++;
			}
		}

		// Used so we don't have to do sqrt inside the loop
		double dminsq = pow(d, 2);

		// Loop through all points inside the strip and see if any are closer then dminsq
		for( int i = 0; i < size; i++)
		{
			int k = i+1;
			while(k < size && (pow(S[i].first.y - S[k].first.y, 2) < dminsq  ))
			{
				// Running a distance calculation
				DISTANCE_CALCULATIONS += 1;
				double dist = pow(S[i].first.x - S[k].first.x, 2) + pow(S[i].first.y - S[k].first.y, 2);
				
				// Distance was less, so the points are closer across the middle than the left or right half
				if( dist < dminsq)
				{
					dminsq = dist;
					closest.first = S[i].first;
					closest.second = S[k].first;
				}

				k++;
			}
		}

		return sqrt(dminsq);
	}
}


/**
 *	@brief	Using a divide an conquer algorith, find the closest points and the distance between them.
 *	
 *	This purpose of this function is not to run the algorith but to set up the data structures
 *	that will allow it to be run more efficiently.  The points in the dataset will be first copied into
 *	a new vector, P, and merge sorted by their X component.  Thos points are then copied into a new vector,
 *	Q, which is merge sorted by the Y component.  The trick to making it work, is that the points in Q keep
 *	a pointer to their matching point in P.  This allows us to only sort the points a single time, but still
 *	correctly distribute for the algorithm.
 *	
 *	@param points		Vector of Points to find the closest pair in.
 *	@param closestPair	A copy of the two closest points will be stored in closest pair
 *	
 *	@return The Euclidean Distance between the two closest point.
 */
double divideClosestPoint( vector<Point>& points, pair<Point, Point>& closestPair )
{
	//copy points into P and sort by X
	vector< Point> P = points;
	merge_sort(P);

	
	//copy points from P into Q with a pointer to the value in P
	vector<pair<Point, Point*>> Q;
	for( int i = 0; i < int(P.size()); i++)
		Q.emplace_back(P[i], &P[i]);

	//sort Q by Y
	merge_sort( Q );
	

	// Do the actual search
	return divideClosetPointSearch(P, Q, closestPair);
}


/**
 *	@brief		Compare two strings for equality, ignoring case.
 *	
 *	@param a	String to use for comparison
 *	@param b	String to compare A to
 *	
 *	@return True is the strings are the same ignoring case, False otherwise
 */
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


/**
 *	@brief		Read in an integer from std input.
 *	
 *	@return		The integer read from input.
 */
int getNextInt()
{
	int input;

	// Continue trying to get a valid int.
	while( !( cin >> input ) )
	{
		cout << "Bad input" << endl;
		cin.clear();
		string bad;
		getline(cin, bad);
	}

	return input;
}


/**
 *	@brief		Read in a positive number from std input.
 *	
 *	@return		A positive number entered by the user through std input.
 */
unsigned getNextPos()
{
	bool valid = false;
	int input;

	// Contiue trying to get a valid pos int
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


/**
 *	@brief	Read input from the std input.  It should be "Brute", "Divide" or "Both".  Based
 *				on what was entered, an algorithm type will be selected for the program.
 *				
 *	@return Which algorithm(s) the user selected.
 */
Algorithm getAlgorithm()
{
	string algorithm;

	// Loop until we get a valid value for the algorithm type
	while( true )
	{
		cout << "Please choose an algorithm (BRUTE, DIVIDE, BOTH): ";
		getline(cin, algorithm);

		// Check which algorithm was selected, ignoring case
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


/**
 *	@brief	Run iterations of each algorithm using random points.
 *				
 *	@return Void.
 */
void runTests( int maxN = 8192, int iterations = 100)
{
	int currentN = 8;
	
	cout << "Divide and Conquer" << endl;
	while( currentN <= maxN)
	{
		int averageCalls = 0;
		int averageCalcs = 0;

		// Run divides
		for( int i = 0; i < iterations; i++)
		{
			DISTANCE_CALCULATIONS = 0;
			RECURSIVE_CALLS = 0;
			vector<Point> points;

			// Generate the Points
			for(int p = 0; p < currentN; p++)
				points.emplace_back( getRandomPoint() );

			// Run the algorithm
			pair<Point, Point> closest{points[0], points[1]};
			double distance = divideClosestPoint(points, closest);

			averageCalcs += DISTANCE_CALCULATIONS;
			averageCalls += RECURSIVE_CALLS;
		}

		averageCalls /= iterations;
		averageCalcs /= iterations;

		cout << "\tN: " << currentN << endl;
		cout << "\t\t iterations:    " << iterations << endl;
		cout << "\t\t average calls: " << averageCalls << endl;
		cout << "\t\t average calcs: " << averageCalcs << endl;

		currentN *= 2;
	}
	

	// Run brutes
	currentN = 8;
	
	cout << "Brute Force" << endl;
	while( currentN <= (maxN/2))
	{
		int averageCalls = 0;
		int averageCalcs = 0;

		// Run divides
		for( int i = 0; i < iterations; i++)
		{
			DISTANCE_CALCULATIONS = 0;
			RECURSIVE_CALLS = 0;
			vector<Point> points;

			// Generate the Points
			for(int p = 0; p < currentN; p++)
				points.emplace_back( getRandomPoint() );

			// Run the algorithm
			pair<Point, Point> closest{points[0], points[1]};
			double distance = bruteForceClosestPair(points, closest);

			averageCalcs += DISTANCE_CALCULATIONS;
		}

		averageCalcs /= iterations;

		cout << "\tN: " << currentN << endl;
		cout << "\t\t iterations:    " << iterations << endl;
		cout << "\t\t average calcs: " << averageCalcs << endl;

		currentN *= 2;
	}
}

vector<Point> points;
Algorithm selected_algorithm;

int main(int argc, char* argv[])
{
	srand(time(NULL));

	//runTests();

	// Verify we have enough arguments
	if( argc < 2)
	{
		selected_algorithm = getAlgorithm();
	}
	else
	{
		if( equalIC(argv[1], "BRUTE"))
			selected_algorithm = BRUTE;
		else if( equalIC(argv[1], "DIVIDE"))
			selected_algorithm = DIVIDE;
		else if( equalIC(argv[1], "BOTH"))
			selected_algorithm = BOTH;
		else
			selected_algorithm = getAlgorithm();
	}
	

	// Take in the number of points
	cout << "Enter point count: ";
	int count = getNextPos();


	// Loop and get the points
	for(int p = 0; p < count; p++)
	{
		int x = getNextInt();
		int y = getNextInt();

		points.emplace_back( x, y );
	}


	// Punch out the sorts
	if( points.size() >= 2)
	{
		if( selected_algorithm == DIVIDE || selected_algorithm ==  BOTH)
		{
			DISTANCE_CALCULATIONS = 0;
			cout << "Algorithm: Divide and Conquer\n\n";

			cout << "N: " << points.size() << "\n\n";

			pair<Point, Point> closest{points[0], points[1]};

			long long distance = divideClosestPoint(points, closest);
			long long ds = distance*distance;					

			cout << "Point 1: (" << closest.first.x << ", " << closest.first.y << ")\n";
			cout << "Point 2: (" << closest.second.x << ", " << closest.second.y << ")\n\n";

			cout << "Distance squared: " << ds << "\n\n";
			cout << "Distance: " << distance << "\n\n";
			cout << "Number of distance calcs: " << DISTANCE_CALCULATIONS << endl;
			cout << "Number of calls: " << RECURSIVE_CALLS << endl;
		}

		if( selected_algorithm == BOTH )
			cout << "\n\n";

		if(selected_algorithm == BRUTE || selected_algorithm == BOTH)
		{
			DISTANCE_CALCULATIONS = 0;

			cout << "Algorithm: Brute Force\n\n";

			cout << "N: " << points.size() << "\n\n";
	
			pair<Point, Point> closest{points[0], points[1]};
			long long distance = bruteForceClosestPair(points, closest);
			long long ds = distance*distance;

			cout << "Point 1: (" << closest.first.x << ", " << closest.first.y << ")\n";
			cout << "Point 2: (" << closest.second.x << ", " << closest.second.y << ")\n\n";

			cout << "Distance squared: " << ds << "\n\n";
			cout << "Distance: " << distance << "\n\n";
			cout << "Number of distance calcs: " << DISTANCE_CALCULATIONS << endl;
		}
	}
	else
	{
		cout << "Error: n = " << points.size() << ". Should be >= 2" << endl;
	}


	return 0;
}