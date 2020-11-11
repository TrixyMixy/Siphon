#include <iostream>
#include <thread>
#include <stdlib.h>
#include "./httplib.h"
#include "./picojson.h"
using namespace std;

const string version="v2"; //Program version.
const int nthread=8; //The amount of threads executed concurrently.
const string comp[64]={"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","0","1","2","3","4","5","6","7","8","9","-","_"};
const int compLen=sizeof(comp)/sizeof(comp[0]); 
bool urlFound=false;

string genID(){
  string id;
  for(int i=12;--i;)id+=comp[rand()%compLen];
  return id;
}

int a=0;
void getVid(){
  int stat=404;string url;
  httplib::Client cli("http://plist.trixter.repl.co");
  auto res=cli.Get("/");
  picojson::value plist;
  picojson::parse(plist,res->body);
  picojson::value::array& ipList=plist.get("ipList").get<picojson::array>();
  picojson::value::array& portList=plist.get("portList").get<picojson::array>();
  do{
    if(urlFound==true)return;
    if(stat==403){
      int num=rand()%100;
      char* host=const_cast<char*>(ipList[num].serialize().substr(1,ipList[num].serialize().length()-2).c_str());
      int port=stoi(portList[num].serialize().substr(1,portList[num].serialize().length()-2));
      cli.set_proxy(host, port);
    }
    try{
      ++a;
      httplib::Client cli("http://img.youtube.com");
      url="/vi/"+genID()+"/0.jpg";
      auto res=cli.Get(&url[0]);
      stat=res->status;
      if(urlFound!=true)cout<<url+" "<<res->status<<" "<<a<<"\n";
    }
    catch(exception& e){
    }
  }while(stat==404||stat==403);
  urlFound=true;
  cout<<"URL found: "+url+"\n";
}

int main(){
  srand(time(NULL));
  cout<<"Detected "<<nthread<<" threads.\n";
  vector<thread> threads;
  for(int i=0;i<nthread;++i){ 
    cout<<"Starting on thread "<<i+1<<".\n";
    threads.push_back(thread (getVid));
  }
  for(int i=0;i<nthread;++i)threads[i].join();
}
/*
Can perform 8000 requests per minute, 1 request every 7.5 ms.
Will take 320 days to generate one working url. 
With multithreading it can perform 20000 requests per minute with 8 active threads. 
With multithreading it will take 130 days to generate one working url.
*/