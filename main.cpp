#include <bits/stdc++.h>

using namespace std;

class JSON;

typedef map<string, JSON> Map;
typedef vector<JSON> Vector;

class JSON {
private:
    void* ptr;
    string type;
public:

    string value() {
        if (type == "string") {
            return *((string*) ptr);
        } else {
            throw "NotAString";
        }
    }

    JSON& operator[](const string& arg) {
        if (type == "string") {
            throw "NotSubscriptableObject";
        }
        if (type == "map") {
            return (*((Map*) ptr))[arg];
        }
        if (type == "list") {
            throw "ListIndexNotInteger";
        }
    }

    JSON& operator[](const size_t arg) {
        if (type == "string") {
            throw "NotSubscriptableObject";
        }
        if (type == "map") {
            throw "MapIndexNotString";
        }
        if (type == "list") {
            return (*((Vector*) ptr))[arg];
        }
    }

    JSON& at(const string& arg) {
        if (type == "string") {
            throw "NotSubscriptableObject";
        }
        if (type == "map") {
            return (*((Map*) ptr)).at(arg);
        }
        if (type == "list") {
            throw "ListIndexNotInteger";
        }
    }

    JSON& at(const size_t arg) {
        if (type == "string") {
            throw "NotSubscriptableObject";
        }
        if (type == "map") {
            throw "MapIndexNotString";
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

    explicit JSON(string s = "") {
        // cerr << s << endl << endl;
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
            Vector v;
            string cur_tok;
            stack<char> st;
            s.back() = ',';
            for (size_t i = 1; i < s.size(); ++i) {
                if (s[i] == ',' and st.empty()) {
                    int cnt = 0;
                    // cerr << "before:" << cur_tok << "end" << endl;
                    while (cnt < cur_tok.size() and cur_tok[cnt] == ' ') {
                        ++cnt;
                    }
                    int cnt_end = (int) cur_tok.size() - 1;
                    while (cnt_end > -1 and cur_tok[cnt_end] == ' ') {
                        --cnt_end;
                    }
                    v.emplace_back(string(cur_tok.begin() + cnt, cur_tok.begin() + cnt_end + 1));
                    // cerr << "after:" << tokens.back() << "end" << endl << endl;
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
            // for (auto token : tokens) {
            //    v.emplace_back(token);
            // }
            ptr = new Vector(v);
        }
        if (s[0] == '{') {
            type = "map";
            Map m;

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
                    // cerr << "before:" << cur_tok << "end" << endl;
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


                    m[string(cur_name.begin() + cnt_name + 1, cur_name.begin() + cnt_name_end)] = JSON(
                            string(cur_value.begin() + cnt, cur_value.begin() + cnt_end + 1));
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
            ptr = new Map(m);
        }
    }
};

int main() {
    freopen("input.txt", "r", stdin);
    string s;
    getline(cin, s);
    JSON obj(s);

    cout << endl << obj.serialize() << endl << JSON(obj.serialize()).serialize();
    return 0;
}