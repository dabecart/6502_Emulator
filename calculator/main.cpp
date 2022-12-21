#include <iostream>
#include <string>

using namespace std;

string input;
int inLength;
int op1, op2, ans, rem;

int x = 0, y = 0;

bool isNumber(char c){
    for(int i = 0; i < 10; i++){
        if(c == ('0'+i)) return true;
    }
    return false;
}

void clearWhitespaces(int from, int to){
    for(int i = from; i < to; i++){
        if(input[i] == ' '){
            // Find a bundle of spaces to displace them
            int clearCount = 1;
            while(input[i+clearCount] == ' '){
                clearCount++;
            }
            for(int j = i; j < inLength-clearCount; j++){
                input[j] = input[j+clearCount];
            }
            inLength = input.length()-clearCount;
            to -= clearCount;
            input = input.substr(0, inLength);
        }
    }

}

bool operate(int from, int to, char opSymbol){
    switch (opSymbol) {
    case '+':{
        ans = op1+op2;
        break;
    }

    case '-':{
        // Analysed number is just negative
        if(op1 == 0) return false;
        ans = op1-op2;
        break;
    }

    case '*':{
        ans = op1*op2;
        break;
    }

    case '/':{
        if(op2 == 0) throw invalid_argument("Math Error: Divide by 0");
        ans = op1/op2;
        break;
    }
    
    default:
        throw invalid_argument("Operation symbol not supported");
        break;
    }

    // Put the answer to the right of the expression
    int index = to;
    bool isNegative = false;
    if(ans < 0){
        ans = -ans;
        isNegative = true;
    }

    while(ans/10 > 0){
        input[--index] = ans%10 + '0';
        ans/=10;
    }
    input[--index] = ans%10 + '0';
    if(isNegative) input[--index] = '-';

    // Clear the content of the expression
    for(int i = from; i < index; i++) input[i] = ' ';
    return true;
}

// True if something is found, this is to rerun the parser in case there's anything left
bool searchForSymbol(int from, int to, char s1, char s2){

restart_symbolsearch:
    int startIndexOfOp1 = from;
    char opSymbol = 0;
    op1 = op2 = 0;
    char c = input[from];
    bool pos = true;
    if(c == '+' || c == '-'){
        while(!isNumber(c)){
            if(c == '-') pos = !pos;
            else if(c != '+') throw invalid_argument("Syntax error: Order of operations is wrong");
            c = input[++from];
            if(from == to) return false;
        }
    }

    while(isNumber(c)){
        op1 = op1*10 + (c-'0');
        c = input[++from];
        if(from == to) return false;
    }

    if(!pos) op1 = -op1;

    if(c == s1 || c == s2){
        opSymbol = c;
    }else{
        // Symbol found is not the one we're looking for, restart operands.
        from++;
        goto restart_symbolsearch;
    }

    c = input[++from];
    if(from == to) return false;

    pos = true;
    if(c == '+' || c == '-'){
        while(!isNumber(c)){
            if(c == '-') pos = !pos;
            else if(c != '+') throw invalid_argument("Syntax error: Order of operations is wrong");
            c = input[++from];
            if(from == to) return false;
        }
    }

    while(isNumber(c)){
        op2 = op2*10 + (c-'0');
        c = input[++from];
        if(from == to) break;
    }
    if(!pos) op2 = -op2;

    return operate(startIndexOfOp1, from, opSymbol);
}

bool checkParenthesis(int from, int to);

// Returns true if contains inner expression
void checkExpression(int from, int to){
    int previousLength = inLength;
    while(checkParenthesis(from, to)){
        to -= previousLength - inLength;
        previousLength = inLength;
    }

    while(searchForSymbol(from, to, '*', '/')){
        // Remove all the whitespaces
        clearWhitespaces(from, to);
        to -= previousLength - inLength;
        previousLength = inLength;
    }

    while(searchForSymbol(from, to, '+', '-')){
        // Remove all the whitespaces
        clearWhitespaces(from, to);
        to -= previousLength - inLength;
        previousLength = inLength;
    }
}

// from included, to excluded.
// Returns true if it contains inner parenthesis (inner expression)
bool checkParenthesis(int from, int to){
    while(from < to){
        if(input[from] == '('){
            uint8_t parenthesisRecount = 1;
            for(int j = from+1; j < to; j++){
                if(input[j] == '(') parenthesisRecount++;
                else if(input[j] == ')') parenthesisRecount--;

                if(parenthesisRecount == 0){
                    checkExpression(from+1,j);
                    // Parenthesis will surely have moved to the left, so the first closing parenthesis found to
                    // the right of the opening parenthesis is the one that moved!
                    for(j = from+1; j < inLength; j++){
                        if(input[j] == ')'){
                            input[from] = ' ';
                            input[j] = ' ';
                            clearWhitespaces(from, j+1);
                            return true;
                        }
                    }
                    throw invalid_argument("How did I get here?");
                }
            }
            throw invalid_argument("Syntax error: missing closing parenthesis");
        }else if(input[from] == ')'){
            throw invalid_argument("Syntax error: misplaced opening parenthesis");
        }
        from++;
    }
    return false;
}

int main(){ 
    cout << "Insert math expression: ";
    cin >> input;
    inLength = input.length();

    cout << input << " = ";
    // Get rid of all invalid syntax and leading zeros
    checkExpression(0, inLength);
    cout << input;
} 