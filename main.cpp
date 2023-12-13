#include "expression.h"

#include <iostream>
#include <set>
#include <fstream>
#include <iomanip>
using std::cout;
using std::endl;
using std::cerr;
using std::istream;
using std::istringstream;
using std::ifstream;
using std::setw;
using std::left;
using std::set;

// A Function to test if two doubles are "equal"
bool DoubleEquals(double a, double b, double epsilon = 1e-6);
void Help(const char* argv);
enum Flags {INVALID = -1, HARD_CODED, TEST_FILE, EXTRA_CREDIT};
struct CommandLineParameters{
    string filename;
    Flags flags;
    bool verbose;
    bool error;
};
CommandLineParameters ProcessCommandLineParameters(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    CommandLineParameters clp = ProcessCommandLineParameters(argc, argv);
    if (clp.error){
        cerr << "There is an error on your parameters" << endl;
        Help(argv[0]);
        exit(1);
    }
    if (clp.flags == INVALID){
        cerr << "You did not provide a valid flag" << endl;
        Help(argv[0]);
        exit(1);
    }
    istream* input = nullptr;
    istream* solution = nullptr;
    istream* evaluateValues = nullptr;
    istream* jsonValues = nullptr;
    if (clp.flags == HARD_CODED){
        input = new istringstream(
                "a+b+c\n"
                "a+b*c\n"
                "(a+b)/c\n"
                "(A+B)*(C-D)\n"
                "A + ( ( B + C ) * ( E - F ) - G ) / ( H - I)\n"
                "A+B*(C+D)-E/F*G+H\n"
                "(1+2)*3\n"
                "4 + 3 - 2 * 5 / 1\n"
                "alpha * beta + gamma\n"
                "125/5*3\n");
        solution = new istringstream(
                "a b + c +\n"
                "a b c * +\n"
                "a b + c /\n"
                "A B + C D - *\n"
                "A B C + E F - * G - H I - / +\n"
                "A B C D + * + E F / G * - H +\n"
                "1 2 + 3 *\n"
                "4 3 + 2 5 * 1 / -\n"
                "alpha beta * gamma +\n"
                "125 5 / 3 *\n");
        jsonValues = new istringstream (
                "{\"infix\":\"a+b+c\", \"postfix\":\"a b + c +\"}\n"
                "{\"infix\":\"a+b*c\", \"postfix\":\"a b c * +\"}\n"
                "{\"infix\":\"(a+b)/c\", \"postfix\":\"a b + c /\"}\n"
                "{\"infix\":\"(A+B)*(C-D)\", \"postfix\":\"A B + C D - *\"}\n"
                "{\"infix\":\"A + ( ( B + C ) * ( E - F ) - G ) / ( H - I)\", \"postfix\":\"A B C + E F - * G - H I - / +\"}\n"
                "{\"infix\":\"A+B*(C+D)-E/F*G+H\", \"postfix\":\"A B C D + * + E F / G * - H +\"}\n"
                "{\"infix\":\"(1+2)*3\", \"postfix\":\"1 2 + 3 *\"}\n"
                "{\"infix\":\"4 + 3 - 2 * 5 / 1\", \"postfix\":\"4 3 + 2 5 * 1 / -\"}\n"
                "{\"infix\":\"alpha * beta + gamma\", \"postfix\":\"alpha beta * gamma +\"}\n"
                "{\"infix\":\"125/5*3\", \"postfix\":\"125 5 / 3 *\"}\n"
        );
        evaluateValues = nullptr;
//        evaluateValues = new istringstream (
//                "e\n"
//                "e\n"
//                "e\n"
//                "e\n"
//                "e\n"
//                "e\n"
//                "9\n"
//                "-3\n"
//                "e\n"
//                "75\n");
    }else if (clp.flags == TEST_FILE){
        // File names convention
        // prefix: test
        // test.input will be the file with the infix expressions
        // test.output will be the file with the expected postfix expressions
        // test.extra will be the file containing the result of evaluating the postfix expressions
        // test.json will be the file containing the result of calling ToJSON
        string prefix = clp.filename;
        string inputFilename = prefix + ".input";
        string solutionFilename = prefix + ".output";
        string jsonFilename = prefix + ".json";
        input = new ifstream(inputFilename);
        solution = new ifstream (solutionFilename);
        jsonValues = new ifstream(jsonFilename);
        evaluateValues = nullptr;
        bool error = false;
        if (input->fail()){
            cerr << "Unable to open file " << inputFilename << endl;
            delete input;
            error = true;
        }
        if (solution->fail()){
            cerr << "Unable to open file " << solutionFilename << endl;
            delete solution;
            error = true;
        }
        if (jsonValues->fail()){
            cerr << "Unable to open file " << jsonFilename << endl;
            delete jsonValues;
            error = true;
        }
        if (error)
            exit(1);

    }else if (clp.flags == EXTRA_CREDIT){
        string prefix = clp.filename;
        string inputFilename = prefix + ".input";
        string solutionFilename = prefix + ".output";
        string evaluateSolutionFilename = prefix + ".extra";
        string jsonFilename = prefix + ".json";
        input = new ifstream(inputFilename);
        solution = new ifstream (solutionFilename);
        jsonValues = new ifstream(jsonFilename);
        evaluateValues = new ifstream(evaluateSolutionFilename);
        bool error = false;
        if (input->fail()){
            cerr << "Unable to open file " << inputFilename << endl;
            delete input;
            error = true;
        }
        if (solution->fail()){
            cerr << "Unable to open file " << solutionFilename << endl;
            delete solution;
            error = true;
        }
        if (evaluateValues->fail()){
            cerr << "Unable to open file " << evaluateSolutionFilename << endl;
            delete evaluateValues;
            error = true;
        }
        if (jsonValues->fail()){
            cerr << "Unable to open file " << jsonFilename << endl;
            delete jsonValues;
            error = true;
        }
        if (error)
            exit(1);
    }

    Expression* e = nullptr;
    string infixExpression, postfixExpression, evaluationValue, jsonValue;
    size_t count = 0, total = 0, extra = 0, json = 0;
    bool error;
    double value;

    while (getline(*input, infixExpression) && getline(*solution, postfixExpression) && getline(*jsonValues, jsonValue)){
        e = new Expression(infixExpression);
        e->ConvertToPostfix();
        if (clp.verbose){
            cout << "TEST - " << count << endl;
            cout << "\t Input:           " << infixExpression << endl;
            cout << "\t Your Output:     " << trim(e->GetPostfix()) << endl;
            cout << "\t Expected Output: " << trim(postfixExpression) << endl;
            cout << "\t Your JSON:       " << e->ToJSON() << endl;
            cout << "\t Expected JSON:   " << jsonValue << endl;
        }
        if (trim(e->GetPostfix()) == trim(postfixExpression))
            total++;
        if (e->ToJSON() == jsonValue)
            json++;

        if (evaluateValues){
            getline(*evaluateValues, evaluationValue);
            value = e->Evaluate(error);
            if (clp.verbose){
                cout << "\t Your Result:     " << value << endl;
                cout << "\t Expected Result: " << (evaluationValue) << endl;

            }
            if (!error && DoubleEquals(stof(evaluationValue), value))
                extra++;
        }
        count++;
        delete e;
    }
    delete input;
    delete solution;
    delete evaluateValues;
    delete jsonValues;
    if (clp.verbose){
        cout << endl << endl;
        cout << "Total tests:          " << count << endl;
        cout << "Conversion succeeded: " << total << endl;
        cout << "JSON output correct:  " << json << endl;
        cout << "Total Extra Credit:   " << extra << endl;
        cout << endl << endl;
    }
    cout << total << "\t" << json << "\t" << extra << "\t" << count << endl;


    return 0;
}

