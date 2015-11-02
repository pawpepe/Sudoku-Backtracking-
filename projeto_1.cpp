/* Amanda Lutzenberger Ruiz (8532227)
 * Anna Paula Pawlicka Maule (4624650)
 *
 * Projeto 1: Backtracking simples e com euristicas, aplicado na resolução de Sudoku
 *
 */

#include <iostream>
#include <list>
#include <cstdlib>
#include <cstdio>

using namespace std;

void print_result(int **grid);
void backtracking(int **grid);
void FV_backtracking(int **grid);
void MRV_FV_backtracking(int **grid);

int main(int argc, char **argv) {
    int **grid, cases, bt_type = 1;

    grid = (int**)malloc(9*sizeof(int*));
    for (int i = 0; i < 9; i++) grid[i] = (int*)malloc(9*sizeof(int));

    std::cin >> cases;

    for (int k = 0; k < cases; k++) {

        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                std::cin >> grid[i][j];
            }
        }

        switch(bt_type) {
            case 1: // Simple backtracking
                backtracking(grid);
                break;
            case 2:
                FV_backtracking(grid);
                break;
            case 3:
                MRV_FV_backtracking(grid);
                break;
        }

        // Print the resolution of the sudoku and the number of positions filled
        print_result(grid);
    }

    return 0;
}


 /**********************
  * OTHERS FUNCTIONS *
  **********************/

void print_result(int **grid) {

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}


 /*************************
 * BACKTRACKING FUNCTIONS *
 **************************/

int verify_possibility(int num, int x, int y, int **grid) {

    for (int i = 0; i < 9; i++) {
        // If there's already "num" in the current row/column, returns INVALID
        if (grid[x][i] == num || grid[i][y] == num) return 0;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            // If there's already "num" in the current 3x3 grid, returns INVALID
            if (grid[3*(x/3) + i][3*(y/3) + j] == num) return 0;
        }
    }
    // Else, returns VALID
    return 1;
}

// Grid is the sudoku map, pos is the position counter
int recur_backtracking(int **grid, int pos) {
    // Calculate (x,y) "coordinates" based on position counter
    int x = pos/9, y = pos%9;

    // If all positions are filled, return SUCCESS! (stop condition)
    if (pos == 81) return 1;
    // If the current position is a fixed number, recall function for the next one
    if (grid[x][y] != 0) return recur_backtracking(grid, pos+1);

    // Try from 1 to 9 the possibilities to fill the current position
    for (int i = 1; i <= 9; i++) {
        // If "i" is a valid number to fill it...
        if (verify_possibility(i, x, y, grid)) {
            grid[x][y] = i; // Write it in the grid

            // and recall function for the next position...
            if (recur_backtracking(grid, pos+1))
                return 1; // If it receives SUCCESS, return SUCCESS!

            grid[x][y] = 0;  // Else, fill with zero again
        }
    }
    // If all the number resulted in invalid or FAIL, return FAIL
    return 0;
}

// Backtracking algorithm without euristics...
void backtracking(int **grid) {
    // It just calls the recursive funcion, starting the position counter in 0
    if (recur_backtracking(grid, 0) == 0)
        cout << "\nSolution not found or a erros has happend" << endl;
}

/******************************
 * FV-BACKTRACKING FUNCTIONS  *
 ******************************/


// Get all filling possibilities of (x,y)
void create_FV_map(int x, int y, int **grid, list<int> &aux) {

    for (int i = 1; i <= 9; i++) aux.push_back(i);

    for (int i = 0; i < 9; i++) {
        if (grid[x][i] != 0) aux.remove(grid[x][i]);
        if (grid[i][y] != 0) aux.remove(grid[i][y]);
    }


    int u = 3*(x/3), v = 3*(y/3);
    for (int i = u; i < (u + 3); i++) {
        for (int j = v; j < (v + 3); j++) {
             if (grid[i][j] != 0) aux.remove(grid[i][j]);
         }
    }
}

// Remove "num" of possibilities list of (x,y) row, column and sub-grid
// Returns positions that has been modified (for backup in backtracking case)
list<int> update_FV_map(int num, int x, int y, int **grid, list<int> *aux) {
    list<int> backup;

    for (int i = 0; i < 9; i++) {
        if (grid[x][i] == 0) {
            int size = aux[9*x + i].size();
            aux[9*x + i].remove(num);
            if (size != aux[9*x + i].size()) backup.push_back(9*x + i);
        }
        if (grid[i][y] == 0) {
            int size = aux[9*i + y].size();
            aux[9*i + y].remove(num);
            if (size != aux[9*i + y].size()) backup.push_back(9*i + y);
        }
    }


    int u = 3*(x/3), v = 3*(y/3);
    for (int i = u; i < (u + 3); i++) {
        for (int j = v; j < (v + 3); j++) {
            if (grid[i][j] == 0) {
                int size = aux[9*i + j].size();
                aux[9*i + j].remove(num);
                if (size != aux[9*i + j].size()) backup.push_back(9*i + j);
            }
        }
    }
    backup.sort();
    backup.unique();
    return backup;
}


// Verify for the current column, row and sub-grid if there's a position that
// the only possibility is "num". If that's the case, FV signals FAIL.
int verify_FV_map(int num, int x, int y, int **grid, list<int> *aux) {

    for (int i = 0; i < 9; i++) {
        int pos = i*9 + y;
        if (grid[i][y] == 0 && aux[pos].size() == 1 && aux[pos].front() == num )
            return 0;

        pos = x*9 + i;
        if (grid[x][i] == 0 && aux[pos].size() == 1 && aux[pos].front() == num )
             return 0;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int u = 3*(x/3) + i, v = 3*(y/3) + j;
            int pos = u*9 + v;
            if (grid[u][v] == 0 && aux[pos].size() == 1 && aux[pos].front() == num )
                return 0;
        }
    }

    return 1; // Else returns 1 (FV signals OK)
}


