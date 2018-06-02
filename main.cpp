#include <iostream>
#include <vector>
#include <map>
#include <stack>

using std::cerr;
using std::string;
using std::map;
using std::vector;
using std::stack;
using std::cin;
using std::runtime_error;


class WrongIndexException : public runtime_error {
public:

    WrongIndexException(const string& good, const string& bad) : runtime_error(
            ("WrongIndexType expected " + good + " got " + bad)) {}
};

class NotIndexableException : public runtime_error {
public:
    NotIndexableException() : runtime_error("NotIndexable") {}
};

class NotStringException : public runtime_error {
public:
    NotStringException() : runtime_error("NotString") {}
};


class JSON;

typedef map<string, JSON> Map;
typedef vector<JSON> Vector;

class JSON {
private:
    void* ptr;
    string type;
public:

    explicit JSON(string s = "") {
        if (s.empty()) {
            type = "string";
            ptr = new string(s);
        }
        if (s[0] == '"') {
            type = "string";
            ptr = new string(s.substr(1, s.size() - 2));
        }
        if (s[0] == '[') {
            type = "list";
            ptr = new Vector;
            string cur_tok;
            stack<char> st;
            s.back() = ',';
            for (size_t i = 1; i < s.size(); ++i) {
                if (s[i] == ',' and st.empty()) {
                    int cnt = 0;
                    while (cnt < cur_tok.size() and cur_tok[cnt] == ' ') {
                        ++cnt;
                    }
                    int cnt_end = (int) cur_tok.size() - 1;
                    while (cnt_end > -1 and cur_tok[cnt_end] == ' ') {
                        --cnt_end;
                    }
                    (*((Vector*) ptr)).emplace_back(string(cur_tok.begin() + cnt, cur_tok.begin() + cnt_end + 1));
                    cur_tok = "";
                } else {
                    cur_tok.push_back(s[i]);
                    if (s[i] == '{' or s[i] == '[') {
                        st.push(s[i]);
                    } else if (s[i] == '}' or s[i] == ']') {
                        st.pop();
                    }
                }
            }
        }
        if (s[0] == '{') {
            type = "map";
            ptr = new Map;
            string cur_name;
            string cur_value;
            bool name = true;
            stack<char> st;
            s.back() = ',';
            for (size_t i = 1; i < s.size(); ++i) {
                if (s[i] == ':' and st.empty()) {
                    name = false;
                } else if (s[i] == ',' and st.empty()) {
                    name = true;
                    int cnt = 0;
                    while (cnt < cur_value.size() and cur_value[cnt] == ' ') {
                        ++cnt;
                    }
                    int cnt_end = (int) cur_value.size() - 1;
                    while (cnt_end > -1 and cur_value[cnt_end] == ' ') {
                        --cnt_end;
                    }

                    int cnt_name = 0;
                    while (cnt_name < cur_name.size() and cur_name[cnt_name] == ' ') {
                        ++cnt_name;
                    }
                    int cnt_name_end = (int) cur_name.size() - 1;
                    while (cnt_name_end > -1 and cur_name[cnt_name_end] == ' ') {
                        --cnt_name_end;
                    }


                    (*((Map*) ptr))[string(cur_name.begin() + cnt_name + 1, cur_name.begin() + cnt_name_end)] =
                            JSON(string(cur_value.begin() + cnt, cur_value.begin() + cnt_end + 1));
                    cur_name = "";
                    cur_value = "";
                } else {
                    if (name) {
                        cur_name.push_back(s[i]);
                    } else {
                        cur_value.push_back(s[i]);
                    }
                    if (s[i] == '{' or s[i] == '[') {
                        st.push(s[i]);
                    } else if (s[i] == '}' or s[i] == ']') {
                        st.pop();
                    }
                }
            }
        }
    }

