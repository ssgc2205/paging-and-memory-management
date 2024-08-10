#include <bits/stdc++.h>
#include <fstream>
using namespace std;
vector <int> mm;
vector <int> vm;
vector <int> mmm;
//ifstream inp;
ofstream opp;
class process
{
    public :
        int pid;
        int size;
        int pgsize;
        vector <int> ptm;
        vector <int> ptv;
        map <int,int> mpp;
        string name;
};

int counter_for_run = 0;
queue <int> qp;
map <int, process> mpm;
map <int, process> mpv;
int curr_msize;
int curr_vsize;
int msize;
int vsize;
int psize;
int tim = 0;

void LOAD(int cnt, string str)
{
    process prc;
    prc.name = str;
    prc.pid = cnt;
    ifstream file;
    file.open(str);
    if(!file.is_open())
    {
        opp << str << " could not be loaded - file does not exist" << endl;
        return;
    }
    string num;
    //int num;
    file >> num;
    //int n = num;
    //cout << n << endl;
    int n = stoi(num);
    prc.size = n;
    int con = n * 1024;
    con = con / psize;
    prc.pgsize = con;
    if(con <= curr_msize)
    {
        curr_msize -= con;
        for(int i = 0; i < msize; i++)
        {
            if(mm[i] == 0)
            {
                con--;
                mm[i] = cnt;
                prc.ptm.push_back(i);
            }
            if(con == 0)
            {
                break;
            }
        }
        mpm[cnt] = prc;
        opp << str << " is loaded in main memory and is assigned process id " << prc.pid << endl;
    }
    else if(con <= curr_vsize)
    {
        curr_vsize -= con;
        for(int i = 0; i < vsize; i++)
        {
            if(vm[i] == 0)
            {
                con--;
                vm[i] = cnt;
                prc.ptv.push_back(i);
            }
            if(con == 0)
            {
                break;
            }
        }
        mpv[cnt] = prc;
        opp << str << " is loaded in virtual memory and is assigned process id " << prc.pid << endl;
    }
    else
    {
        opp << str << " could not be loaded - memory is full" << endl;
    }
    file.close();
    return;
}


void KILL(int p)
{
    if(mpm.count(p))
    {
        for(int i = 0; i < msize; i++)
        {
            if(mm[i] == mpm[p].pid)
            {
                curr_msize--;
                mm[i] = 0;
            }
        }
        mpm.erase(p);
        opp << "successfully killed the process " << p << endl;
    }
    else if(mpv.count(p))
    {
        for(int i = 0; i < vsize; i++)
        {
            if(vm[i] == mpv[p].pid)
            {
                curr_vsize--;
                vm[i] = 0;
            }
        }
        mpv.erase(p);
        opp << "successfully killed the process " << p << endl;
    }
    else
    {
        opp << "No such pid exists to kill" << endl;
    }
    return;
}

void SWAPOUT(int p)
{
    if(mpm.count(p))
    {
        int n = mpm[p].ptm.size();
        if(n > curr_vsize)
        {
            opp << "Error in swapping out process " << p << " as there is no sufficient space in virtual memory" << endl;
        }
        else
        {
            curr_msize += n;
            curr_vsize -= n;
            for(int i = 0; i < msize; i++)
            {
                if(mm[i] == mpm[p].pid)
                {
                    mm[i] = 0;
                }
            }
            for(auto i : mpm[p].mpp)
            {
                int m = i.first % psize;
                int n = i.first / psize;
                process pr1 = mpm[p];
                mmm[psize * (pr1.ptm[n]) + m] = 0;
            }
            mpm[p].ptm.clear();
            for(int i = 0; i < vsize; i++)
            {
                if(vm[i] == 0)
                {
                    vm[i] = mpm[p].pid;
                    mpm[p].ptv.push_back(i);
                    n--;
                }
                if(n == 0)
                {
                    break;
                }
            }
            mpm[p].mpp.clear();
            mpv[p] = mpm[p];
            mpm.erase(p);
            opp << "successfully swapped out process " << p << endl;
        }
    }
    else
    {
        opp << "Error in swapping out " << p << " as there is no such pid exists" << endl;
    }
    return;
}

