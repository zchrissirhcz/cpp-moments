#include "gtest/gtest.h"

// https://www.geeksforgeeks.org/wildcard-pattern-matching/
// Function that matches input str with given wildcard pattern
// Time complexity: O(m x n).
// Auxiliary space: O(m).
bool strmatch(std::string str, std::string pattern)
{
    const int m = str.length();
    const int n = pattern.length();
    // lookup table for storing results of
    // subproblems
    std::vector<bool> prev(m + 1, false), curr(m + 1, false);

    // empty pattern can match with empty string
    prev[0] = true;

    // fill the table in bottom-up fashion
    for (int i = 1; i <= n; i++)
    {
        bool flag = true;
        for (int ii = 1; ii < i; ii++)
        {
            if (pattern[ii - 1] != '*')
            {
                flag = false;
                break;
            }
        }
        curr[0] = flag; // for every row we are assigning
                        // 0th column value.
        for (int j = 1; j <= m; j++)
        {
            // Two cases if we see a '*'
            // a) We ignore ‘*’ character and move
            // to next character in the pattern,
            //  i.e., ‘*’ indicates an empty sequence.
            // b) '*' character matches with ith
            //  character in input
            if (pattern[i - 1] == '*')
            {
                curr[j] = curr[j - 1] || prev[j];
            }

            // Current characters are considered as
            // matching in two cases
            // (a) current character of pattern is '?'
            // (b) characters actually match
            else if (pattern[i - 1] == '?' || str[j - 1] == pattern[i - 1])
            {
                curr[j] = prev[j - 1];
            }

            // If characters don't match
            else
            {
                curr[j] = false;
            }
        }
        prev = curr;
    }

    return prev[m];
}

std::vector<std::string> qmatch(std::vector<std::string>& test_case_names, std::string filter)
{
    std::vector<std::string> res;
    for (int i = 0; i < test_case_names.size(); i++)
    {
        if (strmatch(test_case_names[i], filter))
        {
            res.push_back(test_case_names[i]);
        }
    }
    return res;
}

TEST(c, 1)
{
    EXPECT_EQ(1, 1);
    std::vector<int> va = {1, 2, 3};
    std::vector<int> vb = {1, 2, 3};
    EXPECT_EQ(va, vb);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}