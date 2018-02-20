#include "utility.h"

void InitializePascalsTri(int numberOfRows, std::vector<std::vector<unsigned int>>& pascalsTriangle)
{
    // Initialize pascals triangle
    pascalsTriangle.resize(numberOfRows);
    for (int i = 0; i < numberOfRows; i++)
        pascalsTriangle[i].resize(numberOfRows);
    pascalsTriangle[0][0] = 1;

    // complete rows of triangle
    for (int r = 2; r <= numberOfRows; r++)
    {
        // pad row with ones
        pascalsTriangle[r - 1][0] = 1;
        pascalsTriangle[r - 1][r - 1] = 1;

        // fill in colums in between the ones
        for (int j = 0; j <= r - 3; j++)
        {
            pascalsTriangle[r - 1][j + 1] =
                pascalsTriangle[r - 2][j] + pascalsTriangle[r - 2][j + 1];
        }
    }
}
