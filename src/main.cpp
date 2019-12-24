#include "GarrysMod/Lua/Interface.h"
#include <ctime>
#include <iostream>
#include <thread>
#include <exception>
#include <atomic>

unsigned int build = 0;
std::atomic<std::time_t> srvrtime (0);
std::atomic<bool> flag (true);

void foo() 
{
	unsigned short int timeout = 0;
    while(flag){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if(srvrtime >= (std::time(nullptr))-2){
    		if (timeout != 0){
				timeout = 0;
    			std::cout << "Server caught back up!\n";
    		}
        }else{
            if(srvrtime != 0){
                timeout++;
                std::cout << "Server is behind! (" << timeout << ")\n";
                if(timeout == 60){
                    std::cout << "Server Frozen! killing process...\n";
					throw std::exception();
                }
            }
        }
    }
}
std::thread t1(foo);
LUA_FUNCTION( WatchDogPing )
{
	srvrtime = std::time(nullptr);
    return 0;
}
LUA_FUNCTION( WatchDogStop )
{
	flag = false;
	t1.join();
    return 0;
}
GMOD_MODULE_OPEN()
{
    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB );// Push global table
    LUA->PushString( "WatchDogPing" );
    LUA->PushCFunction(WatchDogPing);
    LUA->SetTable( -3 );
    LUA->PushString( "WatchDogStop" );
    LUA->PushCFunction(WatchDogStop);
    LUA->SetTable( -3 );
	LUA->PushString( "antifreeze" );
	LUA->CreateTable();
	LUA->PushString( "version" );
	LUA->PushNumber( 1 );
	LUA->SetTable( -3 );
	LUA->PushString( "build" );
	LUA->PushNumber( build );
	LUA->SetTable( -3 );
	LUA->SetTable( -3 );
	LUA->Pop(1);
    return 0;
}
GMOD_MODULE_CLOSE()
{
	flag = false;
	t1.join();
    return 0;
}
