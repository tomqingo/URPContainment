#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <unordered_map>
#include <string>
#include <time.h>
using namespace std;


vector<int> GetRelativeEssenCubes(vector<vector<int>> cubelist, ofstream& ofs);
vector<int> GetRedudantCubes(vector<vector<int>> cubelist, vector<int> RelativeEssencol, ofstream& ofs);
bool URPCubeContain(vector<vector<int>> cubelist, int termid, ofstream& ofs);
bool URPTautologyCheck(vector<vector<int>> cubelist, unordered_map<int, int> selectflag, ofstream& ofs);
vector<vector<int>> CubelistAssign(vector<vector<int>> cubelist, int col_chosen, int pos_flag);
bool JudgeAllDCCube(vector<vector<int>> cubelist);
bool JudgeUnate(vector<vector<int>> cubelist);
bool JudgeColumnUnate(vector<int> cubelistColumn);
bool JudgeSingleVarPolar(vector<vector<int>> cubelist);
int FindMostBinateVar(vector<vector<int>> cubelist, unordered_map<int,int> selectflag);
vector<vector<int>> UnateReduction(vector<vector<int>> cubelist);
vector<int> FindDCrows(vector<vector<int>> cubelist, vector<int> col);



int main(int argc, char* argv[])
{
    clock_t startTime, endTime;
    startTime = clock();

    if(argc<3 || argc>4)
    {
        cout<<"usage: reduce <in_file> <out_file> [unate_reduce_log_file]"<<endl;
    }
    else
    {
        string in_file_name = argv[1];
        string out_file_name = argv[2];
        string unate_reduct_file_name = (argc==4? argv[3]:"");

        //cout<<in_file_name<<endl;
        ifstream fin(in_file_name);
        string line;

        getline(fin, line);


        int num_variable = stoi(line);

        cout<<num_variable<<endl;

        getline(fin, line);

        int num_term = stoi(line);

        cout<<num_term<<endl;

        vector<vector<int>> cubelist(num_term, vector<int>(num_variable, 0));
        for(int i=0; i<num_term; i++)
        {
            getline(fin, line);
            for(int j=0; j<num_variable; j++)
            {
                cubelist[i][j] = int(line[j]-'0');
                cout<<cubelist[i][j]<<' ';
            }
            cout<<endl;
        }

        vector<int> RelativeEssencol;
        vector<int> Redundantcol;

        ofstream ofs1;
        ofs1.open(unate_reduct_file_name, ios::out);
        RelativeEssencol = GetRelativeEssenCubes(cubelist, ofs1);

        Redundantcol = GetRedudantCubes(cubelist, RelativeEssencol, ofs1);

        ofs1.close();
        
        unordered_map<int, int> Redundantmap;
        for(int i=0; i<Redundantcol.size(); i++)
            Redundantmap[Redundantcol[i]]= 1;
        
        vector<vector<int>> cubelistremain;
        for(int i=0; i<cubelist.size(); i++)
        {
            if(Redundantmap.find(i)==Redundantmap.end())
                cubelistremain.push_back(cubelist[i]);
        }

        /*
        cout<<cubelistremain.size()<<endl;

        for(int i=0; i<cubelistremain.size(); i++)
        {
            for(int j=0; j<cubelistremain[i].size(); j++)
                cout<<cubelistremain[i][j]<<' ';
            cout<<endl;
        }
        */

        ofstream ofs2;
        ofs2.open(out_file_name, ios::out);

        if(cubelistremain.size()>0)
            ofs2<<cubelistremain[0].size()<<endl;
        else
            ofs2<<0<<endl;

        ofs2<<to_string(cubelistremain.size())<<endl;

        for(int i=0; i<cubelistremain.size(); i++)
        {
            for(int j=0; j<cubelistremain[0].size(); j++)
                ofs2<<cubelistremain[i][j];
            ofs2<<endl;
        }

        ofs2.close();        

    }

    endTime = clock();
    cout<<"Total TIME:"<<(double) (endTime - startTime) / CLOCKS_PER_SEC << "s"<<endl;

    return 0;
}

vector<int> GetRelativeEssenCubes(vector<vector<int>> cubelist, ofstream& ofs)
{
    vector<int> RelativeEssencol;
    for(int i=0; i<cubelist.size(); i++)
    {
        if(!URPCubeContain(cubelist, i, ofs))
        {   //cout<<'d'<<endl;
            RelativeEssencol.push_back(i);
        }
    }
    return RelativeEssencol;
    
}

vector<int> GetRedudantCubes(vector<vector<int>> cubelist, vector<int> RelativeEssencol, ofstream& ofs)
{
    unordered_map<int, int> RelativeEssencolmap;
    vector<vector<int>> cubelistnew;

    for(int i=0; i<RelativeEssencol.size(); i++)
    {
        RelativeEssencolmap[RelativeEssencol[i]] = 1;
        cubelistnew.push_back(cubelist[RelativeEssencol[i]]);
    }
    
    vector<int> Redundantcol;

    for(int i=0; i<cubelist.size(); i++)
    {
        if(RelativeEssencolmap.find(i) == RelativeEssencolmap.end())
        {
            cubelistnew.push_back(cubelist[i]);
            if(URPCubeContain(cubelistnew, cubelistnew.size()-1, ofs))
                Redundantcol.push_back(i);
            cubelistnew.pop_back();
        }
    }

    return Redundantcol;
}


