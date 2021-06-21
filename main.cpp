#include <iostream>

using namespace std;

typedef char huge[];

class Arithmetic{

public:

    Arithmetic(char *string, int length = 0) {

        int limit = 1000000;

        if (length == 0)
            while (string[length] != '\0') {
                if (length > limit) throw invalid_argument("String is too large.");
                length++;
            }

        if ( length <= 0 )
            throw invalid_argument("Layout: a length is invalid.");

        if ( !is_valid(string, length) )
            throw invalid_argument("Layout: a string is invalid.");

        if ( !fill_layout(string, length) )
            throw invalid_argument("Layout: Unknown error.");
    }

    int     get_size() const {
        return size;
    }

    bool    is_positive() const {
        return positive;
    }

    void    change_sign(bool pos) {
        positive = pos;
    }

    char    &operator[] (int i) const{
        if ((i >= size) || (i < 0)) throw invalid_argument("Element does not exist");
        return decimals[i];
    }

    Arithmetic operator+ (Arithmetic const &term) {
        if (positive != term.is_positive())
            return difference(term);

        return sum(term);
    }

    Arithmetic operator- (Arithmetic const &term) {
        if (positive != term.is_positive())
            return sum(term);

        return difference(term);
    }

    Arithmetic operator* (Arithmetic const &term) {
        return product(term);
    }

    void    display_decimals() const {

        if (!positive)
            cout << '-';

        for (int i = 0; i < size; i++)
            cout << decimals[i];
        cout << endl;
    }

    ~Arithmetic() {
        if ( decimals != nullptr )
            free(decimals);
    }

private:
    char *decimals  = nullptr;
    bool positive   = true;
    int  size       = 0;

    bool static is_sign(char symbol) {
        if ( (symbol == '+') || (symbol == '-') )
            return true;
        return false;
    }

    bool static is_plus(char symbol) {
        if (symbol == '-')
            return false;
        return true;
    }

    bool static is_valid(char const *string, int length) {

        for (int i = 0; i < length; i++) {

            char symbol = string[i];

            if ((symbol >= '0') && (symbol <= '9'))
                continue;

            if (is_sign(symbol) && (i == 0))
                continue;

            return false;
        }

        return true;
    }

    bool        fill_layout(char const *string, int length) {

        int  shift          = 0;
        char symbol         = string[0];
        int  shift_zero     = 0;
        int  length_zero    = 0;

        if ( is_sign(symbol) ) {
            positive = is_plus(symbol);
            length--;
            shift++;
        }

        for (int i = 0; i < length; i++) {
            if (string[i + shift] != '0') break;
            if (length + length_zero == 1) break;
            shift_zero++;
            length_zero--;
        }

        shift  += shift_zero;
        length += length_zero;

        decimals            = (char *) calloc(length + 1, sizeof(char));
        decimals[length]    = '\0';
        size                = length;

        for (int i = 0; i < length; i++)
            decimals[i] = string[i + shift];

        return true;
    }

    Arithmetic  sum( Arithmetic const &term) {
        int  smaller    = 0;
        int  transition = 0;
        int  remains    = 0;
        int  digit      = 0;
        int  size_a     = size;
        int  size_b     = term.get_size();
        int  size_max   = max(size_a, size_b);
        int  shift      = abs(size_a - size_b);
        char buffer[ size_max + 2 ];
        buffer[ size_max + 1 ] = '\0';

        for (int i = size_max; i >= 1; i--) {

            if (size_a > size_b) {
                smaller     = (i - shift - 1 < 0) ? 0 : term[i - shift - 1];
                digit       = (decimals[i - 1] % 48) + (smaller % 48) + transition;
                transition  = digit / 10;
                remains     = digit % 10;
                buffer[i]   = (char)(remains + 48);
            }
            else {
                smaller     = (i - shift - 1 < 0) ? 0 : decimals[i - shift - 1];
                digit       = (smaller % 48) + (term[i - 1] % 48) + transition;
                transition  = digit / 10;
                remains     = digit % 10;
                buffer[i]   = (char)(remains + 48);
            }
        }

        buffer[0] = (char)(transition + 48);
        Arithmetic result(buffer, size_max + 1);
        result.change_sign(positive);
        return result;
    }

    Arithmetic  difference( Arithmetic const &term ) {

        int  smaller    = 0;
        int  transition = 0;
        int  remains    = 0;
        int  digit      = 0;
        int  size_a     = size;
        int  size_b     = term.get_size();
        int  size_max   = max(size_a, size_b);
        int  shift      = abs(size_a - size_b);
        char buffer[ size_max + 1 ];
        buffer[ size_max ] = '\0';

        for (int i = size_max-1; i >= 0; i--) {

            if (size_a > size_b) {
                smaller     = (i - shift < 0) ? 0 : term[i - shift];
                digit       = (decimals[i] % 48) - (smaller % 48) - transition;
                transition  = (digit < 0) ? 1 : 0;
                remains     = (digit < 0) ? 10 + digit: digit;
                buffer[i]   = (char)(remains + 48);
            }
            else {
                smaller     = (i - shift < 0) ? 0 : decimals[i - shift];
                digit       = (term[i] % 48) - (smaller % 48) - transition;
                transition  = (digit < 0) ? 1 : 0;
                remains     = (digit < 0) ? 10 + digit: digit;
                buffer[i]   = (char)(remains + 48);
            }
        }

        Arithmetic result(buffer, size_max);

        if (size_a > size_b) {
            if (positive) result.change_sign(true);
            else result.change_sign(false);
        }
        else if (size_a == size_b) {
            if (transition == positive) result.change_sign(false);
            else result.change_sign(true);
        }
        else {
            if (positive) result.change_sign(false);
            else result.change_sign(true);
        }

        return result;
    }

    Arithmetic  product( Arithmetic const &term ) {

        int  transition = 0;
        int  remains    = 0;
        int  digit      = 0;
        int  prev       = 0;
        int  position   = 0;
        int  size_a     = size;
        int  size_b     = term.get_size();
        int  shift      = abs(size_a - size_b);
        char buffer[ size_a + size_b + 1 ];

        for (int i = 0; i <= size_a + size_b; i++)
            buffer[i] = 0;

        for (int i = 0; i < size_a; i++) {

            for (int j = 0; j < size_b; j++) {
                position    = size_a + size_b - j - i - 1;
                prev        = buffer[position] % 48;
                digit       = (decimals[size_a - i - 1] % 48) * (term[size_b - j - 1] % 48) + transition;
                transition  = (digit + prev) / 10;
                remains     = (digit + prev) % 10;
                buffer[position] = (char)(remains + 48);
            }

            if (i != size_a-1) {
                buffer[position - 1] = (char)(transition + 48);
                transition           = 0;
            }
        }

        buffer[position - 1] = (char)(transition + 48);
        for (int i = 0; i < size_a + size_b; i++) {
            if (buffer[i] != 0) break;
            buffer[i] = 48;
        }

        Arithmetic result(buffer, size_a + size_b);

        if ( !(positive == term.is_positive()) )
            result.change_sign(false);
        else
            result.change_sign(true);

        return result;
    }
};

int main() {

    char million[1000002];

    for (int i = 1; i<1000001; i++) million[i] = '0';
    million[0]          = '1';
    million[1000001]    = 0;
    huge three = "5";

    Arithmetic a(million, 1000001);
    Arithmetic c(three, 1);

    Arithmetic result = a * c;
    cout << result[0];
    cout << result[999999];

    return 0;
}
