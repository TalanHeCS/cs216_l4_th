#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

// one creature record
struct Creature
{
    string name;
    string type;
    int level;
    int strength;
};

// menu choices
enum MainMenu
{
    PRINT_LIST = 1,
    SORT_LIST,
    SEARCH_LIST,
    QUIT_APP
};

enum SortMenu
{
    SORT_BY_NAME = 1,
    SORT_BY_TYPE,
    SORT_BY_LEVEL,
    SORT_BY_STRENGTH,
    SORT_BACK
};

// changes a string to lower case
string makeLower(string s)
{
    for (int i = 0; i < (int)s.length(); i++)
    {
        if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] = s[i] - 'A' + 'a';
    }
    return s;
}

// prints the table header
void printHeader()
{
    cout << left << setw(15) << "name";
    cout << left << setw(15) << "type";
    cout << right << setw(10) << "level";
    cout << right << setw(12) << "strength" << endl;

    cout << "--------------------------------------------------------" << endl;
}

// prints all creatures in the array
void printCreatures(Creature **arr, int size)
{
    if (size == 0)
    {
        cout << "no records found." << endl;
        return;
    }

    printHeader();

    for (int i = 0; i < size; i++)
    {
        cout << left << setw(15) << arr[i]->name;
        cout << left << setw(15) << arr[i]->type;
        cout << right << setw(10) << arr[i]->level;
        cout << right << setw(12) << arr[i]->strength << endl;
    }
}

// makes a copy of the pointer array
Creature **copyPointerArray(Creature **arr, int size)
{
    Creature **copy = new Creature *[size];

    for (int i = 0; i < size; i++)
        copy[i] = arr[i];

    return copy;
}

// loads creatures from a file of unknown length
void loadCreatures(const string &filename, Creature **&arr, int &size)
{
    ifstream fin(filename.c_str());

    arr = 0;
    size = 0;

    if (!fin)
    {
        cout << "could not open file." << endl;
        return;
    }

    int capacity = 10;
    arr = new Creature *[capacity];

    string name, type;
    int level, strength;

    while (fin >> name >> type >> level >> strength)
    {
        if (size == capacity)
        {
            int newCapacity = capacity * 2;
            Creature **temp = new Creature *[newCapacity];

            for (int i = 0; i < size; i++)
                temp[i] = arr[i];

            delete[] arr;
            arr = temp;
            capacity = newCapacity;
        }

        arr[size] = new Creature;
        arr[size]->name = makeLower(name);
        arr[size]->type = makeLower(type);
        arr[size]->level = level;
        arr[size]->strength = strength;
        size++;
    }

    fin.close();
}

// frees all memory used by the array
void freeCreatures(Creature **arr, int size)
{
    for (int i = 0; i < size; i++)
        delete arr[i];

    delete[] arr;
}

// sorts the pointer array in descending order
void sortCreatures(Creature **arr, int size, int field)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - 1 - i; j++)
        {
            bool swapNeeded = false;

            if (field == SORT_BY_NAME)
            {
                if (arr[j]->name < arr[j + 1]->name)
                    swapNeeded = true;
            }
            else if (field == SORT_BY_TYPE)
            {
                if (arr[j]->type < arr[j + 1]->type)
                    swapNeeded = true;
            }
            else if (field == SORT_BY_LEVEL)
            {
                if (arr[j]->level < arr[j + 1]->level)
                    swapNeeded = true;
            }
            else if (field == SORT_BY_STRENGTH)
            {
                if (arr[j]->strength < arr[j + 1]->strength)
                    swapNeeded = true;
            }

            if (swapNeeded)
            {
                Creature *temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// sort menu 
void sortMenu(Creature **original, int size)
{
    int choice;

    do
    {
        cout << "\nsort menu" << endl;
        cout << "1. sort by name" << endl;
        cout << "2. sort by type" << endl;
        cout << "3. sort by level" << endl;
        cout << "4. sort by strength" << endl;
        cout << "0. back" << endl;
        cout << "choice: ";
        cin >> choice;

        switch (choice)
        {
        case SORT_BY_NAME:
        case SORT_BY_TYPE:
        case SORT_BY_LEVEL:
        case SORT_BY_STRENGTH:
        {
            Creature **sorted = copyPointerArray(original, size);
            sortCreatures(sorted, size, choice);
            printCreatures(sorted, size);
            delete[] sorted;
            break;
        }
        case SORT_BACK:
            break;
        default:
            cout << "invalid choice." << endl;
        }

    } while (choice != SORT_BACK);
}

// search by partial name or type
void searchCreatures(Creature **arr, int size)
{
    string key;
    bool found = false;

    cout << "enter a name or type to search: ";
    cin >> key;
    key = makeLower(key);

    printHeader();

    for (int i = 0; i < size; i++)
    {
        if (arr[i]->name.find(key) != string::npos ||
            arr[i]->type.find(key) != string::npos)
        {
            cout << left << setw(15) << arr[i]->name;
            cout << left << setw(15) << arr[i]->type;
            cout << right << setw(10) << arr[i]->level;
            cout << right << setw(12) << arr[i]->strength << endl;
            found = true;
        }
    }

    if (!found)
        cout << "no matching records found." << endl;
}

int main()
{
    // load file
    Creature **army = 0;
    int size = 0;

    loadCreatures("creatures.txt", army, size);

    // main menu
    int choice;

    do
    {
        cout << "\nmain menu" << endl;
        cout << "1. print items in original order" << endl;
        cout << "2. sort menu" << endl;
        cout << "3. search" << endl;
        cout << "0. quit" << endl;
        cout << "choice: ";
        cin >> choice;

        switch (choice)
        {
        case PRINT_LIST:
            printCreatures(army, size);
            break;

        case SORT_LIST:
            sortMenu(army, size);
            break;

        case SEARCH_LIST:
            searchCreatures(army, size);
            break;

        case QUIT_APP:
            break;

        default:
            cout << "invalid choice." << endl;
        }

    } while (choice != QUIT_APP);

    freeCreatures(army, size);
    return 0;
}