void SWAPIN(int p)
{
    if(mpv.count(p))
    {
        int n = mpv[p].pgsize;
        //cout << "n : " << n << endl;
        // n = n * 1024;
        // n = n / psize;
        if(n <= curr_msize)
        {
            int num = n;
            mpv[p].ptv.clear();
            for(int i = 0; i < msize; i++)
            {
                if(mm[i] == 0)
                {
                    mm[i] = mpv[p].pid;
                    mpv[p].ptm.push_back(i);
                    num--;
                }
                if(num == 0)
                {
                    break;
                }
            }
            for(int i = 0; i < vsize; i++)
            {
                if(vm[i] == mpv[p].pid)
                {
                    vm[i] = 0;
                }
            }
            curr_msize -= n;
            curr_vsize += n;
            mpm[p] = mpv[p];
            mpv.erase(p);
            opp << "successfully swapped in pid " << p << endl;
            for(auto i : mpm[p].mpp)
            {
                int m = i.first % psize;
                int n = i.first / psize;
                process pr1 = mpm[p];
                mmm[psize * (pr1.ptm[n]) + m] = i.second;
            }
            //cout << "hello " << endl;
        }
        else
        {
            int rem = n - curr_msize;
            //cout << rem << endl;
            while(!qp.empty())
            {
                int lru = qp.front();
                //cout << lru << endl;
                if(rem > 0)
                {
                    if(curr_vsize >= mpm[lru].pgsize)
                    {
                        SWAPOUT(lru);
                        rem -= mpv[lru].pgsize;
                        qp.pop();
                    }
                    else
                    {
                        opp << "error swapin pid " << p << " no sufficient space in virtual memory while using lru" << endl;
                        return;
                    }
                }
                if(rem <= 0)
                {
                    int num = n;
                    mpv[p].ptv.clear();
                    for(int i = 0; i < msize; i++)
                    {
                        if(mm[i] == 0)
                        {
                            mm[i] = mpv[p].pid;
                            mpv[p].ptm.push_back(i);
                            num--;
                        }
                        if(num == 0)
                        {
                            break;
                        }
                    }
                    for(int i = 0; i < vsize; i++)
                    {
                        if(vm[i] == mpv[p].pid)
                        {
                            vm[i] = 0;
                        }
                    }
                    curr_msize -= n;
                    curr_vsize += n;
                    mpm[p] = mpv[p];
                    //cout << mpm[p].pid << endl;
                    mpv.erase(p);
                    opp << "successfully swapped in pid " << p << endl;
                    for(auto i : mpm[p].mpp)
                    {
                        int m = i.first % psize;
                        int n = i.first / psize;
                        process pr1 = mpm[p];
                        mmm[psize * (pr1.ptm[n]) + m] = i.second;
                    }
                }
            }
            //cout << rem << endl;
            if(rem > 0)
            {
                for(auto i : mpm)
                {
                    process pp = i.second;
                    if(pp.size <= curr_vsize)
                    {
                        if(rem > 0)
                        {
                            SWAPOUT(i.first);
                            rem -= pp.pgsize;
                        }
                        else
                        {
                            int num = n;
                            mpv[p].ptv.clear();
                            for(int i = 0; i < msize; i++)
                            {
                                if(mm[i] == 0)
                                {
                                    mm[i] = mpv[p].pid;
                                    mpv[p].ptm.push_back(i);
                                    num--;
                                }
                                if(num == 0)
                                {
                                    break;
                                }
                            }
                            for(int i = 0; i < vsize; i++)
                            {
                                if(vm[i] == mpv[p].pid)
                                {
                                    vm[i] = 0;
                                }
                            }
                            curr_msize -= n;
                            curr_vsize += n;
                            mpm[p] = mpv[p];
                            mpv.erase(p);
                            opp << "successfully swapped in pid " << p << endl;
                            for(auto i : mpm[p].mpp)
                            {
                                int m = i.first % psize;
                                int n = i.first / psize;
                                process pr1 = mpm[p];
                                mmm[psize * (pr1.ptm[n]) + m] = i.second;
                            }
                        }
                    }
                    else
                    {
                        opp << "Error in swapin pid " << p << " no sufficient place in virtual memory even after using lru" << endl;
                        return;
                    }
                }
            }
        }
    }
    else
    {
        opp << "Error swapin no such pid " << p << " in virtual memory" << endl;
    }
    return;
}

