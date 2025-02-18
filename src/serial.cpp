#include <iostream>
#include <cstdlib>
#include <ctime>
#include "miller_rabin.h"

using namespace std;

int main(int argc, char* argv[]) {
    srand(time(0));

    uint64_t start, end;
    int iterations = 100;

    // Handle command-line arguments
    if (argc == 3) {
        start = strtoull(argv[1], nullptr, 10);
        end = strtoull(argv[2], nullptr, 10);
    } else {
        cout << "Usage: " << argv[0] << " <start> <end>\n";
        return 1;
    }

    cout << "Prime numbers in range [" << start << ", " << end << "] are:\n";
    for (uint64_t num = start; num <= end; num++) {
        if (miller_rabin(num, iterations)) {
            cout << num << " ";
        }
    }
    cout << endl;

    return 0;
}