bool URPCubeContain(vector<vector<int>> cubelist, int termid, ofstream& ofs)
{
    vector<vector<int>> cubelistnew;
    unordered_map<int, int> cubedeleteidmap;
    bool Containflag = false;
    for(int i=0; i<cubelist.size(); i++)
    {
        if(i==termid)
            continue;

        for(int j=0; j<cubelist[0].size(); j++)
        {
            if(cubelist[termid][j]==2 || cubelist[i][j] == 2)
                continue;
            else if(cubelist[termid][j] == 0)
            {
                if(cubelist[i][j] == 0)
                    cubelist[i][j] = 2;
                else
                {
                    if(cubedeleteidmap.find(i) == cubedeleteidmap.end())
                        cubedeleteidmap[i] = 1;
                }
            }
            else
            {
                if(cubelist[i][j]==1)
                    cubelist[i][j] = 2;
                else
                {
                    if(cubedeleteidmap.find(i) == cubedeleteidmap.end())
                        cubedeleteidmap[i] = 1;                    
                }
            }
            
        }
    }

    for(int i=0; i<cubelist.size(); i++)
    {
        if(i==termid)
            continue;
        if(cubedeleteidmap.find(i) == cubedeleteidmap.end())
            cubelistnew.push_back(cubelist[i]);
    }
    
    /*
    cout<<endl;
    for(int i=0; i<cubelistnew.size(); i++)
    {
        for(int j=0; j<cubelistnew[0].size(); j++)
            cout<<cubelistnew[i][j]<<' ';
        cout<<endl;
    }
    */

    unordered_map<int,int> selectflag;
    Containflag = URPTautologyCheck(cubelistnew, selectflag, ofs);
    //cout<<Containflag<<endl;
    //cout<<'d'<<endl;

    return Containflag;
}

bool URPTautologyCheck(vector<vector<int>> cubelist, unordered_map<int, int> selectflag, ofstream& ofs)
{
    if(cubelist.size() == 0)
        return false;

    if(JudgeAllDCCube(cubelist))
        return true;
    
    if(JudgeUnate(cubelist))
        return false;
    
    if(JudgeSingleVarPolar(cubelist))
        return true;

    if(cubelist.size()>0)
        ofs<<cubelist[0].size()<<endl;
    else
        ofs<<0<<endl;

    ofs<<cubelist.size()<<endl;

    for(int i=0; i<cubelist.size(); i++)
    {
        for(int j=0; j<cubelist[0].size(); j++)
            ofs<<cubelist[i][j];
        ofs<<endl;
    }

    vector<vector<int>> cubelistnew;
    cubelistnew = UnateReduction(cubelist);

    if(cubelistnew.size()>0)
        ofs<<cubelistnew[0].size()<<endl;
    else
        ofs<<0<<endl;

    ofs<<cubelistnew.size()<<endl;

    for(int i=0; i<cubelistnew.size(); i++)
    {
        for(int j=0; j<cubelistnew[0].size(); j++)
            ofs<<cubelistnew[i][j];
        ofs<<endl;
    }

    //cout<<'a'<<endl;
    int col_chosen = FindMostBinateVar(cubelistnew, selectflag);
    selectflag[col_chosen] = 1;
    //cout<<col_chosen<<endl;
    //cout<<'b'<<endl;
    
    vector<vector<int>> cubelistpos;
    vector<vector<int>> cubelistneg;
    cubelistpos = CubelistAssign(cubelistnew, col_chosen, 1);
    cubelistneg = CubelistAssign(cubelistnew, col_chosen, 0);

    //cout<<cubelistpos.size()<<endl;
    //cout<<cubelistneg.size()<<endl;

    return URPTautologyCheck(cubelistpos, selectflag, ofs) && URPTautologyCheck(cubelistneg, selectflag, ofs);
}

vector<vector<int>> UnateReduction(vector<vector<int>> cubelist)
{
    vector<vector<int>> cubelistnew;
    vector<int> xarray;
    vector<int> yunate;

    if(cubelist.size()==0)
        return cubelist;

    for(int i=0; i<cubelist[0].size(); i++)
    {
        vector<int> column;
        for(int j=0; j<cubelist.size(); j++)
            column.push_back(cubelist[j][i]);
        if(JudgeColumnUnate(column))
            yunate.push_back(i);
    }

    xarray = FindDCrows(cubelist, yunate);

    for(int i=0; i<xarray.size(); i++)
        cubelistnew.push_back(cubelist[xarray[i]]);
    
    return cubelistnew;
    
    //cubelist = cubelistnew;
    //cout<<cubelistnew.size()<<' '<<cubelistnew[0].size()<<endl;

}