void RUN1(int p)
{
    if(!mpm.count(p))
    {
        counter_for_run = 1;
        opp << "error in run cause there is no such pid " << p << " in main memory" << endl;
        return;
    }
    process pr = mpm[p];
    long long maxi = mpm[p].size;
    //cout << "maxi : " << maxi << endl;
    maxi = maxi * 1024;
    //map <int, int> mp;
    ifstream inp;
    inp.open(pr.name);
    string str;
    //cout << "pid : " << p << endl;
    while(inp >> str)
    {
        if(str == "load")
        {
            string n1, n2;
            inp >> n1;
            inp >> n2;
            if(stoi(n2) >= maxi)
            {
                opp << "Invaid Memory Address " << stoi(n2) << " specified for process id " << p << endl;
            }
            else
            {
                mpm[p].mpp[stoi(n2)] = stoi(n1);
                opp << "Command : load " << stoi(n1) << ", " << stoi(n2) << "; Result : Value of " << stoi(n1) << " is now stores in addr " << stoi(n2) << endl;
            }
        }
        else if(str == "add")
        {
            string n1, n2, n3;
            inp >> n1;
            inp >> n2;
            inp >> n3;
            if(stoi(n1) >= maxi)
            {
                opp << "Invaid Memory Address " << stoi(n1) << " specified for process id " << p << endl;
            }
            else if(stoi(n2) >= maxi)
            {
                opp << "Invaid Memory Address " << stoi(n2) << " specified for process id " << p << endl;
            }
            else if(stoi(n3) >= maxi)
            {
                opp << "Invaid Memory Address " << stoi(n3) << " specified for process id " << p << endl;
            }
            else
            {
                //if(mp.count(stoi(n1)) && mp.count(stoi(n2)))
                {
                    mpm[p].mpp[stoi(n3)] = mpm[p].mpp[stoi(n1)] + mpm[p].mpp[stoi(n2)];
                    opp << "command : add " << stoi(n1) << " " << stoi(n2) << " " << stoi(n3) << "; Result : Value in addr " << stoi(n1) << " = " << mpm[p].mpp[stoi(n1)] << " addr " << stoi(n2) << " = " << mpm[p].mpp[stoi(n2)] << " addr " << stoi(n3) << " = " << mpm[p].mpp[stoi(n3)] << endl;
                    //cout << mp[stoi(n3)] << endl;
                }
            }
        }
        else if(str == "sub")
        {
            string n1, n2, n3;
            inp >> n1;
            inp >> n2;
            inp >> n3;
            if(stoi(n1) >= maxi)
            {
                opp << "Invaid Memory Address " << stoi(n1) << " specified for process id " << p << endl;
            }
            else if(stoi(n2) >= maxi)
            {
                opp << "Invaid Memory Address " << stoi(n2) << " specified for process id " << p << endl;
            }
            else if(stoi(n3) >= maxi)
            {
                opp << "Invaid Memory Address " << stoi(n3) << " specified for process id " << p << endl;
            }
            else
            {
                //if(mp.count(stoi(n1)) && mp.count(stoi(n2)))
                {
                    mpm[p].mpp[stoi(n3)] = mpm[p].mpp[stoi(n1)] - mpm[p].mpp[stoi(n2)];
                    opp << "command : sub " << stoi(n1) << " " << stoi(n2) << " " << stoi(n3) << "; Result : Value in addr " << stoi(n1) << " = " << mpm[p].mpp[stoi(n1)] << " addr " << stoi(n2) << " = " << mpm[p].mpp[stoi(n2)] << " addr " << stoi(n3) << " = " << mpm[p].mpp[stoi(n3)] << endl;
                    //cout << mp[stoi(n3)] << endl;
                }
            }
        }
        else if(str == "print")
        {
            string n1;
            inp >> n1;
            if(stoi(n1) >= maxi)
            {
                opp << "Invaid Memory Address " << stoi(n1) << " specified for process id " << p << endl;
            }
            else
            {
                //if(mp.count(stoi(n1)))
                {
                    //cout << mp[stoi(n1)] << endl;
                    opp << "Command : print " << stoi(n1) << "; Result : Value in addr " << stoi(n1) << " = " << mpm[p].mpp[stoi(n1)] << endl;
                }
            }
        }
    }
    for(auto i : mpm[p].mpp)
    {
        int m = i.first % psize;
        int n = i.first / psize;
        process pr1 = mpm[p];
        mmm[psize * (pr1.ptm[n]) + m] = i.second;
    }
    opp << "Run successful for process id " << p << endl;
    return;
}

void RUN(int p)
{
    if(mpm.count(p))
    {
        RUN1(p);
        queue <int> qpb;
        while(!qp.empty())
        {
            int pd = qp.front();
            if(pd != p)
            {
                qpb.push(pd);
            }
            qp.pop();
        }
        qp = qpb;
        qp.push(p);
    }
    else if(mpv.count(p))
    {
        SWAPIN(p);
        counter_for_run = 0;
        RUN1(p);
        if(counter_for_run == 0)
        {
            queue <int> qpb;
            while(!qp.empty())
            {
                int pd = qp.front();
                if(pd != p)
                {
                    qpb.push(pd);
                }
                qp.pop();
            }
            qp = qpb;
            qp.push(p);
        }
        else
        {
            opp << "Run failed for pid " << p << " as there is no such process id in main memory" << endl;
        }
    }
    else
    {
        opp << "Error in run cause there exists no such pid " << p << " in both virtual and main memory" << endl;
    }
    return;
}

void LISTPR()
{
    for(auto i : mpm)
    {
        opp << "Main Memory : " << i.first << endl;
    }
    for(auto i : mpv)
    {
        opp << "Virtual Memory : " << i.first << endl;
    }
    opp << "Successful implemented listpr" << endl;
    return;
}

