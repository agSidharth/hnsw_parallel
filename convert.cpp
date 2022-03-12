#include <bits/stdc++.h>
using namespace std;

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

    vector<string> innames  = {"/levels.txt","/index.txt","/indptr.txt","/level_offset.txt","/vect.txt"};
    vector<string> outnames = {"/levels.bin","/index.bin","/indptr.bin","/level_offset.bin","/vect.bin"};

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
}