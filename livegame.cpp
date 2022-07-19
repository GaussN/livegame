#include <Windows.h>
#include <iostream>
#include <conio.h>


#define ALIVE_CELL "*"
#define DEAD_CELL " "

#define MOVE_SPEED 300

#define VK_RIGHT 77
#define VK_LEFT 75
#define VK_UP 72
#define VK_DOWN 80


struct Size { int w, h; };


struct Coord { int x, y; };


void gotoxy(int x, int y)
{
    COORD position;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    position.X = x;
    position.Y = y;
    SetConsoleCursorPosition(hConsole, position);
}


void print(bool** field, Size field_size) {
    gotoxy(1, 1);
    for (int i = 1; i < field_size.h - 1; i++) {
        for (int j = 1; j < field_size.w - 1; j++) {
            std::cout << (field[i][j] ? ALIVE_CELL : DEAD_CELL);
        }
        gotoxy(1, i+1);
    }
}


int count_nighbor(bool** field, Coord cell_coord, Size field_size) throw(std::out_of_range) {
    if (cell_coord.x < 1 || cell_coord.x > field_size.w - 1)
        throw std::out_of_range("x=" + cell_coord.x);
    if (cell_coord.y < 1 || cell_coord.y > field_size.h - 1)
        throw std::out_of_range("y=" + cell_coord.y);;

    int count = 0;
    
    //пиздец
    if (field[cell_coord.y + 1][cell_coord.x-1])
        count++;
    if (field[cell_coord.y + 1][cell_coord.x])
        count++;
    if (field[cell_coord.y + 1][cell_coord.x+1])
        count++;


    if (field[cell_coord.y][cell_coord.x - 1])
        count++;
    if (field[cell_coord.y][cell_coord.x + 1])
        count++;

    if (field[cell_coord.y - 1][cell_coord.x - 1])
        count++;
    if (field[cell_coord.y - 1][cell_coord.x])
        count++;
    if (field[cell_coord.y - 1][cell_coord.x + 1])
        count++;

    return count;
}


int main()
{
    bool** field;//Russian field of experiments
    bool** field_b;
    Size size;

    //ввод размеров поля
    while (true) {
        std::cout << "enter width: ";
        std::cin >> size.w;
        if (!std::cin.fail())
            break;
        std::cin.clear();
        std::cin.ignore(32767, '\n');
    }
    std::cin.ignore(32767, '\n');

    while (true) {
        std::cout << "enter heigth: ";
        std::cin >> size.h;
        if (!std::cin.fail())
            break;
        std::cin.clear();
        std::cin.ignore(32767, '\n');
    }
    std::cin.ignore(32767, '\n');

    //бортики
    size.w += 2;
    size.h += 2;

    try {
        field = new bool* [size.h];
        field_b = new bool* [size.h];
        for (int i = 0; i < size.h; i++) {
            field[i] = new bool[size.w];
            field_b[i] = new bool[size.w];
        }
    }
    catch (std::bad_alloc) {
        std::cout << "bad alloc";
        return EXIT_FAILURE;
    }
    catch (std::exception) {
        std::cout << "exception";
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cout << "...";
        return EXIT_FAILURE;
    }

    system("cls | clear");

    //запонение 
    for (int i = 0; i < size.h; i++)
        for (int j = 0; j < size.w; j++)
            field[i][j] = false;

    //draw frame
    std::cout << "+";
    for (int i = 1; i < size.w - 1; i++)
        std::cout << "-";
    std::cout << "+";

    for (int i = 1; i < size.h - 1; i++) {
        gotoxy(0, i);
        std::cout << "|";
        gotoxy(size.w-1, i);
        std::cout << "|";
    }
    std::cout << "\n";

    std::cout << "+";
    for (int i = 1; i < size.w - 1; i++)
        std::cout << "-";
    std::cout << "+";

    //set points
    /*
    size.w - 2      -1 тк (w-1) - индекс последнего элемента
                    -1 тк есть бортик

    coords.x == 1   ==1 тк бортик
    */
    Coord coords {1, 1};
    gotoxy(coords.x, coords.y);
    while (true) {
        char pressed_key = getch();
        switch (pressed_key)
        {
        case VK_RIGHT:
            if (coords.x == size.w - 2)
                coords.x = 0;
            coords.x++;
            break;
        case VK_LEFT:
            if (coords.x == 1)
                coords.x = size.w - 1;
            coords.x--;
            break;
        case VK_UP:
            if (coords.y == 1)
                coords.y = size.h - 1;
            coords.y--;
            break;
        case VK_DOWN:
            if (coords.y == size.h - 2)
                coords.y = 0;
            coords.y++;
            break;
        case VK_RETURN:
            if (field[coords.y][coords.x]) {
                std::cout << DEAD_CELL;
                field[coords.y][coords.x] = false;
                break;
            }
            std::cout << ALIVE_CELL;
            field[coords.y][coords.x] = true;
            break;
        case VK_ESCAPE:
            goto start;
            break;
        default:
            break;
        }
        gotoxy(coords.x, coords.y);
    }
    
    start:;

    while (true) {
        //перебор измененных значений клеток в буфер
        //gotoxy(0, size.w+1);
        for (int i = 1; i < size.h-1; i++) {
            for (int j = 1; j < size.w-1; j++) {
                field_b[i][j] = false;
                int count = count_nighbor(field, Coord(j, i), size);
                if (!field[i][j] && (3 == count)) {
                    field_b[i][j] = true;
                    continue;
                }
                if (field[i][j] && (count < 2 || count > 3)) {
                    field_b[i][j] = false;
                    continue;
                }
                field_b[i][j] = field[i][j];
            }
        }


        //copy field_b -> field
        for (int i = 1; i < size.h - 1; i++) {
            for (int j = 1; j < size.w - 1; j++) {
                field[i][j] = field_b[i][j];
            }
        }

        print(field, size);
        Sleep(MOVE_SPEED);
    }
    


            
    for (int i = 0; i < size.h; i++) {
        delete[] field[i];
    }
    delete[] field;

    return EXIT_SUCCESS;
}