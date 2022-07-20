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


HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO console_info;


struct Size { int w, h; };
struct Coord { int x, y; };

int* get_console_size();
void gotoxy(int x, int y);
void print(bool** field, Size field_size);
int count_neighbors(bool** field, Coord cell_coord, Size field_size) throw(std::out_of_range);


int main()
{
    bool** field;   //Russian field of experiments
    bool** field_b; //бyфер 
    Size size;      //размер поля


    //ввод размеров поля
    while (true) {
        std::cout << "enter width: ";
        std::cin >> size.w;
        if (!std::cin.fail()) {
            int *console_size;
            if ((console_size = get_console_size()) != nullptr) {
                if (size.w > console_size[0] - 2) {
                    std::cout << "field size(" 
                        << size.w 
                        << ") must not exceed console size(" 
                        << console_size[0] - 2 << ")\n";
                    continue;
                }
            }
            
            break;
        }
        std::cin.clear();
        std::cin.ignore(32767, '\n');
    }
    std::cin.ignore(32767, '\n');


    while (true) {
        std::cout << "enter heigth: ";
        std::cin >> size.h;
        if (!std::cin.fail()) {
            int* console_size;
            if ((console_size = get_console_size()) != nullptr) {
                if (size.h > console_size[1] - 2) {
                    std::cout << "field size(" 
                        << size.h 
                        << ") must not exceed console size(" 
                        << console_size[1] - 2 
                        << ")\n";
                    continue;
                }
            }
            break;
        }
        std::cin.clear();
        std::cin.ignore(32767, '\n');
    }
    std::cin.ignore(32767, '\n');


    //бортики
    //size.w += 2;
    //size.h += 2;


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


    system("cls");


    //запонение 
    for (int i = 0; i < size.h; i++)
        for (int j = 0; j < size.w; j++)
            field[i][j] = false;


    //draw frame
    std::cout << "+";
    for (int i = 0; i < size.w; i++)
        std::cout << "-";
    std::cout << "+";

    for (int i = 0; i < size.h; i++) {
        gotoxy(0, i+1);
        std::cout << "|";
        gotoxy(size.w + 1, i+1);
        std::cout << "|";
    }
    std::cout << "\n";

    std::cout << "+";
    for (int i = 0; i < size.w; i++)
        std::cout << "-";
    std::cout << "+";


    //set points
    Coord coords {0, 0};
    gotoxy(1, 1);
    while (true) {
        char pressed_key = getch();
        switch (pressed_key)
        {
        case VK_RIGHT:
            if (coords.x == size.w-1)
                coords.x = 0;
            else
                coords.x++;
            break;
        case VK_LEFT:
            if (coords.x == 0)
                coords.x = size.w - 1;
            else
                coords.x--;
            break;
        case VK_UP:
            if (coords.y == 0)
                coords.y = size.h - 1;
            else
                coords.y--;
            break;
        case VK_DOWN:
            if (coords.y == size.h - 1)
                coords.y = 0;
            else 
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
        gotoxy(coords.x + 1, coords.y + 1);
    }
    
    start:;

    //основной цикл
    while (true) {
        //перебор измененных значений клеток в буфер
        for (int i = 0; i < size.h; i++) {
            for (int j = 0; j < size.w; j++) {
                field_b[i][j] = false;
                int count = count_neighbors(field, Coord(j, i), size);
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
        for (int i = 0; i < size.h; i++) {
            for (int j = 0; j < size.w; j++) {
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


int* get_console_size() {
    if (GetConsoleScreenBufferInfo(console, &console_info))
    {
        int widht = console_info.srWindow.Right - console_info.srWindow.Left + 1;
        int height = console_info.srWindow.Bottom - console_info.srWindow.Top + 1;
        return new int[] {widht, height};
    }
    return nullptr;
}


//перемещает курсор
void gotoxy(int x, int y)
{
    COORD position;
    position.X = x;
    position.Y = y;
    SetConsoleCursorPosition(console, position);
}


//вывод поля
void print(bool** field, Size field_size) {
    gotoxy(1, 1);
    for (int i = 0; i < field_size.h; i++) {
        for (int j = 0; j < field_size.w; j++) {
            std::cout << (field[i][j] ? ALIVE_CELL : DEAD_CELL);
        }
        gotoxy(1, i + 1);
    }
}


//считает количество соседних живых клеток
int count_neighbors(bool** field, Coord cell_coord, Size field_size) throw(std::out_of_range) {
    //проверка диапазона 
    if (cell_coord.x < 0 || cell_coord.x >= field_size.w)
        throw std::out_of_range("x=" + cell_coord.x);
    if (cell_coord.y < 0 || cell_coord.y >= field_size.h)
        throw std::out_of_range("y=" + cell_coord.y);;

    int count = 0;


    bool x_min = cell_coord.x == 0;
    bool x_max = cell_coord.x == field_size.w - 1;
    bool y_min = cell_coord.y == 0;
    bool y_max = cell_coord.y == field_size.h - 1;

    //хочется плакать 
    try {
        if (field[y_max ? (0) : (cell_coord.y + 1)][x_min ? (field_size.w - 1) : (cell_coord.x - 1)])
            count++;
        if (field[y_max ? (0) : (cell_coord.y + 1)][cell_coord.x])
            count++;
        if (field[y_max ? (0) : (cell_coord.y + 1)][x_max ? (0) : (cell_coord.x + 1)])
            count++;


        if (field[cell_coord.y][x_min ? (field_size.w - 1) : (cell_coord.x - 1)])
            count++;
        if (field[cell_coord.y][x_max ? (0) : (cell_coord.x + 1)])
            count++;

        if (field[y_min ? (field_size.h - 1) : (cell_coord.y - 1)][x_min ? (field_size.w - 1) : (cell_coord.x - 1)])
            count++;
        if (field[y_min ? (field_size.h - 1) : (cell_coord.y - 1)][cell_coord.x])
            count++;
        if (field[y_min ? (field_size.h - 1) : (cell_coord.y - 1)][x_max ? (0) : (cell_coord.x + 1)])
            count++;
    }
    catch (std::exception& ex) {
    }

    return count;
}