    JSON(const JSON& other) {
        type = other.type;
        if (type == "string") {
            ptr = new string;
            *((string*) ptr) = *((string*) other.ptr);
        }
        if (type == "list") {
            ptr = new Vector;
            for (const JSON& child : *((Vector*) other.ptr)) {
                (*((Vector*) ptr)).push_back(child);
            }
        }
        if (type == "map") {
            ptr = new Map;
            for (const auto& child : *((Map*) other.ptr)) {
                (*((Map*) ptr))[child.first] = child.second;
            }
        }
    }

    JSON& operator=(const JSON& other) {
        JSON tmp(other);
        swap(tmp.type, type);
        void* p = ptr;
        ptr = tmp.ptr;
        tmp.ptr = p;
        //swap((void*) tmp.ptr, (void*)ptr);
        return *this;
    }

    string value() {
        if (type == "string") {
            return *((string*) ptr);
        } else {
            throw NotStringException();
        }
    }

    JSON& operator[](const string& arg) {
        if (type == "string") {
            throw NotIndexableException();
        }
        if (type == "map") {
            return (*((Map*) ptr))[arg];
        }
        if (type == "list") {
            throw WrongIndexException("int", "string");
        }
    }

    JSON& operator[](const size_t arg) {
        if (type == "string") {
            throw NotIndexableException();
        }
        if (type == "map") {
            throw WrongIndexException("string", "int");
        }
        if (type == "list") {
            return (*((Vector*) ptr))[arg];
        }
    }

    JSON& at(const string& arg) {
        if (type == "string") {
            throw NotIndexableException();
        }
        if (type == "map") {
            return (*((Map*) ptr)).at(arg);
        }
        if (type == "list") {
            throw WrongIndexException("int", "string");
        }
    }

    JSON& at(const size_t arg) {
        if (type == "string") {
            throw NotIndexableException();
        }
        if (type == "map") {
            throw WrongIndexException("string", "int");
        }
        if (type == "list") {
            return (*((Vector*) ptr)).at(arg);
        }
    }

    string serialize() const {
        if (type == "string") {
            return '"' + *((string*) ptr) + '"';
        }
        if (type == "map") {
            string answer = "{";
            for (const auto& child : (*((Map*) ptr))) {
                answer += '"';
                answer += child.first;
                answer += '"';
                answer += ':';
                answer += child.second.serialize();
                answer += ',';
            }
            answer.back() = '}';
            return answer;
        }
        if (type == "list") {
            string answer = "[";
            Vector v = (*((Vector*) ptr));
            for (const auto& child : v) {
                // cerr << child.type << endl;
                answer += child.serialize();
                answer += ',';
            }
            if (answer.back() == ',') {
                answer.back() = ']';
            }
            return answer;
        }
    }

    ~JSON() {
        // WHAT THE HELL IS HERE??
        if (type == "string") {
            delete (string*) ptr;
        }
        if (type == "map") {
            // (*((Map*) ptr)).~Map();
        }
        if (type == "list") {
            (*((Vector*) ptr)).~Vector();;
        }
    }
};

int main() {
    freopen("input.txt", "r", stdin);
    string s;
    getline(cin, s);
    JSON obj(s);
    cerr << "first_" << obj["a"][0].value() << "_end" << "\n";
    {
        JSON obj2 = obj;
        string tmp = "\" buba \"";
        obj2.at("a").at(0) = JSON(tmp);
        cerr << "second_" << obj2["a"][0].value() << "_end" << "\n";

        try {
            obj.at(1);
        }
        catch (...) {
            cerr << "ok1\n";
        }
        try {
            obj.at("a").at("a");
        }
        catch (...) {
            cerr << "ok2\n";
        }
        try {
            obj.at("a").at("a").at(1);
        }
        catch (...) {
            cerr << "ok3\n";
        }
        try {
            obj.value();
        }
        catch (...) {
            cerr << "ok4\n";
        }
    }
    cerr << "first_" << obj["a"][0].value() << "_end" << "\n";
    return 0;
}