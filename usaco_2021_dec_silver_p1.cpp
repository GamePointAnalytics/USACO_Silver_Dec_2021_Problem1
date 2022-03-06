
// USACO 12/2021 Silver P1

// Divide patches into groups separated by enemy cows
// The key insight is that you only need at most two cows from John to take 
// the entire interval.
// If an interval only has one enemy cow at the end, you can take the entire
// interval with only one cow.
// If you place one cow in an interval, this cow can take patches within a 
// window of 1/2 of the length of the interval. This is a max subarray sum problem.
// You can have a two-cow list (sum of all patches in each interval) and a one
// cow list (max subarry for each interval). The question is which item to take
// from the two-cow list and which item to take from the one cow list to make the
// max sum. 

// The idea of the solution is to subtract the two cow list from one cow list.
// Start with the biggest difference, and pick the correponding two-cow interval sum.
// Subtract the max subarray sum of the corresponding interval from the one cow total sum. 
// Keep adding the items from the two cow list based on the differences, and keep removing
// the corresponding the max subarray sum from the one-cow totoal sum. In this process,
// keep track of the max sum. 

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

int main() {

	unsigned int K; // num patches
	unsigned int M; // num enemy cows
	unsigned int N; // num cows for John

	cin >> K >> M >> N;

	vector<pair<long long, long long>> patches_cows;
	long long p;
	long long t;
	for (unsigned int i = 0; i < K; i++) {
		cin >> p >> t;
		patches_cows.push_back(make_pair(p, t));
	}

	vector<long long> enemy_cows;
	long long f;
	for (unsigned int i = 0; i < M; i++) {
		cin >> f;
		patches_cows.push_back(make_pair(f, -1));
		enemy_cows.push_back(f);
	}

	// Place enemy cows and patches on the same list and sort them
	sort(patches_cows.begin(), patches_cows.end());

	// This list only contains the enemy cows
	sort(enemy_cows.begin(), enemy_cows.end());

	unsigned int cur_enemy_cow = 0;
	unsigned int cur_interval_index = 0;
	long long interval_sum = 0;
	long long max_subarray_sum = 0;
	long long subarray_sum = 0;

	long long enemy_cow_start = -1;
	int enemy_cow_start_index = -1;

	unsigned int first_patch_index = 0;

	bool calculate_max_subarray = false;
	double distance = 0.0;

	vector<pair<long long, unsigned int>> two_cow_list;
	vector<pair<long long, unsigned int>> one_cow_list;

	// Go through the sorted list
	for (unsigned int i = 0; i < patches_cows.size(); i++) {
		if (patches_cows[i].second < 0) {
			// This is a cow
			if (interval_sum > 0) {
				if (enemy_cow_start >= 0) {
					// There is a start cow 

					// push interval sum and interval index to 2 cow array
					two_cow_list.push_back(make_pair(interval_sum, cur_interval_index));

					// push max subarray sum to one cow array
					one_cow_list.push_back(make_pair(max_subarray_sum, cur_interval_index));
				}
				else {
					// There is no starting cow 
					two_cow_list.push_back(make_pair(interval_sum, cur_interval_index));
					// Also push it to one cow array because max subarray sum 
					// is the same as interval sum
					one_cow_list.push_back(make_pair(interval_sum, cur_interval_index));
				}

				// Start a new interval
				interval_sum = 0;
				max_subarray_sum = 0;
				subarray_sum = 0;
				cur_interval_index++;
			}

			enemy_cow_start = patches_cows[i].first;
			enemy_cow_start_index++;

			if ((enemy_cow_start_index + 1) < enemy_cows.size()) {
				// If there is a start cow and end cow, calculate max subarray sum.
				// Otherwise, no need. 
				calculate_max_subarray = true;

				// Calculate the length of this interval
				distance = ((double)(enemy_cows[enemy_cow_start_index + 1] - enemy_cows[enemy_cow_start_index]) / 2.0);
			}
			else {
				calculate_max_subarray = false;
			}
		}
		else {
			// This is a patch. Add it to the interval sum
			interval_sum += patches_cows[i].second;

			if (calculate_max_subarray) {
				if ((max_subarray_sum == 0) && (subarray_sum == 0)) {
					// This is the first patch in the interval
					first_patch_index = i;
				}

				// If the distance between the current patch and the first patch in the interval 
				// is larger than the window (1/2 of the interval length), keep removing the first patch 
				// until the first patch is within the window
				while ((double)(patches_cows[i].first - patches_cows[first_patch_index].first) >= distance) {
					// Out of window. Remove the patch from the subarray sum
					subarray_sum -= patches_cows[first_patch_index].second;
					first_patch_index++;
				}

				// This patch is in the "window"
				subarray_sum += patches_cows[i].second;

				// Keep track of the current max subarray sum
				max_subarray_sum = max(max_subarray_sum, subarray_sum);
			}
		}
	}

	// Handle the last interval
	if (interval_sum > 0) {
		// No cow at the end
		two_cow_list.push_back(make_pair(interval_sum, cur_interval_index));
		one_cow_list.push_back(make_pair(interval_sum, cur_interval_index));
	}

	vector<pair<long long, unsigned int>> differences;

	// Calculate the difference between the two-cow and one-cow list.
	for (unsigned int i = 0; i < two_cow_list.size(); i++) {
		differences.push_back(make_pair((two_cow_list[i].first - one_cow_list[i].first),
			two_cow_list[i].second));
	}

	// Sort in ascending order
	sort(differences.begin(), differences.end());
	sort(one_cow_list.begin(), one_cow_list.end());

	// Create two new lists so we can quickly access tasty indices
	long long* one_cow_taste_by_index = new long long[one_cow_list.size()]();
	long long* two_cow_taste_by_index = new long long[two_cow_list.size()]();

	for (auto i : one_cow_list) {
		one_cow_taste_by_index[i.second] = i.first;
	}

	for (auto i : two_cow_list) {
		two_cow_taste_by_index[i.second] = i.first;
	}

	long long max_sum = 0;

	set<unsigned int> visited_one_cow_index;

	unsigned int min_one_cow_index = 0;
	unsigned int one_cow_lowerbound = 0;
	long long one_cow_sum = 0;

	// Check if N (number of cows) is larger than the one cow list.
	if (one_cow_list.size() <= N) {
		one_cow_lowerbound = 0;
	}
	else {
		one_cow_lowerbound = one_cow_list.size() - N;
	}

	// This is the case when we place at most one cow per interval.
	// We don't place two cows in any interval.
	for (int i = (one_cow_list.size() - 1); i >= one_cow_lowerbound; i--) {
		one_cow_sum += one_cow_list[i].first;
		min_one_cow_index = i;
		visited_one_cow_index.insert(one_cow_list[i].second);
	}

	max_sum = one_cow_sum;

	long long two_cow_sum = 0;
	unsigned int count = 2;
	unsigned int step = 0;
	
	for (int i = differences.size() - 1; i >= 0; i--) {

		// Start from the biggest difference

		if ((N - count) <= 1) {
			// If we have 0 or 1 cow left, stop.
			break;
		}

		// Add the tasty index based on the largest difference.
		two_cow_sum += two_cow_taste_by_index[differences[i].second];

		if (visited_one_cow_index.find(differences[i].second) != visited_one_cow_index.end()) {
			// If the patch added to the two cow sum is already selected for the one-cow sum
			// remove it from the one-cow sum
			one_cow_sum -= one_cow_taste_by_index[differences[i].second];

			if (differences[i].second == one_cow_list[min_one_cow_index].second) {
				// If this happens to be the last item
				min_one_cow_index++;
			}

			// Erase the index of this patch from visited list
			visited_one_cow_index.erase(differences[i].second);
		}
		else {
			// If the patch added to the two cow list is not in the one cow sum
			// delete the smallest item
			one_cow_sum -= one_cow_list[min_one_cow_index].first;
			visited_one_cow_index.erase(one_cow_list[min_one_cow_index].second);
			min_one_cow_index++;
		}

		// We have to delete one smallest item anyway because the two cow item
		// takes two cows. We need to remove two cows from the one cow sum.
		one_cow_sum -= one_cow_list[min_one_cow_index].first;
		visited_one_cow_index.erase(one_cow_list[min_one_cow_index].second);
		min_one_cow_index++;

		// Track the current max value
		max_sum = max(max_sum, (two_cow_sum + one_cow_sum));

		// Two cows are used
		count += 2;
	}

	cout << max_sum;

	delete[] one_cow_taste_by_index;
	delete[] two_cow_taste_by_index;

	return(0);
}