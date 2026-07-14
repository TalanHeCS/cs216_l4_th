#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

const int MAX_CREATURES = 200; // maximum number of creatures the arrays can hold

// class that stores information for one creature
class Creature
{
public:
    Creature();
    Creature(const string &nameIn, const string &typeIn, int levelIn, int strengthIn);

    void setAll(const string &nameIn, const string &typeIn, int levelIn, int strengthIn);

    const string &getName() const;
    const string &getType() const;
    int getLevel() const;
    int getStrength() const;

private:
    string name = "";
    string type = "";
    int level = 0;
    int strength = 0;
};

// options for the main menu
enum MainMenu
{
    PRINT_LIST = 1,
    SORT_LIST = 2,
    SEARCH_LIST = 3,
    QUIT_APP = 0
};

// options for the sort submenu
enum SortMenu
{
    SORT_BY_NAME = 1,
    SORT_BY_TYPE = 2,
    SORT_BY_LEVEL = 3,
    SORT_BY_STRENGTH = 4,
    SORT_BACK = 0
};

// function declarations
string makeLower(const string &s);
void printHeader();
void printCreatures(Creature *arr[], int size);
void loadCreatures(const string &filename, Creature arr[], int &size);
void buildPointerArray(Creature arr[], Creature *pointers[], int size);
void copyPointerArray(Creature *source[], Creature *dest[], int size);
bool shouldSwap(const Creature &left, const Creature &right, int field);
void sortCreatures(Creature *arr[], int size, int field);
void sortMenu(Creature *original[], int size);
void searchCreatures(Creature *arr[], int size);

int main()
{
    // fixed-size array holds the actual creature data, no dynamic allocation
    Creature army[MAX_CREATURES];

    // array of pointers into "army", used so the data does not need to be
    // physically moved when sorting or copying
    Creature *pointers[MAX_CREATURES] = {};
    int size = 0;

    // load the data from file, then set up the pointer array one time
    loadCreatures("creatures.txt", army, size);
    buildPointerArray(army, pointers, size);

    int choice = -1;
    bool quit = false;

    // main menu loop, keeps running until the user chooses to quit
    while (!quit)
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
            quit = true;
            break;
        default:
            cout << "invalid choice" << endl;
        }
    }

    return 0;
}

// default constructor, creates an empty creature with blank fields
Creature::Creature()
{
    setAll("", "", 0, 0);
}

// constructor that builds a creature directly from given values
Creature::Creature(const string &nameIn, const string &typeIn, int levelIn, int strengthIn)
{
    setAll(nameIn, typeIn, levelIn, strengthIn);
}

// sets all fields of the creature at once
void Creature::setAll(const string &nameIn, const string &typeIn, int levelIn, int strengthIn)
{
    name = nameIn;
    type = typeIn;
    level = levelIn;
    strength = strengthIn;
}

// returns the creature's name
const string &Creature::getName() const
{
    return name;
}

// returns the creature's type
const string &Creature::getType() const
{
    return type;
}

// returns the creature's level
int Creature::getLevel() const
{
    return level;
}

// returns the creature's strength
int Creature::getStrength() const
{
    return strength;
}

// takes a string and returns a lowercase copy of it
// needed because 'Z' has a lower ascii value than 'a', so comparing
// mixed-case strings directly would give the wrong sort order
string makeLower(const string &s)
{
    string result = s;

    for (int i = 0; i < (int)result.length(); i++)
    {
        if (result[i] >= 'A' && result[i] <= 'Z')
        {
            result[i] = result[i] - 'A' + 'a';
        }
    }

    return result;
}

// prints the column titles above the creature list
void printHeader()
{
    cout << left << setw(15) << "name";
    cout << left << setw(15) << "type";
    cout << right << setw(10) << "level";
    cout << right << setw(12) << "strength" << endl;
    cout << "--------------------------------------------------------" << endl;
}

