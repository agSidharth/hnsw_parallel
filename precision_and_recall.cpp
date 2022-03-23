#include <bits/stdc++.h>
using namespace std;

int create_set(set<int> &tmp, string str)
{
    stringstream s(str);
    float temp;
    int cnt = 0;
    while(s>>temp)
    {
        tmp.insert(temp);
        cnt++;
    }
    return cnt;

}

int main(int argc, char** argv)
{
    int k = stoi(argv[1]);
    string output_file = argv[2];
    string truth_value = argv[3];

    fstream of(output_file , ios::in);
    fstream tv(truth_value,ios::in);

    float prec=0,recall=0;

    string s1,s2;
    int cnt = 0;

    while(getline(of,s1) || getline(tv,s1))
    {
        cnt ++;
        if(s1.size() == 0 && s2.size() == 0) break;
        if(s1.size()==0 || s2.size()==0)
        {
            cout << "LINES IN 2 FILES DO NOT MATCH";
            return;
        }
        set<int> a,b;
        create_set(a,s1);
        create_set(b,s2);
        set<int> c;

        set_intersection(a.begin(), a.end(), a.begin(), a.end(),
                 inserter(c, c.begin()));
        prec += c.size()/a.size();
        recall += c.size()/b.size();
    }
    cout << "Precision" << prec/cnt << endl;
    cout << "Recall : " << recall/cnt << endl;
    return 0;
}