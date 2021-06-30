#include <iostream>
#include <vector>
#include <bitset>

using namespace std;

using grid = vector<vector<char>>;
grid solve(const grid& unsolved)
{
    grid candidate(unsolved);
    auto checkrow = [&cand=candidate](int row)
    {
        bitset<9> b; 
        for (size_t col = 0; col < cand.front().size(); ++col)
            if (cand[row][col] == '.') continue;
            else if (b[cand[row][col] - '1']) return false;
            else b.set(cand[row][col] - '1');
        return true;
    };
    auto checkcol = [&cand = candidate](int col)
    {
        bitset<9> b;
        for (size_t row = 0; row < cand.size(); ++row)
            if (cand[row][col] == '.') continue;
            else if (b[cand[row][col] - '1']) return false;
            else b.set(cand[row][col] - '1');
        return true;
    };
    auto checkbox = [&cand = candidate](int row, int col)
    {
        bitset<9> b;
        row /= 3; row *= 3;
        col /= 3; col *= 3;
        for (size_t index = 0; index < cand.size(); ++index)
            if (cand[row + index / 3][col + index % 3] == '.') continue;
            else if (b[cand[row + index / 3][col + index % 3] - '1']) return false;
            else b.set(cand[row + index / 3][col + index % 3] - '1');
        return true;
    };
    int row = 0;
    int col = 0;
    while (candidate[row][col] - '1' < 9 || candidate[row][col] == '.')
    {

        if (candidate[row][col] == '.')
            candidate[row][col] = '1';
        else
        {
            if (candidate[row][col] == unsolved[row][col])
            {
                //already filled, advance current cell
                ++col;
                if (col > 8)
                {
                    col = 0;
                    ++row;
                }
                if (row > 8) break;
                continue;
            }
        }

        //try current value
        if (checkrow(row) && checkcol(col) && checkbox(row, col))
        {
            //advance
            ++col;
            if (col > 8)
            {
                col = 0;
                ++row;
            }
            if (row > 8) break;
            continue;
        }

        while (candidate[row][col] == unsolved[row][col] || ++candidate[row][col] - '1' > 8)
        {
            if (candidate[row][col] != unsolved[row][col])
                candidate[row][col] = '.';
            if (col < 1)
            {
                --row;
                col = 8;
            }
            else --col;
            if (row < 0) return unsolved;
        }
    }
    return candidate;
}

ostream& operator<<(ostream& s, const grid& g)
{
    int x = 0;
    for (const auto& v : g)
    {
        for (const auto& c : v)
        {
            s << c << " ";
            if (++x % 3 == 0) s << '\t';
        }
        s << endl;
        if (x % 27 == 0) s << endl;
    }
    s << endl;
    return s;
}

int main()
{
    grid puz1 =
    {
        vector<char>{'.','.','.',   '.','5','.',    '.','.','8'},
        vector<char>{'.','.','.',   '.','.','7',    '5','6','.'},
        vector<char>{'4','2','.',   '.','.','.',    '.','.','.'},

        vector<char>{'.','.','.',   '.','.','.',    '1','.','.'},
        vector<char>{'.','5','.',   '2','.','6',    '.','.','.'},
        vector<char>{'.','.','1',   '.','8','3',    '.','2','4'},

        vector<char>{'3','7','.',   '4','9','5',    '.','.','.'},
        vector<char>{'.','.','.',   '.','.','.',    '.','.','.'},
        vector<char>{'6','.','.',   '.','.','.',    '3','.','.'},
    };
    
    size_t row = 0;
    size_t col = 0;
    char c;
    while(cin >> c)
    {
        puz1[row][col] = c;
        ++col;
        if (col > 8)
        {
            col = 0;
            ++row;
        }
        if (row > 8) break;
    }

    cout << puz1 << endl << endl << solve(puz1);
    return 0;
}

