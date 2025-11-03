#include <iostream>
#include <string>

using namespace std;

int main(){

    char letter = 'a';
    int valor = (int)letter;
    int new_val = valor - 32;
    char let = (char)new_val;
    cout << letter << endl;
    cout << valor << endl;
    cout << new_val << endl;
    cout << let << endl;
    return 0;
}