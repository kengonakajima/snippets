#include <vector>
#
#include <iostream>
#include <iterator>

struct goukakusya {
        int number;
        int score;
        goukakusya(int n, int s) : number(n), score(s) {}
};

inline bool operator<(const goukakusya& x, const goukakusya& y) {
        return x.score < y.score;
}

std::ostream& operator<<(std::ostream& stream, const goukakusya& g) {
        return stream << g.number << ':' << g.score;
}

int main() {
        std::vector<goukakusya> table;
        table.push_back(goukakusya(1,65));
        table.push_back(goukakusya(2,92));
        table.push_back(goukakusya(3,95));

        std::cout << "before sort\n";
        std::copy(table.begin(), table.end(),
                  std::ostream_iterator<goukakusya>(std::cout,"\n"));
        std::cout << std::endl;

        std::sort(table.begin(), table.end());

        std::cout << "after sort\n";
        std::copy(table.begin(), table.end(),
                  std::ostream_iterator<goukakusya>(std::cout,"\n"));
        std::cout << std::endl;

        return 0;
}