void Help(const char* argv){
    cerr << "Usage: " << endl;
    cerr << argv << " -flag file-prefix" << endl << endl;
    cerr << "Flags: " << endl;
    cerr << "-hard Use the hard coded tests, when using hard coded test use x as file prefix" << endl;
    cerr << "-file Use the provided file to read the infix expressions" << endl;
    cerr << "-extra Use the provided file to read the infix expressions, and evaluate them for extra credit" << endl;

}

bool DoubleEquals(double a, double b, double epsilon){
    return abs(a - b) < epsilon;
}

CommandLineParameters ProcessCommandLineParameters(int argc, char* argv[]){
    CommandLineParameters clp;
    if (argc < 2){
        clp.error = true;
        return clp;
    }
    clp.error = false;
    clp.flags = INVALID;
    set<string> parameters;
    for (int i=1; i<argc; i++)
        parameters.insert(string(argv[i]));
    if (parameters.count("-v") > 0)
        clp.verbose = true;
    if (parameters.count("-file") > 0){
        clp.flags = TEST_FILE;
        bool filenameFound = false;
        for (set<string>::iterator it = parameters.begin(); it != parameters.end(); ++it){
            string currentParameter = *it;
            if (currentParameter.at(0) != '-'){
                clp.filename = currentParameter;
                filenameFound = true;
                break;
            }
        }
        clp.error = !filenameFound;
    }
    if (parameters.count("-extra") > 0){
        clp.flags = EXTRA_CREDIT;
        bool filenameFound = false;
        for (set<string>::iterator it = parameters.begin(); it != parameters.end(); ++it){
            string currentParameter = *it;
            if (currentParameter.at(0) != '-'){
                clp.filename = currentParameter;
                filenameFound = true;
                break;
            }
        }
        clp.error = !filenameFound;
    }
    if (parameters.count("-hard") > 0){
        clp.flags = HARD_CODED;
        clp.filename = "";
    }
    return clp;
}