int recur_FV_backtracking(int **grid, int pos, list<int> *aux) {

    int x = pos/9, y = pos%9;

    // If all positions are filled, return SUCCESS! (stop condition)
    if (pos == 81) return 1;
    // If the current position is a fixed number, recall function for the next one
    if (grid[x][y] != 0) return recur_FV_backtracking(grid, pos+1, aux);

    int *b = (int*)calloc(9,sizeof(int));

    // Try all filling possibilities of the current position
    while (!aux[pos].empty()) {
        int num = aux[pos].front();
        aux[pos].pop_front();
        b[num-1] = 1;

        grid[x][y] = num; // Write it in the grid...

        // If there's no position without options after the filling above
        /** it Checks if any position is without options**/
        if (verify_FV_map(num, x, y, grid, aux)){
             // Update possibility lists and save the lists modifieds...
            list<int> backup = update_FV_map(num, x, y, grid, aux);

            // And recall function for the next position...
            if (recur_FV_backtracking(grid, pos+1, aux))
                return 1; // If it receives SUCCESS, return SUCCESS!

            // Else fill with zero again and recover lists
            grid[x][y] = 0;
            while (!backup.empty()) {
                int backup_pos = backup.front();
                backup.pop_front();
                aux[backup_pos].push_back(num);
            }
        }
        else grid[x][y] = 0;
    }

    for (int i = 0; i < 9; i++) {
        if (b[i] == 1) aux[pos].push_back(i+1);
    }
    // If all the number resulted in invalid or FAIL, return FAIL
    return 0;
}

// Backtracking with Forward verification Heuristic
void FV_backtracking(int **grid) {

    list<int> aux[81]; // List of filling possibilities

    // Initial state
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            // Enlist any number that can be placed in the (x,y) grid's position
            if (grid[i][j] == 0) {
                for (int num = 1; num <= 9; num++) {
                    create_FV_map(i, j, grid, aux[i*9 + j]);
                }
            }
        }
    }

    if (recur_FV_backtracking(grid, 0, aux) == 0)
        cout << "\nSolution not found or a error has happened" << endl;
}




/******************************
 * MRV-FV-BACKTRACKING FUNCTIONS *
 ******************************/

// Get position which has the lowest filling possibilities
int get_MRV(int *aux_grid, int **grid) {
    int pos = -1, min = 10;

    for (int i = 0; i < 81; i++) {
        if (grid[i/9][i%9] == 0 && aux_grid[i] < min) {
            min = aux_grid[i];
            pos = i;
        }
    }
    if (min == 0) return -2; // if there's no options to a free position
    return pos; // It'll return -1 when completed
}

int recur_MRV_FV_backtracking(int **grid, int *aux_grid, list<int> *aux) {

    int pos, x, y;
    // If all positions are filled, return SUCCESS! (stop condition)
    pos = get_MRV(aux_grid, grid);

    if (pos == -1) return 1;
    else if (pos == -2 ) return 0;

    x = pos/9; y = pos%9;


    int *b = (int*)calloc(9,sizeof(int));

    // Try all filling possibilities of the current position
    while (!aux[pos].empty()) {
        int num = aux[pos].front();
        aux[pos].pop_front();
        aux_grid[pos]--;
        b[num-1] = 1;

        grid[x][y] = num; // Write it in the grid...

        // If there's no position without options after the filling above
        /** it Checks if any position is without options**/
        if (verify_FV_map(num, x, y, grid, aux)) {

            // Update MRV auxiliar and save updates positions...
            list<int> backup = update_FV_map(num, x, y, grid, aux);

            for (int i = 0; i < backup.size(); i++) {
                int backup_pos = backup.front();
                backup.pop_front();
                aux_grid[backup_pos]--;
                backup.push_back(backup_pos);
            }

            // And recall function for the next position...
            if (recur_MRV_FV_backtracking(grid, aux_grid, aux))
                    return 1; // If it receives SUCCESS, return SUCCESS!

            // Else fill with zero again and recover MRV auxiliars
            grid[x][y] = 0;
            while (!backup.empty()) {
    			int backup_pos = backup.front();
    			backup.pop_front();
    			aux[backup_pos].push_back(num);
                aux_grid[backup_pos]++;
            }
        }
        else grid[x][y] = 0;
    }

    for (int i = 0; i < 9; i++) {
        if (b[i] == 1) {
            aux[pos].push_back(i+1);
            aux_grid[pos]++;
        }
    }
    // If all the number resulted in invalid or FAIL, return FAIL
    return 0;
}


// Backtracking with Minimum Remaining Value heuristic
void MRV_FV_backtracking(int **grid) {
    list<int> aux[81]; // Possibilities list
	int *aux_grid = (int*)malloc(81*sizeof(int));

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] == 0) {
                create_FV_map(i, j, grid, aux[9*i + j]);
                aux_grid[9*i + j] = aux[9*i + j].size();
            }
            else aux_grid[9*i + j] = 0;
        }
    }

    if (recur_MRV_FV_backtracking(grid, aux_grid, aux) == 0)
        cout << "\nSolution not found or a error has happened" << endl;
}