vector<vector<int>> CubelistAssign(vector<vector<int>> cubelist, int col_chosen, int pos_flag)
{
    vector<vector<int>> cubelistnew;
    unordered_map<int, int> cubedeleteidmap;

    for(int i=0; i<cubelist.size(); i++)
    {
        if(cubelist[i][col_chosen] == 2)
            continue;
        else if(cubelist[i][col_chosen]==1)
        {
            if(pos_flag)
                cubelist[i][col_chosen] = 2;
            else
                cubedeleteidmap[i] = 1;
        }
        else
        {
            if(pos_flag)
                cubedeleteidmap[i] = 1;
            else
                cubelist[i][col_chosen] = 2;
        }
    }

    for(int i=0; i<cubelist.size(); i++)
    {
        if(cubedeleteidmap.find(i) == cubedeleteidmap.end())
            cubelistnew.push_back(cubelist[i]);
    }


    /*
    for(int i=0; i<cubelistnew.size(); i++)
    {
        for(int j=0; j<cubelistnew[i].size(); j++)
            cout<<cubelistnew[i][j]<<' ';
        cout<<endl;
    }

    cout<<endl;
    */
    

    return cubelistnew;
}

bool JudgeAllDCCube(vector<vector<int>> cubelist)
{
    bool flag_all_dc;
    
    for(int i=0; i<cubelist.size(); i++)
    {
        flag_all_dc = true;
        for(int j=0; j<cubelist[0].size(); j++)
        {
            if(cubelist[i][j] != 2)
            {
                flag_all_dc = false;
                break;
            }
        }

        if(flag_all_dc)
            return true;
    }

    return false;
}

vector<int> FindDCrows(vector<vector<int>> cubelist, vector<int> col)
{
    vector<int> xarray;
    bool flag_all_dc;

    for(int i=0; i<cubelist.size(); i++)
    {
        flag_all_dc = true;
        for(int j=0; j<col.size(); j++)
        {
            if(cubelist[i][col[j]]!=2)
            {
                flag_all_dc = false;
                break;
            }
        }

        if(flag_all_dc)
            xarray.push_back(i);
    }

    return xarray;
}

bool JudgeUnate(vector<vector<int>> cubelist)
{
    vector<int> cubelistColumn;
    for(int i=0; i<cubelist[0].size(); i++)
    {
        //cout<<i<<endl;
        cubelistColumn.clear();
        for(int j=0; j<cubelist.size(); j++)
            cubelistColumn.push_back(cubelist[j][i]);
        if(!JudgeColumnUnate(cubelistColumn))
            return false;
    }

    return true;

}

bool JudgeColumnUnate(vector<int> cubelistColumn)
{
    unordered_map<int, int> valuemap;
    vector<int> value;

    for(int i=0; i<cubelistColumn.size(); i++)
    {
        if(valuemap.find(cubelistColumn[i])==valuemap.end())
        {
            valuemap[cubelistColumn[i]] = 1;
            value.push_back(cubelistColumn[i]);
        }
    }

    if(value.size()<=1)
        return true;
    else if(value.size()>2)
        return false;
    else
    {
        if(value[0]==2 || value[1]==2)
            return true;
        else
            return false;
    }
}

bool JudgeSingleVarPolar(vector<vector<int>> cubelist)
{
    unordered_map<int, int> singlevarIDmap;

    int count_dc;
    int last_non_dc;

    for(int i=0; i<cubelist.size(); i++)
    {
        count_dc = 0;
        for(int j=0; j<cubelist[0].size(); j++)
        {
            if(cubelist[i][j]==2)
                count_dc++;
            else
                last_non_dc = j;
        }

        if(count_dc == cubelist[0].size()-1)
        {
            if(singlevarIDmap.find(last_non_dc)!=singlevarIDmap.end())
            {
                if(singlevarIDmap[last_non_dc]==1-cubelist[i][last_non_dc])
                    return true;
            }
            else
                singlevarIDmap[last_non_dc] = cubelist[i][last_non_dc];
        }
    }
    return false;
}

int FindMostBinateVar(vector<vector<int>> cubelist, unordered_map<int,int> selectflag)
{
    int num_zero;
    int num_one;
    int most_binate_metric = cubelist.size();
    int most_binate_col = 0;

    if(cubelist.size()==0)
        return -1;

    for(int i=0; i<cubelist[0].size(); i++)
    {
        if(selectflag.find(i)!=selectflag.end())
            continue;
        num_zero = 0;
        num_one = 0;
        for(int j=0; j<cubelist.size(); j++)
        {
            if(cubelist[j][i] == 0)
                num_zero++;
            else if(cubelist[j][i] == 1)
                num_one++;
        }

        if(abs(num_one-num_zero)<most_binate_metric)
        {
            most_binate_metric = abs(num_one-num_zero);
            most_binate_col = i;
        }
    }

    return most_binate_col;
}