void PTE(int p, string name)
{
    ofstream oo(name, ios::app);
    oo << "Time : " << tim << endl;
    if(mpm.count(p))
    {
        process pr = mpm[p];
        for(int i = 0; i < pr.ptm.size(); i++)
        {
            oo << "Main Memory : PID : " << p << " logical pagenumber : " << i << " physical pagenumber : " << pr.ptm[i] << endl;
        }
    }
    else if(mpv.count(p))
    {
        process pr = mpv[p];
        for(int i = 0; i < pr.ptv.size(); i++)
        {
            oo << "Virtual Memory : PID : " << p << " logical pagenumber : " << i << " physical pagenumber : " << pr.ptv[i] << endl;
        }
    }
    else
    {
        oo << "Error in pte as there exists no such pid : " << p << endl;
    }
    oo.close();
    return;
}

void PTEALL(string name)
{
    ofstream oo(name, ios::app);
    oo << "Time : " << tim << endl;
    for(auto i : mpm)
    {
        process pr = i.second;
        for(int ii = 0; ii < pr.ptm.size(); ii++)
        {
            oo << "Main Memory : PID : " << i.first << " logical pagenumber : " << ii << " physical pagenumber : " << pr.ptm[ii] << endl;
        }
    }
    for(auto i : mpv)
    {
        process pr = i.second;
        for(int ii = 0; ii < pr.ptv.size(); ii++)
        {
            oo << "Virtual Memory : PID : " << i.first << " logical pagenumber : " << ii << " physical pagenumber : " << pr.ptv[ii] << endl;
        }
    }
    oo << "Successfully implemented pteall" << endl;
    oo.close();
    return;
}

void PRINT(int meml, int length)
{
    for(int i = meml; i < meml + length; i++)
    {
        opp << "Value of " << i << " : " << mmm[i] << endl;
    }
    opp << "Successfully implemented print system command" << endl;
    return;
}

int main(int argc, char* argv[])
{
    if(argc != 11)
    {
        cout << "Doesn't match number of arguments" << endl;
        return 0;
    }
    //cout << argv[2] << endl;
    msize = atoi(argv[2]);
    long long byt = msize;
    byt = byt * 1024;
    vsize = atoi(argv[4]);
    psize = atoi(argv[6]);
    msize = (msize * 1024) / psize;
    vsize = (vsize * 1024) / psize;
    vector <int> un(msize, 0);
    vector <int> us(vsize, 0);
    vector <int> sh(byt, 0);
    mm = un;
    vm = us;
    mmm = sh;
    curr_msize = msize;
    curr_vsize = vsize;
    ifstream inp;
    inp.open(argv[8]);
    opp.open(argv[10]);
    string str;
    int count = 0;
    int ctr = 0;
    while(inp >> str)
    {
        if(str == "load")
        {
            tim++;
            ctr = 1;
        }
        else if(str == "listpr")
        {
            tim++;
            LISTPR();
        }
        else if(str == "run")
        {
            tim++;
            ctr = 5;
        }
        else if(str == "swapin")
        {
            tim++;
            ctr = 4;
        }
        else if(str == "kill")
        {
            tim++;
            ctr = 2;
        }
        else if(str == "swapout")
        {
            tim++;
            ctr = 3;
        }
        else if(str == "pte")
        {
            tim++;
            string s1, s2;
            inp >> s1;
            inp >> s2;
            //cout << s2 << endl;
            PTE(stoi(s1), s2);
        }
        else if(str == "pteall")
        {
            tim++;
            string s1;
            inp >> s1;
            PTEALL(s1);
        }
        else if(str == "exit")
        {
            tim++;
            mm.clear();
            vm.clear();
            mmm.clear();
            un.clear();
            us.clear();
            sh.clear();
            mpm.clear();
            mpv.clear();
            opp << "exit" << endl;
            exit(0);
        }
        else if(str == "print")
        {
            tim++;
            string meml, len;
            inp >> meml;
            inp >> len;
            PRINT(stoi(meml), stoi(len));
        }
        else if(ctr == 5)
        {
            int p = stoi(str);
            RUN(p);
        }
        else if(ctr == 4)
        {
            int p = stoi(str);
            SWAPIN(p);
        }
        else if(ctr == 3)
        {
            int p;
            //cout << str << endl;
            p = stoi(str);
            SWAPOUT(p);
        }
        else if(ctr == 2)
        {
            int num;
            //cout << str << endl;
            num = stoi(str);
            KILL(num);
        }
        else if(ctr == 1)
        {
            count++;
            //cout << "count " << count << endl;
            LOAD(count, str);
        }

    }
    opp.close();
    inp.close();
    return 0;
}