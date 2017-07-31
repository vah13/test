```
#include <windows.h>
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
			if (
					!isalnum(chunk[i]) 
										&& 
					!isprint(chunk[i])
				)
			{
				//if (chunk[i]!= (int)'*' || chunk[i]!=(int)'/' || chunk[i]!=(int)'\\' || chunk[i]!=(int)'!' || chunk[i]!=(int)'@' || chunk[i]!=(int)'#' || chunk[i]!=(int)'$' || chunk[i]!=0)
					return false;
			}
		}
		for (i = 0;i<bytesRead;i+=2)
		{
				cout<< chunk[i];
		}
		cout <<endl;
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

void __main(char end_magic_data[])
{
		const char start_magic_data[] = "\x00\x88"; 
  		//char end_magic_data[]  = "";= _end_magic_data; //"\x00\x00\x00\x20\x00\x00\x00"; 
		
		int PID = 920;	
		std::cout << "Local data address: " << (void*)start_magic_data << "\n";
		list<int> start_address = GetAddressOfData(PID, start_magic_data, 2);	
		cout<<"start end magic search "<<'\n';	
		list<int> end_address = GetAddressOfData(PID, end_magic_data,  16);
		int key = 0;
		int count = 0;
		
		start_address.reverse();
		for (std::list<int>::const_iterator iterator = start_address.begin(), end = start_address.end(); iterator != end; ++iterator) 
		{
			int i = *iterator;
						key = i;	
						for (int j=key;j<key+33;j++){
							std::list<int>::iterator it = std::find(end_address.begin(), end_address.end(), j);
							if ( it != end_address.end() )
								{
										if (readMemory(PID,(char*)(void*)(i+2),j-i-2))
										{	
              								//cout<<"***********"<<endl<<intToHexString(i)<<endl;
											//cout<<intToHexString(j)<<endl<<"***********"<<endl;
											count++;
											break;
										}
								}
						}
		}

	cout<<count;
}
int main()
{
	char _end_magic_data1[] = "\x00\x00\x00\x20\x00\x00\x00";
	__main(_end_magic_data1);

	char type = '\0';
	while( PromptForChar( "Try 00x00x00x00x00x00x00 magic data? [y/n]", type ) )
		{
			if (type == 'y')
			{
					char _end_magic_data2[] = "\x00\x00\x00\x00\x00\x00\x00";
					__main(_end_magic_data2);			
			}
			if (type== 'n')
			{
					cout << ":-)";
					return 0;
			}
		}
    return 0;	


}
```
