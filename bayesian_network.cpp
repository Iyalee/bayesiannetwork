#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <random>
#include <iomanip>
#include <sstream>

using namespace std;

class Variable {
    public:
        Variable(int name, vector<vector<double>> array, vector<int> children, vector<int> parents);
        int name;
        vector<vector<double>> array;
        vector<int> parents, children;
};

Variable::Variable(int name, vector<vector<double>> array, vector<int> children, vector<int> parents) : name(name), array(array), children(children), parents(parents) {}

int arr_to_int(vector<int> array, vector<int> indexes, vector<int> names) {
    int output = 0;
    for(int i = array.size()-1; i >= 0; i--) {
        output += pow(indexes[names[i]], array.size()-i-1) * array[i];
    }
    return output;
}

int product(vector<int> arr) {
    int output = 1;
    for(auto& i : arr) output *= i;
    return output;
}

class BayesianNetwork {
    public:
        BayesianNetwork(vector<Variable> variables);
        void multiply_outcome(Variable var);
        void return_probability();
    private:
        vector<Variable> variables;
        vector<int> indexes, statuses, visited;
        double outcome = 1;
};

BayesianNetwork::BayesianNetwork(vector<Variable> variables) : variables(variables) {
    for(auto& i : variables) {
        indexes.push_back(i.array[0].size());
        statuses.push_back(0);
    }
}

void BayesianNetwork::multiply_outcome(Variable var) {
    for(auto i : visited) if(i == var.name) return ;
    visited.push_back(var.name);

    vector<int> names, parents_values;
    for(int i = 0; i < var.parents.size(); i++) {
        parents_values.push_back(statuses[var.parents[i]]);
        names.push_back(var.parents[i]);
    }

    if(parents_values.size() == 0) {
        outcome *= variables[var.name].array[0][statuses[var.name]];
        return ;
    }

    int index = arr_to_int(parents_values, indexes, names);
    outcome *= var.array[index][statuses[var.name]];

    for(int i = 0; i < var.parents.size(); i++) {
        multiply_outcome(variables[var.parents[i]]);
    } 
}

void BayesianNetwork::return_probability() {
    for(auto&i:indexes) cout << i << ' ';
    cout << '\n';
    vector<Variable> var_without_children;
    double s = 0.0;

    for(int i = 0; i < variables.size(); i++) {
        if(variables[i].children.size() == 0) var_without_children.push_back(variables[i]);
    }

    for(int i = 0; i < product(indexes); i++) {
        outcome = 1;
        visited.clear();

        for(int j = 0; j < var_without_children.size(); j++) multiply_outcome(var_without_children[j]);

        std::ostringstream oss;
        for (size_t j = 0; j < statuses.size(); ++j) {
            if (j != 0) oss << ", ";
            oss << statuses[j];
        }
        std::cout << oss.str() << " : " << std::scientific << std::setprecision(4) << outcome << std::endl;
        s += outcome;

        for(int k = statuses.size()-1; k >= 0; k--) {
            if(statuses[k] == indexes[k]-1) statuses[k] = 0;
            else {
                statuses[k]++;
                break;
            }
        }
    }
    cout << s;
}

int main() {
    Variable a(0, {{0.1, 0.7, 0.2}}, {4}, {});
    Variable b(1, {{0.2, 0.8}}, {4}, {});
    Variable c(2, {{0.6, 0.4}}, {5}, {});
    Variable d(3, {{0.3, 0.7}}, {5}, {});

    Variable e(4, {{0.2, 0.8}, {0.3, 0.7}, {0.1, 0.9}, {0.5, 0.5}, {0.1, 0.9}, {0.2, 0.8}, {0.6, 0.4}, {0.45, 0.55}}, {6}, {0,1});
    Variable f(5, {{0.1, 0.9}, {0.8, 0.2}, {0.4, 0.6}, {0.3, 0.7}}, {}, {2,3});

    Variable g(6, {{0.3, 0.7}, {0.2, 0.8}}, {}, {4});

    vector<Variable> input = {a, b, c, d, e, f, g};

    BayesianNetwork bn(input);
    bn.return_probability();
}