// prints every creature in the pointer array, one row per creature
void printCreatures(Creature *arr[], int size)
{
    if (size == 0)
    {
        cout << "no records found." << endl;
    }
    else
    {
        printHeader();

        for (int i = 0; i < size; i++)
        {
            cout << left << setw(15) << arr[i]->getName();
            cout << left << setw(15) << arr[i]->getType();
            cout << right << setw(10) << arr[i]->getLevel();
            cout << right << setw(12) << arr[i]->getStrength() << endl;
        }
    }
}

// reads creature data from the given file into the fixed-size array
// stops early if the file has more records than MAX_CREATURES
void loadCreatures(const string &filename, Creature arr[], int &size)
{
    ifstream fin(filename.c_str());
    string name = "";
    string type = "";
    int level = 0;
    int strength = 0;

    size = 0;

    if (!fin)
    {
        cout << "could not open file." << endl;
    }
    else
    {
        while (size < MAX_CREATURES && (fin >> name >> type >> level >> strength))
        {
            arr[size].setAll(name, type, level, strength);
            size++;
        }

        fin.close();
    }
}

// sets each pointer to point at the matching creature in the data array
// only needs to run once, right after the data is loaded
void buildPointerArray(Creature arr[], Creature *pointers[], int size)
{
    for (int i = 0; i < size; i++)
    {
        pointers[i] = &arr[i];
    }
}

// copies the pointer values from one array into another
// only the addresses are copied, not the creature data itself
void copyPointerArray(Creature *source[], Creature *dest[], int size)
{
    for (int i = 0; i < size; i++)
    {
        dest[i] = source[i];
    }
}

// compares two creatures on the given field and returns true if they
// should be swapped, used as a helper for the sort function
// name and type are converted to lowercase here since this version does
// not store a precomputed lowercase copy
bool shouldSwap(const Creature &left, const Creature &right, int field)
{
    bool swapNeeded = false;

    if (field == SORT_BY_NAME)
    {
        swapNeeded = makeLower(left.getName()) < makeLower(right.getName());
    }
    else if (field == SORT_BY_TYPE)
    {
        swapNeeded = makeLower(left.getType()) < makeLower(right.getType());
    }
    else if (field == SORT_BY_LEVEL)
    {
        swapNeeded = left.getLevel() < right.getLevel();
    }
    else if (field == SORT_BY_STRENGTH)
    {
        swapNeeded = left.getStrength() < right.getStrength();
    }

    return swapNeeded;
}

// sorts the pointer array in descending order using bubble sort
// the field parameter decides which property is compared
void sortCreatures(Creature *arr[], int size, int field)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - 1 - i; j++)
        {
            if (shouldSwap(*arr[j], *arr[j + 1], field))
            {
                Creature *temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// displays the sort submenu and lets the user pick a field to sort by
// works on a copy of the pointer array so the original order is not lost
// choosing "back" returns to the main menu without sorting anything
void sortMenu(Creature *original[], int size)
{
    int choice = -1;
    bool back = false;
    Creature *sorted[MAX_CREATURES] = {};

    while (!back)
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
            copyPointerArray(original, sorted, size);
            sortCreatures(sorted, size, choice);
            printCreatures(sorted, size);
            break;
        case SORT_BACK:
            cout << "redirecting to main menuyu" << endl;
            back = true;
            break;
        default:
            cout << "invalid choice. try again" << endl;
        }
    }
}

// searches for creatures whose name or type contains the given text
// comparison is case-insensitive, so both the key and each field are
// converted to lowercase before checking
void searchCreatures(Creature *arr[], int size)
{
    string key = "";
    bool found = false;

    cout << "enter a name or type to search: ";
    cin >> key;
    key = makeLower(key);

    if (size == 0)
    {
        cout << "no records found" << endl;
    }
    else
    {
        printHeader();

        for (int i = 0; i < size; i++)
        {
            if (makeLower(arr[i]->getName()).find(key) != string::npos ||
                makeLower(arr[i]->getType()).find(key) != string::npos)
            {
                cout << left << setw(15) << arr[i]->getName();
                cout << left << setw(15) << arr[i]->getType();
                cout << right << setw(10) << arr[i]->getLevel();
                cout << right << setw(12) << arr[i]->getStrength() << endl;
                found = true;
            }
        }

        if (!found)
        {
            cout << "not found" << endl;
        }
    }
}
