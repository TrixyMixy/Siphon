#include <iostream>
#include <thread>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include "./httplib.h"
#include "./picojson.h"
using namespace std;

const string version="v2"; //Program version.
vector<string> compa;
int compLen=64;
bool allFound=false;
int nthread=8; //The amount of threads executed concurrently.
int IDLength, urlCount;
string baseUrl, prePath, postPath;

string genID(){
  string id;
  for(int i=IDLength+1;--i;)id+=compa[rand()%compLen];
  return id;
}

int a=0,b=0;
void getVid(){
  int stat=404;string url;
  httplib::Client cli("http://plist.trixter.repl.co");
  auto res=cli.Get("/");
  picojson::value plist;
  picojson::parse(plist,res->body);
  picojson::value::array& ipList=plist.get("ipList").get<picojson::array>();
  picojson::value::array& portList=plist.get("portList").get<picojson::array>();
  do{
    if(compa.size()<compLen){}else{
    if(allFound==true)return;
    try{
      ++a;
      httplib::Client cli(&baseUrl[0]);
      if(stat==403){ // 403? Turn on Proxy.
        //cli.set_connection_timeout(500);
        int num=rand()%100;
        char* host=const_cast<char*>(ipList[num].serialize().substr(1,ipList[num].serialize().length()-2).c_str());
        int port=stoi(portList[num].serialize().substr(1,portList[num].serialize().length()-2));
        cli.set_proxy(host,port);
        cout<<"Proxy Set to "<<host<<":"<<port<<"\n";
      }
      url=prePath+genID()+postPath;
      //if(a==5000)url=prePath+"qVXlIaos37s"+postPath;
      auto res=cli.Get(&url[0]);
      stat=res->status;
      if(stat==404||stat==403)/*cout<<url+" "<<stat<<" "<<a<<"\n"*/;else if(allFound==false){b++;cout<<"URL found: "+baseUrl+url+" Code: "<<stat<<"\n";};
      if(b>=urlCount){allFound=true;}
    }
    catch(exception& e){
    }}
  }while(allFound!=true);
  cout<<"Thread Stopped.\n";

}

int main(){
  ifstream fileT;
  fileT.open("./config/config.json", ios::binary);
  picojson::value config;
  stringstream configVal;
  configVal<<fileT.rdbuf();
  fileT.close();
  picojson::parse(config,configVal);
  baseUrl=config.get("url").get<string>();
  prePath=config.get("prePath").get<string>();
  postPath=config.get("postPath").get<string>();
  nthread=stoi(config.get("threadCount").get<string>());
  IDLength=stoi(config.get("IDLength").get<string>());
  urlCount=stoi(config.get("urlCount").get<string>());
  picojson::array comp=config.get("IDCharList").get<picojson::array>();
  compLen=comp.size();
  for(int i=0;i<compLen;++i)  {
    compa.push_back(comp[i].serialize().substr(1,1));
  }
  srand(time(NULL));
  cout<<"Detected "<<nthread<<" threads.\n";
  vector<thread> threads;
  for(int i=0;i<nthread;++i){ 
    cout<<"Starting on thread "<<i+1<<".\n";
    threads.push_back(thread (getVid));
  }
  for(int i=0;i<nthread;++i)threads[i].join();
}
