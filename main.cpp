#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

const int MAX_CREATURES = 200; // max size for the fixed array, no new/delete needed

// one creature record
struct Creature
{
    string name;
    string type;
    int level;
    int strength;

    // lowercase versions of name/type, stored so case doesn't have to be
    // converted over and over during sorting or searching
    string nameLower;
    string typeLower;
};

// menu choices for the main menu
enum MainMenu
{
    PRINT_LIST = 1,
    SORT_LIST,
    SEARCH_LIST,
    QUIT_APP
};

// menu choices for the sort submenu
enum SortMenu
{
    SORT_BY_NAME = 1,
    SORT_BY_TYPE,
    SORT_BY_LEVEL,
    SORT_BY_STRENGTH,
    SORT_BACK
};

// converts a string to lowercase letter by letter
// note: 'Z' < 'a' in ascii, so comparisons on mixed-case strings
// would sort incorrectly without this
string makeLower(string s)
{
    for (int i = 0; i < (int)s.length(); i++)
    {
        if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] = s[i] - 'A' + 'a';
    }
    return s;
}

// prints the column headers for the table
void printHeader()
{
    cout << left << setw(15) << "name";
    cout << left << setw(15) << "type";
    cout << right << setw(10) << "level";
    cout << right << setw(12) << "strength" << endl;

    cout << "--------------------------------------------------------" << endl;
}

// loops through the pointer array and prints every creature
void printCreatures(Creature *arr[], int size)
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

// reads the file line by line into a fixed-size array
// no new/delete here, so the loop just stops early if the file
// has more records than MAX_CREATURES can hold
void loadCreatures(const string &filename, Creature arr[], int &size)
{
    ifstream fin(filename.c_str());

    size = 0;

    if (!fin)
    {
        cout << "could not open file." << endl;
        return;
    }

    string name, type;
    int level, strength;

    while (size < MAX_CREATURES && (fin >> name >> type >> level >> strength))
    {
        arr[size].name = name;
        arr[size].type = type;
        arr[size].level = level;
        arr[size].strength = strength;

        // computing the lowercase fields once here saves doing it
        // repeatedly later during sort/search
        arr[size].nameLower = makeLower(name);
        arr[size].typeLower = makeLower(type);

        size++;
    }

    fin.close();
}

// points every entry in "pointers" at the matching entry in "arr"
// only needs to run once, right after loading — this is the part
// covered in the "how many times do you need to assign pointers"
// question from the notes
void buildPointerArray(Creature arr[], Creature *pointers[], int size)
{
    for (int i = 0; i < size; i++)
        pointers[i] = &arr[i];
}

// copies the addresses from one pointer array into another
// (just copying pointers, not the actual creature data)
void copyPointerArray(Creature *source[], Creature *dest[], int size)
{
    for (int i = 0; i < size; i++)
        dest[i] = source[i];
}

// bubble sort on the pointer array, descending order
// compares nameLower/typeLower instead of name/type so uppercase
// letters don't throw off the ordering
void sortCreatures(Creature *arr[], int size, int field)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - 1 - i; j++)
        {
            // bool can just be set directly from a comparison,
            // no need for an if/else here
            bool swapNeeded = false;

            if (field == SORT_BY_NAME)
                swapNeeded = arr[j]->nameLower < arr[j + 1]->nameLower;
            else if (field == SORT_BY_TYPE)
                swapNeeded = arr[j]->typeLower < arr[j + 1]->typeLower;
            else if (field == SORT_BY_LEVEL)
                swapNeeded = arr[j]->level < arr[j + 1]->level;
            else if (field == SORT_BY_STRENGTH)
                swapNeeded = arr[j]->strength < arr[j + 1]->strength;

            if (swapNeeded)
            {
                Creature *temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// sort submenu — lets the user pick a field to sort by or go back
// switch falls through for the 4 sort options into the same sort call
// going back does not force a sort, it just breaks out of the loop
void sortMenu(Creature *original[], int size)
{
    int choice;
    Creature *sorted[MAX_CREATURES]; // reused for every sort attempt

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
            // work on a copy so the original order stays intact
            copyPointerArray(original, sorted, size);
            sortCreatures(sorted, size, choice);
            printCreatures(sorted, size);
            break;

        case SORT_BACK:
            cout << "going back to the main menu" << endl;
            break;

        default:
            cout << "invalid choice. try again" << endl;
        }

    } while (choice != SORT_BACK);
}

// searches name and type fields for a partial match
// uses the precomputed lowercase fields so the search is case-insensitive
void searchCreatures(Creature *arr[], int size)
{
    string key;
    bool found = false;

    cout << "enter a name or type to search: ";
    cin >> key;
    key = makeLower(key);

    printHeader();

    for (int i = 0; i < size; i++)
    {
        if (arr[i]->nameLower.find(key) != string::npos ||
            arr[i]->typeLower.find(key) != string::npos)
        {
            cout << left << setw(15) << arr[i]->name;
            cout << left << setw(15) << arr[i]->type;
            cout << right << setw(10) << arr[i]->level;
            cout << right << setw(12) << arr[i]->strength << endl;
            found = true;
        }
    }

    if (!found)
        cout << "not found" << endl;
}

int main()
{
    // static here to avoid putting 200 creature records directly on the stack
    static Creature army[MAX_CREATURES];
    static Creature *pointers[MAX_CREATURES];
    int size = 0;

    loadCreatures("creatures.txt", army, size);

    // build the pointer array one single time, right after the data loads
    buildPointerArray(army, pointers, size);

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
            printCreatures(pointers, size);
            break;

        case SORT_LIST:
            sortMenu(pointers, size);
            break;

        case SEARCH_LIST:
            searchCreatures(pointers, size);
            break;

        case QUIT_APP:
            cout << "quitting" << endl;
            break;

        default:
            cout << "invalid choice" << endl;
        }

    } while (choice != QUIT_APP);

    return 0;
}
