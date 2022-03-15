#include <bits/stdc++.h>
using namespace std;

int count_elements(string str)
{
    int cnt = 0;
    char last = '2';
    for (auto s:str)
    {
        if(s!=last) cnt++;
        last = s;
    }
    return cnt;
}

int main(int argc, char* argv[]){
    assert(argc>2);

    string input_dir = argv[1];
    string output_dir = argv[2];

    int temp;

    ifstream infil(input_dir+"/max_level.txt",ios::in);
    fstream outfil(output_dir+"/max_level.bin",ios::out);
    infil>>temp;
    outfil.write((char*)&temp,4);
    infil.close();
    outfil.close();

    infil.open(input_dir+"/ep.txt",ios::in);
    outfil.open(output_dir+"/ep.bin",ios::out);
    infil>>temp;
    outfil.write((char*)&temp,4);
    infil.close();
    outfil.close();
    cout << "Writing";
    vector<string> innames  = {"/level.txt","/index.txt","/indptr.txt","/level_offset.txt"};
    vector<string> outnames = {"/level.bin","/index.bin","/indptr.bin","/level_offset.bin"};

    for(int idx=0;idx<innames.size();idx++)
    {
        infil.open(input_dir+innames[idx],ios::in);
        outfil.open(output_dir+outnames[idx],ios::out);
        while(infil>>temp)
        {
            outfil.write((char*)&temp,4);
        }    
        infil.close();
        outfil.close();
    }

    string in_vect = "/vect.txt";
    string out_vect = "/vect.bin";

    infil.open(input_dir+in_vect,ios::in);
    string str;
    getline(infil,str);
    int cnt = count_elements(str);
    infil.close();

    infil.open(input_dir+in_vect,ios::in);
    outfil.open(output_dir+out_vect,ios::out);

    outfil.write((char*)&cnt,4);
    while(infil>>temp)
    {
        outfil.write((char*)&temp,4);
    }


}