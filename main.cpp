#include <algorithm>
#include <arpa/inet.h>
#include <cstddef>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <signal.h>
#include <cstdlib>
#include <experimental/filesystem>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <chrono>

#include "json.hpp"
#include "sha256.h"
#include "bLibs/RandomX/src/randomx.h"

using namespace std;
using json = nlohmann::json;

namespace fs = std::experimental::filesystem;
using std::this_thread::sleep_for;


// Should Debug?
bool Debug = true;

// typedef signed char sbyte;
typedef unsigned char byte;

// print, with debug
template <typename T> void p(T AnyObj) {
  if (Debug)
    cout << AnyObj;
}
template <typename T> void pn(T AnyObj) {
  if (Debug)
    cout << AnyObj << endl;
}
template <typename T, typename U> void pf(T AnyObj, U AnyObj2) {
    cout << AnyObj << AnyObj2;
}
template <typename T, typename U> void pfn(T AnyObj, U AnyObj2) {
    cout << AnyObj << AnyObj2 << endl;
}

//  above is debug
//  bellow is normal

string logPath;
vector<string> MessageLogs;
int lastIndexCheck = 0;
bool tol = false;
bool ito = false;
class Logger{
  // log file
  static bool addMessageToLog(string inMes){
    fstream outfile;
    outfile.open(logPath, std::ios_base::app | std::ios_base::in);
    if(outfile.is_open()){
      outfile << "\n" << inMes;
    }
    outfile.close();
    return true;
  }
  static void LoggerLogFile(){
    constexpr int TIME_TO_SLEEP = 100;
    while(!tol){
      /*  // This works
      if(lastIndexCheck<MessageLogs.size()){
        
        int diffSize = MessageLogs.size()-lastIndexCheck;
        
        for(int i = 0; i<diffSize; i++){
          bool jw = addMessageToLog(MessageLogs.at(lastIndexCheck));
          lastIndexCheck = lastIndexCheck + 1;
        }
      }
*/
      
      if(MessageLogs.size()!=0){
        string message = MessageLogs.back();
        MessageLogs.pop_back();
        bool jw = addMessageToLog(message);
      }
      sleep_for(std::chrono::milliseconds(TIME_TO_SLEEP));
    }
    ito = true;
  }
  //
  void createLogFile(){

    // get current date and time
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    std::ostringstream ossd;
    oss << std::put_time(&tm, "%m-%d-%Y-%H:%M:%S");
    ossd << std::put_time(&tm, "%m/%d/%Y %H:%M:%S");
    string str = oss.str();
    string strd = ossd.str();

    // log file name
    string logFileName = string("Log_"+str+".txt");

    // Log file header
    string logHeader = "/*\nTime: "+strd+"\nLog File for My Miner!\n*/\n";

    // Make the file!
    struct stat st = {0};

    if (stat("logs", &st) == -1){
        mkdir("logs", 0700);
    }
    
    logPath = "logs/"+logFileName;
    
    ofstream outfile(logPath);
    outfile << logHeader;
    outfile.close();
    // created log file and header
    // 
    thread threadForLogFile(LoggerLogFile);
    threadForLogFile.detach();
  }
public:
  int severity = 0;
  Logger(int Severity){
    severity = Severity;
    // Create Log File
    createLogFile();
  }
  Logger(){
    severity = 4;
    // Create Log File
  }

  enum logLevels{
//  Main Log Levels
    NONE=0,
    FATEL=1,
    ERROR=2,
    WARNING=3,
    INFO=4,
    DEBUG=5,
//  Stratum Log Codes
    STRATUM_SUB=10,
    STRATUM_AUTH=11,
    STRATUM_SET_DIFF=12,
    STRATUM_NOTIFY=13,
    STRATUM_SHARE_ACCEPTED=14,
    STRATUM_SHARE_DENIED=15,
    STRATUM_MAIN=16,
//  Miner Log Codes
    MINER_MAIN=20,
    MINER_THREAD=21,
//  Miner Thread Log Codes
    MINER_THREAD_HASH=30,
    MINER_THREAD_FOUND_HASH=31,
    MINER_THREAD_HASHRATE=32
//  END
  };

  enum colorCodes{
//  Text Color
    TEXT_BLACK=30,
    TEXT_RED=31,
    TEXT_GREEN=32,
    TEXT_YELLOW=33,
    TEXT_BLUE=34,
    TEXT_MAGENTA=35,
    TEXT_CYAN=36,
    TEXT_WHITE=37,
//  Background Color
    BACK_BLACK=40,
    BACK_RED=41,
    BACK_GREEN=42,
    BACK_YELLOW=43,
    BACK_BLUE=44,
    BACK_MAGENTA=45,
    BACK_CYAN=46,
    BACK_WHITE=47,
//  Additional actions
    RESET=0,        // Resets everything
    BOLD=1,         // Makes it a bit brighter
    UNDERLINE=4,    // Underlines the text
    INVERSE=7,      // Swap forground and background colors
    OFF_BOLD=21,
    OFF_UNDERLINE=24,
    OFF_INVERSE=27
//  END
  };

  // Get Value Name
  string getStringFromCode(logLevels logOrCode){
    switch(logOrCode){
      case STRATUM_SUB:
        return "STRATUM_SUBSCRIBE";
      case STRATUM_AUTH:
        return "STRATUM_AUTHORIZE";
      case STRATUM_SET_DIFF:
        return "STRATUM_SET_DIFFICULTY";
      case STRATUM_NOTIFY:
        return "STRATUM_NOTIFY";
      case STRATUM_SHARE_ACCEPTED:
        return "STRATUM_SHARE_ACCEPTED";
      case STRATUM_SHARE_DENIED:
        return "STRATUM_SHARE_DENIED";
      case MINER_THREAD_HASH:
        return "MINER_THREAD_HASH";
      case MINER_THREAD_FOUND_HASH:
        return "MINER_THREAD_FOUND_HASH";
      case MINER_MAIN:
        return "MINER";
      case MINER_THREAD_HASHRATE:
        return "MINER_THREAD_HASHRATE";
      case NONE:
        return "NONE";
      case FATEL:
        return "FATEL";
      case ERROR:
        return "ERROR";
      case WARNING:
        return "WARNING";
      case INFO:
        return "INFO";
      case DEBUG:
        return "DEBUG";
      case STRATUM_MAIN:
        return "STRATUM";
      case MINER_THREAD:
        return "MINER_THREAD";
      default:
        return "UNKNOWN";
        break;
    }
  }

  // Add color to the text!
  string c(colorCodes colorCode){
    return "\033["+to_string(colorCode)+"m";
  }
  string cr(){
    return "\033[0m";
  }
  // Get Time!
  string getCurrentTime(){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%m/%d/%Y-%H:%M:%S");
    string str = oss.str();
    return str;
  }

  // Formated Message
  string formatedMessage(string serv, string Message){
    string fas = "["+getCurrentTime()+"]["+serv+cr()+"] ► "+Message;
    return fas;
  }
  string formatedMessage(string serv, string message_code, string Message){
    string fas = "["+getCurrentTime()+"]["+serv+cr()+"]["+message_code+"] ► "+Message;
    return fas;
  }
  string formatedMessage(string serv, string PreMess, string message_code, string Message){
    string fas = "["+getCurrentTime()+"]["+serv+cr()+"]["+message_code+"]["+PreMess+"] ► "+Message;
    return fas;
  }
  // Normal Log
  void print_fatel(string Mess){
    if(severity<FATEL) return;
    string cfm = c(TEXT_RED)+c(UNDERLINE);
    string fm = formatedMessage(cfm+"FATEL", Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  void print_error(string Mess){
    if(severity<ERROR) return;
    string cfm = c(TEXT_RED);
    string fm = formatedMessage(cfm+"ERROR", Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  void print_warning(string Mess){
    if(severity<WARNING) return;
    string cfm = c(TEXT_YELLOW);
    string fm = formatedMessage(cfm+"WARNING", Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  void print_info(string Mess){
    if(severity<INFO) return;
    string cfm = c(TEXT_WHITE);
    string fm = formatedMessage(cfm+"INFO", Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  void print_debug(string Mess){
    if(severity<DEBUG) return;
    string cfm = c(TEXT_CYAN);
    string fm = formatedMessage(cfm+"DEBUG", Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  // coded log
  void print_fatel(string Mess, logLevels code){
    if(severity<FATEL) return;
    string cfm = c(TEXT_RED)+c(UNDERLINE);
    string fm = formatedMessage(cfm+"FATEL", getStringFromCode(code), Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  void print_error(string Mess, logLevels code){
    if(severity<ERROR) return;
    string cfm = c(TEXT_RED);
    string fm = formatedMessage(cfm+"ERROR", getStringFromCode(code), Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  void print_warning(string Mess, logLevels code){
    if(severity<WARNING) return;
    string cfm = c(TEXT_YELLOW);
    string fm = formatedMessage(cfm+"WARNING", getStringFromCode(code), Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  void print_info(string Mess, logLevels code){
    if(severity<INFO) return;
    string cfm = c(TEXT_WHITE);
    string fm = formatedMessage(cfm+"INFO", getStringFromCode(code), Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  void print_debug(string Mess, logLevels code){
    if(severity<DEBUG) return;
    string cfm = c(TEXT_CYAN);
    string fm = formatedMessage(cfm+"DEBUG", getStringFromCode(code), Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
// For preMessages
  void print_fatel(string PreMess, string Mess, logLevels code){
    if(severity<FATEL) return;
    string cfm = c(TEXT_RED)+c(UNDERLINE);
    string fm = formatedMessage(cfm+"FATEL", PreMess, getStringFromCode(code), Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  void print_error(string PreMess, string Mess, logLevels code){
    if(severity<ERROR) return;
    string cfm = c(TEXT_RED);
    string fm = formatedMessage(cfm+"ERROR", PreMess, getStringFromCode(code), Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  void print_warning(string PreMess, string Mess, logLevels code){
    if(severity<WARNING) return;
    string cfm = c(TEXT_YELLOW);
    string fm = formatedMessage(cfm+"WARNING", PreMess, getStringFromCode(code), Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  void print_info(string PreMess, string Mess, logLevels code){
    if(severity<INFO) return;
    string cfm = c(TEXT_WHITE);
    string fm = formatedMessage(cfm+"INFO", PreMess, getStringFromCode(code), Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
  void print_debug(string PreMess, string Mess, logLevels code){
    if(severity<DEBUG) return;
    string cfm = c(TEXT_CYAN);
    string fm = formatedMessage(cfm+"DEBUG", PreMess, getStringFromCode(code), Mess);
    cout << fm << endl;
    MessageLogs.push_back(fm);
  }
//     print(messages.INFO, messages.STRATUM_SUB, "Stratum subed!")
  void print(logLevels logLevel, logLevels code, string preMessage, string Message){
    switch(logLevel){
      case FATEL:
        print_fatel(preMessage, Message, code);
        break;
      case ERROR:
        print_error(preMessage, Message, code);
        break;
      case WARNING:
        print_warning(preMessage, Message, code);
        break;
      case INFO:
        print_info(preMessage, Message, code);
        break;
      case DEBUG:
        print_debug(preMessage, Message, code);
        break;
    }
  }
  void print(logLevels logLevel, logLevels code, string Message){
    switch(logLevel){
      case FATEL:
        print_fatel(Message, code);
        break;
      case ERROR:
        print_error(Message, code);
        break;
      case WARNING:
        print_warning(Message, code);
        break;
      case INFO:
        print_info(Message, code);
        break;
      case DEBUG:
        print_debug(Message, code);
        break;
    }
  }
//     print(log.INFO, "Message");
  void print(logLevels logLevel, string Message){
    switch(logLevel){
      case FATEL:
        print_fatel(Message);
        break;
      case ERROR:
        print_error(Message);
        break;
      case WARNING:
        print_warning(Message);
        break;
      case INFO:
        print_info(Message);
        break;
      case DEBUG:
        print_debug(Message);
        break;
    }
  }
  bool close(){
    tol = true;
    while(!ito) sleep(1);
    return true;
  }
};

// algo
enum currentlySupportedAlgorithms{
  sha256Algo=0,
  randomXAlgo=1
};

currentlySupportedAlgorithms miningAlgorithm = sha256Algo; // defualt is sha-256


// Function used by more then one class
static string ReverseHexStringByFours(string inHexString){
  // Get HexIn and make a array of each 2 bits
  int inCount = inHexString.length()/2;
  string hexArray2Bits[inCount];
  for (int index = 0; index < inCount; index++){
      hexArray2Bits[index] = inHexString.substr(index * 2, 2);
  }
  string temp;
  for (int index = 0; index < inCount; index += 4)
  {
      temp = hexArray2Bits[index];
      hexArray2Bits[index] = hexArray2Bits[index + 3];
      hexArray2Bits[index + 3] = hexArray2Bits[index + 2];
      hexArray2Bits[index + 2] = hexArray2Bits[index + 1];
      hexArray2Bits[index + 1] = hexArray2Bits[index + 3];
      hexArray2Bits[index + 3] = temp;
  }
  string backToString;
  for(int i = 0; i < inCount; i++){
      backToString = backToString + hexArray2Bits[i];
  }
  return backToString;
}
void sleepSeconds(float seconds){
  clock_t startClock = clock();
  float secondsAhead = seconds * CLOCKS_PER_SEC;
  while(clock() < startClock+secondsAhead);
  return;
}

// Work Queue System
class workQueue {
private:
  vector<json> allWork;
  json lastGetLastestWork;
  string latestWorkID = "0000000";
public:
  int pendingQueueCount = 0;
  bool pendingQueueBool = false;
  void addWork(json newJsonObject) {
    switch(miningAlgorithm){
      case sha256Algo:
        allWork.push_back(newJsonObject); 
        latestWorkID = string(newJsonObject["params"][0]);
        break;
      case randomXAlgo:
        allWork.push_back(newJsonObject); 
        latestWorkID = string(newJsonObject["params"]["job_id"]);
        break;
    };
    pendingQueueCount++;
  }
  vector<json> getAllWork() { return allWork; }
  json getLatestWork() {
    lastGetLastestWork = allWork.back();
    return allWork.back();
  }
  bool newWork() {
    if (lastGetLastestWork != allWork.back())
      return true;
    return false;
  }
  bool checkNewWorkWithID(string workToCheckID){
    if(workToCheckID!=latestWorkID) return true;
    return false;
  }
} wQueue;

// Varibles for Stratum

int sock = 0, valread;
struct sockaddr_in serv_addr;
vector<json> reseviedJson;
bool continueLookingForMessages = true;

string extraNonce1;
int extraNonce2Size;
int difficulty;

/*
                              Add extraNonce1, extraNonceSize, and the
   difficalty.
*/

// for Stratum spefic methods

// mining.subscribe
json subJsonMes;
bool subMessage = false;
// mining.authorize
json authJsonMes;
bool autMessage = false;

// Messages
bool ifMessageReady = false;
bool isMessagesClose = false;

// Username
string authUsername;

// submit

int SharesAccepted;
int SharesRejected;
int TotalResults;


class Stratum {
  // string lastCheckedBuffer;
  Logger log;
public:
  // Logger
  Stratum(Logger tnl){
    log = tnl;
  }
  Stratum(){
    Logger ad(0);
    log = ad;
  }

  // UserInput

  // Getting target
  static string revereseArrayToString(string arr[], int size){
    vector<string> nvs;
    for(int i = 0; i < size; i++){
        nvs.push_back(arr[i]);
    }
    reverse(nvs.begin(),nvs.end());
    string retss;
    for(int i = 0; i < size; i++){
        retss = retss + nvs[i];
    }
    return retss;
}
static string intToHex(int nInt){
    if(nInt==0){
        return "00";
    }
    stringstream stream;
    stream << std::hex << nInt;
    string result( stream.str() );
    return result;
}
static string GenerateTargetFromDiff(int Diff){
    int ba[] = { 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int baLength = 32;
    int index = 0;
    int d = Diff;
    int n = ba[0];
    string result[baLength];
    while(true){
        int r = n / d;
        string hex = intToHex(r);
        result[index] = hex;
        int x = n - r * d;
        index++;
        if (index == baLength) break;
        n = (x << 8) + ba[index];
    }
    string revHex = revereseArrayToString(result, baLength);
    return revHex;
}


  // Functions for socket messages

  static void GotMessageNotify(json JsonMessage, Logger log) {
    log.print(log.INFO, log.STRATUM_NOTIFY, "Got Notify Message!");
    // Add extraNonce1, extraNonceSize, and the difficalty here

    string NewTarget;
    switch(miningAlgorithm){
      case sha256Algo:
        log.print(log.DEBUG, log.STRATUM_NOTIFY, "sha-256");
        // Target
        NewTarget = GenerateTargetFromDiff(difficulty);
    
        //log.print(log.DEBUG, log.STRATUM_NOTIFY, "");
        
        JsonMessage["jobData"]["extraNonce1"] = extraNonce1;
        JsonMessage["jobData"]["extraNonce2Size"] = extraNonce2Size;
        JsonMessage["jobData"]["difficulty"] = difficulty;
        JsonMessage["jobData"]["target"] = NewTarget;
        JsonMessage["jobData"]["previousHash"] = ReverseHexStringByFours(JsonMessage["params"][1]);
        JsonMessage["jobData"]["workerName"] = authUsername;
        break;
      case randomXAlgo:
        log.print(log.DEBUG, log.STRATUM_NOTIFY, "randomX");
        // Target
        NewTarget = JsonMessage["params"]["target"];
    
        //log.print(log.DEBUG, log.STRATUM_NOTIFY, "");
        
        JsonMessage["jobData"]["extraNonce1"] = extraNonce1;
        JsonMessage["jobData"]["extraNonce2Size"] = extraNonce2Size;
        JsonMessage["jobData"]["difficulty"] = difficulty;
        JsonMessage["jobData"]["target"] = NewTarget;
        JsonMessage["jobData"]["workerName"] = authUsername;
        break;
    };
    log.print(log.DEBUG, log.STRATUM_NOTIFY, "Target: "+NewTarget);
    log.print(log.DEBUG, log.STRATUM_NOTIFY, "Job: "+JsonMessage.dump(4));

    wQueue.addWork(JsonMessage);
  }
  static void GotMessageSet_difficulty(json JsonMessage, Logger log) {
    log.print(log.INFO, log.STRATUM_SET_DIFF, "Got Set Difficulty: "+to_string((int)JsonMessage["params"][0]));
    
    difficulty = (int)JsonMessage["params"][0];
  }
  static void GotMessageSubscribe(json JsonMessage, Logger log) {
    log.print(log.INFO, log.STRATUM_SUB, "Got Sub Message!");

    extraNonce1 = JsonMessage["result"][1];
    extraNonce2Size = (int)JsonMessage["result"][2];
    difficulty = (int)JsonMessage["result"][0][1][1];

    // change the mining.sub vars above
    subJsonMes = JsonMessage;
    subMessage = true;
  }
  static void GotAuthorize(json JsonMessage, Logger log) {
    log.print(log.INFO, log.STRATUM_AUTH, "Got Auth Message!");

    authJsonMes = JsonMessage;
    autMessage = true;
  }
  static void GotSubmit(json JsonMessage, Logger log){
    log.print(log.INFO, log.STRATUM_MAIN, "Got Submit Message!");
    TotalResults++;
    if((bool)JsonMessage["result"]){
      log.print(log.INFO, log.STRATUM_SHARE_ACCEPTED, "Share Accepted!");
      SharesAccepted++;
    }else{
      log.print(log.INFO, log.STRATUM_SHARE_DENIED, "Share Rejected: "+string(JsonMessage["error"][1]));
      SharesRejected++;
    }
    log.print(log.INFO, log.STRATUM_MAIN, "Current Results: "+log.c(log.TEXT_GREEN)+to_string(SharesAccepted)+log.cr()+"|"+log.c(log.BACK_BLUE)+to_string(TotalResults)+log.cr()+"|"+log.c(log.TEXT_RED)+to_string(SharesRejected)+log.cr());
  }
  static vector<json> getMessagesVectorJSON(char *inString) {
    vector<json> messagesInBuffer;
    std::stringstream ss(inString);
    std::string to;
    if (inString != NULL) {
      while (std::getline(ss, to, '\n')) {
        messagesInBuffer.push_back(json::parse(to));
      }
    }
    return messagesInBuffer;
  }
  static void lookingForNewMessage(Logger log) {
    log.print(log.INFO, log.STRATUM_MAIN, "Waiting for Messages!");
    ifMessageReady = true;
    while (continueLookingForMessages) {
      // p("Looking for message! ");
      // pn(tlc);
      char buffer[3072] = {0};
      valread = read(sock, buffer, 3072);
      vector<json> messagesInCurrentBuffer = getMessagesVectorJSON(buffer);
      //
      log.print(log.DEBUG, log.STRATUM_MAIN, "Looking for new messages");
      vector<json> npm;
      for (json MessagesInCurrentBuffer : messagesInCurrentBuffer) {
        log.print(log.DEBUG, log.STRATUM_MAIN, "New Loop");
        bool hasSameElement = false;
        for (json ReseviedJson : reseviedJson) {
          if (ReseviedJson == MessagesInCurrentBuffer){
            hasSameElement = true;
            log.print(log.DEBUG, log.STRATUM_MAIN, "Already found this message!");
          }
        }
        if (!hasSameElement){
          npm.push_back(MessagesInCurrentBuffer);
          log.print(log.DEBUG, log.STRATUM_MAIN, "Found new message: "+MessagesInCurrentBuffer.dump(4));
          
        }
      }
/*
what():  [json.exception.type_error.302] type must be number, but is object
is here

ID = null
*/
      for (json newMessages : npm) {
        //log.print(log.DEBUG, log.STRATUM_MAIN, log.c(log.TEXT_RED)+"newMessages: "+log.cr()+newMessages.dump(4));
        if (newMessages["id"] == 1) {
          GotMessageSubscribe(newMessages,log);
        } else if (newMessages["id"] == 2) {
          GotAuthorize(newMessages,log);
        } else if (newMessages["method"] == "mining.notify") {
          GotMessageNotify(newMessages,log);
        } else if (newMessages["method"] == "mining.set_difficulty") {
          GotMessageSet_difficulty(newMessages,log);
        } else if (newMessages["method"] == "job"){
          GotMessageNotify(newMessages,log);
        }
        reseviedJson.push_back(newMessages);
      }

      // Count Mess PRE
      // pn(buffer);
      /*
        pn("(PRE)reseviedJson size: "+to_string(reseviedJson.size()));
        pn("(PRE)messagesInCurrentBuffer size:
      "+to_string(messagesInCurrentBuffer.size()));

        while(reseviedJson.size() != messagesInCurrentBuffer.size()){
            //checking the message for humans
            //p("messagesInCurrentBuffer: ");
            //pn(messagesInCurrentBuffer[reseviedJson.size()].dump(4));

            // check that json befor putting it in resevied
            json curJsnMes = messagesInCurrentBuffer[reseviedJson.size()];
            if(curJsnMes["id"] == 1){
                GotMessageSubscribe(curJsnMes);
            }else if(curJsnMes["id"] == 2){
                GotAuthorize(curJsnMes);
            } else if(curJsnMes["method"] == "mining.notify"){
                GotMessageNotify(curJsnMes);
            }else if(curJsnMes["method"] == "mining.set_difficulty"){
                GotMessageSet_difficulty(curJsnMes);
            }
            // put the json in the reseviedJson array
            reseviedJson.push_back(messagesInCurrentBuffer[reseviedJson.size()]);



            pn("(POST)reseviedJson size: "+to_string(reseviedJson.size()));
            pn("(POST)messagesInCurrentBuffer size:
      "+to_string(messagesInCurrentBuffer.size()));
      }
*/
      // sleep(1);
    }
    // pn("Closed Messages thread!");
    isMessagesClose = true;
  }

  string getAuthUser(){
    return authUsername;
  }

  bool connectSock(string Host, int Port) {
    log.print(log.INFO, log.STRATUM_MAIN, "Tring to connect to socket...");
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      log.print(log.FATEL, log.STRATUM_MAIN, "Socket creation error");
      return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(Port);

    if (inet_pton(AF_INET, Host.c_str(), &serv_addr.sin_addr) <= 0) {
      log.print(log.FATEL, log.STRATUM_MAIN, "Invalid address/ Address not supported");
      return false;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      log.print(log.FATEL, log.STRATUM_MAIN, "Connection Failed");
      return false;
    }
    log.print(log.INFO, log.STRATUM_MAIN, "Succesfuly Connected to Socket!");

    // need new thread
    // lookingForNewMessage();
    thread t1(lookingForNewMessage, log);
    t1.detach();

    while (!ifMessageReady) {
      log.print(log.DEBUG, log.STRATUM_MAIN, "Waiting for Message Thread to respond");
      sleep(1);
    }
    return ifMessageReady;
  }
  bool sendJSON(string ID, string method, string para) {

    // JSON formated string, Includes ID, Method, and params
    std::string ss = std::string("{\"id\": ") + ID + ", \"method\": \"" + method + "\", \"params\": [" + para + "]}\n";

    log.print(log.DEBUG, log.STRATUM_MAIN, "Sending Json: "+json::parse(ss).dump(4));

    char buffer[3072] = {0};
    const char *jjsonmess = ss.c_str();
    send(sock, jjsonmess, strlen(jjsonmess), 0);
    // valread =read(sock, buffer, 3072);
    // read(sock, buffer, 3072);

    // Returns the direct responce of the message

    // p("sendJSON responce: ");
    // pn(buffer);
    // return buffer;

    // wanna check for the same ID                                            *
    return true;
  }
  bool subscribe() {
    log.print(log.INFO, log.STRATUM_MAIN, "Subscribing to Stratum's blockchain!");
    
    bool subMess = sendJSON("1", "mining.subscribe", " ");
    while (!subMessage)
      sleep(1);
    log.print(log.DEBUG, log.STRATUM_MAIN, "Subscribe Responce Message: "+subJsonMes.dump(4));
    return true;
  }
  bool authorize(string username, string password) {
    log.print(log.INFO, log.STRATUM_MAIN, "Authorizing Miner Credentiols Credentials!");
    string alc;
    string minStaticDiff;
    switch(miningAlgorithm){
      case sha256Algo:
        alc = "sha-256";
        minStaticDiff = "1024";
        break;
      case randomXAlgo:
        alc = "randomx";
        minStaticDiff = "256";
        break;
    };
    string newPass = string("a=")+alc+","+password+",d="+minStaticDiff;
    
    log.print(log.DEBUG, log.STRATUM_MAIN, "User: "+username+", Pass: "+newPass);
    
    string nas = string("\"") + username + "\", \"" + newPass + "\"";
    bool subMess = sendJSON("2", "mining.authorize", nas);
    while (!autMessage)
      sleep(1);
    log.print(log.DEBUG, log.STRATUM_MAIN, "Authorize Responce Message: "+authJsonMes.dump(4));

    authUsername = username;
    return true;
  }
  bool algorithm(currentlySupportedAlgorithms wantedAlgorithm){
    // make list of algorithms it can currentl support
    miningAlgorithm = wantedAlgorithm;
    return true;
  }

  bool submit(string workerName, string jobID, string extraNonce2, string nTime, string nonce){
    log.print(log.INFO, log.STRATUM_MAIN, "Sending Work!");

    //           "<workerName>, <jobID>, <extraNonce2>, <nTime>, <nonce>"
    string nas = string("\"")+workerName+"\", \""+jobID+"\", \""+extraNonce2+"\", \""+nTime+"\", \""+nonce+"\"";

    bool subMess = sendJSON("4", "mining.submit", nas);

    // Add function for ID 4
    

    return true;
  }
    
  bool close() {
    log.print(log.INFO, log.STRATUM_MAIN, "Closing Stratum Messages thread!");
    continueLookingForMessages = false;
    while (!isMessagesClose)
      sleep(1);
    log.print(log.INFO, log.STRATUM_MAIN, "Closed Stratum Messages!");
    return true;
  }
};

// Vars for miner
// unsigned int nthreads = std::thread::hardware_concurrency();
bool miningEnd = false;
int isAllThreadsShutdown = 0; // this should equal the number of cores, all the
                              // Mining threads will add 1 to this.
int ExtraNonce2 = 0;

int NNoNoice = 00000000;

int minerID = 0;

int NonceSize = 4;

int allHash = 0;


class Miner {
private:
  // Logger
  Logger log;
  Stratum stratum;

  static string sha256d(string inString) { 
    SHA256 sha256;
    return sha256(sha256(inString)); 
  }
  static string sha256(string inString){
    SHA256 sha256one;
    return sha256one(inString); 
  }

  static int stringHexToInt(string inHex){
    unsigned int i;
    std::istringstream iss(inHex);
    iss >> std::hex >> i;
    return i;
  }
  bool static isCurrentHashSmallerThenTheTarget(string inHash, string inTarget){
    int currentHexInt = stringHexToInt(inHash);
    int targetHexInt = stringHexToInt(inTarget);
    if(currentHexInt < targetHexInt) return true;
    return false;
  }

  static string intToXBytes(int inNonce, int Size){
    string currNOn = to_string(inNonce);
    while(currNOn.length() != Size){
      if(currNOn.length() < Size){
        currNOn = "0" + currNOn;
      }
      if(currNOn.length() > Size){
        currNOn.erase(0,1);
      }
    }
    return currNOn;
  }

  static string intToHex(int inInt){
    stringstream stream;
    stream << std::hex << inInt;
    string result( stream.str() );
    return result;
  }
  static string intToHexWithXBytes(int inInt, int Size){
    string intToHexString = intToHex(inInt);
    while(intToHexString.length() != Size){
      if(intToHexString.length() < Size){
        intToHexString = "0" + intToHexString;
      }
      if(intToHexString.length() > Size){
        intToHexString.erase(0,1);
      }
    }
    return intToHexString;
  }


  static void displayMinerInfo(Logger log){
    log.print(log.DEBUG, log.MINER_THREAD_HASHRATE, "Starting Miner Info, waiting 30 seconds.");
    sleep(30); // wait until everything is running
    
    // wanna get hash-rate
    while(!miningEnd){

      // Hash-rate

      int curHashCount = allHash;
      //sleepSeconds(1.0); // wait a second
      sleep(1);
      int nchr = allHash;
      log.print(log.INFO, log.MINER_THREAD_HASHRATE, "Hash-Rate: "+to_string(nchr-curHashCount)+"/h/s");
      log.print(log.INFO, log.STRATUM_MAIN, "Current share rate: "+log.c(log.TEXT_GREEN)+to_string(SharesAccepted)+log.cr()+"|"+log.c(log.BACK_BLUE)+to_string(TotalResults)+log.cr()+"|"+log.c(log.TEXT_RED)+to_string(SharesRejected)+log.cr());

        // wait 10 seconds
      sleep(30);
    }
  }
  // pn("Testing1");

// Functions for Mining
  static void doScrypt(json Work){
    string dataByte = ReverseHexStringByFours(Work["jobData"]["workData"]);
    string targetByte = Work["jobData"]["target"];


    
  }
  static string doSHA256(json work, string lTI, Logger log){
    
    string unit512Padding = "000000800000000000000000000000000000000000000000000000000000000000000000000000000000000080020000";

    int NonceSize = work["jobData"]["extraNonce2Size"];
    int hashCount = work["jobData"]["currentNonce"];
      
    string fullHeader = string(work["jobData"]["workData"]) + intToHexWithXBytes(hashCount, NonceSize*2) + unit512Padding;

    log.print(log.DEBUG, log.MINER_THREAD_HASH, lTI, "Full Job Data: "+fullHeader);
    log.print(log.DEBUG, log.MINER_THREAD, lTI, "Nonce(int): "+to_string(hashCount));
    log.print(log.DEBUG, log.MINER_THREAD_HASH, lTI, "Nonce(hex): "+intToHexWithXBytes(hashCount, NonceSize*2));

    string HashedHeader = sha256(fullHeader);
      //pfn("Header: ",fullHeader);

      log.print(log.DEBUG, log.MINER_THREAD_HASH, lTI, "Hashed: "+HashedHeader+"\n");

    return HashedHeader;
  }

  static string doRandomX(json work, string lTI, Logger log){
    // use the randomX algorithm to get hash
    
  }

  // thread function for the actull mining
  static void MinerThread(Logger log, Stratum stratum) {
    // This will be the ExtraNonce2, the miner ID.
    
    
    int thisMinerID = minerID+=1;
    string headerNonce2 = intToHexWithXBytes(thisMinerID, NonceSize*2);

    string lTI = to_string(thisMinerID);
    
    log.print(log.DEBUG, log.MINER_THREAD, lTI, "Miner Thread Started!");

    json currentJob;
    int ri = 0;

    int hashCount = 0;
    while (!miningEnd) {
      string workID = "1010101";
      if(ri!=0){
        workID = string(currentJob["params"][0]);
      }
      ri++;
      //pfn("Work ID: ", workID);
      if(wQueue.checkNewWorkWithID(workID)){
        if(!wQueue.getLatestWork()["params"][8] && ri<=1) continue;
        currentJob = wQueue.getLatestWork();


        switch(miningAlgorithm){
          case sha256Algo: {
            NonceSize = currentJob["jobData"]["extraNonce2Size"];

            log.print(log.DEBUG, log.MINER_THREAD, lTI, "Miner using worked found! ID: "+string(currentJob["params"][0]));

            // New work!
            
            // put header here
            string coinbase1 = string(currentJob["params"][2]);
            log.print(log.DEBUG, log.MINER_THREAD, lTI, "coinbase1: "+coinbase1);
            string extraNonce1 = string(currentJob["jobData"]["extraNonce1"]);
            log.print(log.DEBUG, log.MINER_THREAD, lTI, "extraNonce1: "+extraNonce1);
            string coinbase2 = currentJob["params"][3];
            log.print(log.DEBUG, log.MINER_THREAD, lTI, "coinbase2: "+coinbase2);
            string coinbase = coinbase1 + extraNonce1 + headerNonce2 + coinbase2;
            log.print(log.DEBUG, log.MINER_THREAD, lTI, "Nonce: "+headerNonce2);
            log.print(log.DEBUG, log.MINER_THREAD, lTI, "Generated Coinbase: "+coinbase);
            //pfn("coinbase: ",coinbase);
            
            string DHcoinbase = sha256d(coinbase);
            log.print(log.DEBUG, log.MINER_THREAD, lTI, "Hashed coinbase: "+DHcoinbase);
            int MerkelLenght = currentJob["params"][4].size();
            log.print(log.DEBUG, log.MINER_THREAD, lTI, "Size of merkle array: "+to_string(MerkelLenght));
            // merkleRoot proccess

            string mrts = DHcoinbase;
            string MerkleRoot;
            if(MerkelLenght!=0){
              for(int i = 0; i < MerkelLenght; i++){
                string nr = currentJob["params"][4][i];
                string btr = mrts + nr;
                mrts = sha256d(btr);
              }
              MerkleRoot = ReverseHexStringByFours(mrts);
              log.print(log.DEBUG, log.MINER_THREAD, lTI, "Merkle Root: "+MerkleRoot);
            }else{
              MerkleRoot = ReverseHexStringByFours(mrts);
            }
            string thisVersion  = currentJob["params"][5];
            string thisPrevHash = currentJob["params"][1];
            string thisNetTime  = currentJob["params"][7];
            string thisNetDiff  = currentJob["params"][6];
            
            string jobHeader = thisVersion + thisPrevHash + MerkleRoot + thisNetTime + thisNetDiff;

            log.print(log.DEBUG, log.MINER_THREAD, lTI, "Job Data: "+jobHeader);

            currentJob["jobData"]["workData"] = jobHeader;
            hashCount = 0;

            break;
          }
          case randomXAlgo:{
            // make a randomX header

            break;
          }
        };

        
      }
      currentJob["jobData"]["currentNonce"] = hashCount;

      string hashedString;
      switch(miningAlgorithm){
        case sha256Algo:
          hashedString = doSHA256(currentJob, lTI, log);
          break;
        case randomXAlgo:
          //hashedString = ;
          break;
      };

      bool doesWork = isCurrentHashSmallerThenTheTarget(hashedString, string(currentJob["jobData"]["target"]));


      if(doesWork){
        // Found Hash!
        // Send to Stratum
        log.print(log.INFO, log.MINER_THREAD_FOUND_HASH, lTI, "Found Hash: "+hashedString);
        log.print(log.INFO, log.MINER_THREAD_FOUND_HASH, lTI, "With Nonce: "+intToHexWithXBytes(hashCount, NonceSize*2));
        log.print(log.INFO, log.MINER_THREAD_FOUND_HASH, lTI, "Sending to Stratum!");
      stratum.submit(string(currentJob["jobData"]["workerName"]), string(currentJob["params"][0]), headerNonce2, string(currentJob["params"][7]), to_string(hashCount));
        }
      
      //pfn("is smaller? ", doesWork);
      
      // now check if it's smaller then the target
      // if so sumit, if not, resume

      
      

      // https://developer.bitcoin.org/reference/block_chain.html
      // first Scrypt (more documentation[https://github.com/ma261065/DotNetStratumMiner/blob/master/Miner.cs#L66])

      allHash++;
      
      hashCount++;
    }
    log.print(log.DEBUG, log.MINER_THREAD, lTI, "Shuting Down!");
    isAllThreadsShutdown += 1;
  }
  int threadCount = 0;
  bool startMining(int ctc) {
    // Will need to start miners with the number of threads
    threadCount = ctc;
    log.print(log.INFO, log.MINER_MAIN, "Starting mining, thread count: "+to_string(ctc));
    for (int i = 0; i < ctc; i++) {
      thread t1(MinerThread, log, stratum);
      t1.detach();
    }
    thread t2(displayMinerInfo, log);
    t2.detach();
    while(!miningEnd){
      // Keeps the program alive as it runs.
      sleep(1);
    }
    return false;
  }

public:
  Miner(Logger tnl, Stratum iss) {
    log = tnl;
    stratum = iss;
    /*
    pn("Miner made! going to get computer info to get ready to mine!");
    // Get system info
    //     - CPU core count
    //     -
    //     - if GPU (later)
    p("CPU core count: ");
    pn(thread::hardware_concurrency());
*/
  }
  Miner(){
    // For just getting the class
  }
  bool start() {
    // start mining here
    return startMining(thread::hardware_concurrency());
  }
  bool start(int ntc) { 
    return startMining(ntc);
  }
  bool close() {
    log.print(log.INFO, log.MINER_MAIN, "Closing Miner!");
    miningEnd = true;
    while(isAllThreadsShutdown != threadCount){
      log.print(log.DEBUG, log.MINER_MAIN, "Waiting for all threads to stop");
      sleep(1);
    }
    log.print(log.INFO, log.MINER_MAIN, "Miner Closed!");
    return true;
  }
};

bool dsd = false;
void close(Stratum stratum, Miner miner, Logger log) {
  log.print(log.INFO, "Closing Everything Now!");
  if (miner.close() && stratum.close()) {
    log.print(log.INFO, "Everything is shutdown, stoping program!");
    // stopes Logging to file
    log.close();
    dsd = true;
  }
}

void CLIinput(Logger log, Stratum stratum, Miner miner){
  while(true){
    string command;
    cin >> command;

    if(command=="stop"){
      close(stratum,miner,log);
    }
    
  }
}

long getTime(){
  return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

void benchmarkingForRandomX(){
  // first get the time
  long long startTime = getTime();

  // then do the benchmarking

  // varibles to hash
  const char myKey[] = "0123456789abcdef";
  const char myInput[] = "0123456789abcdefg";
  

  // make the VM with different parameters
  // - flags
  // - different dataset thread counts (1, 2, 4, 8, 16, 32, 64, 128, 256)
  int datasetThreadCounts[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  int datasetThreadCountsSize = sizeof(datasetThreadCounts)/sizeof(int);
  // - different hash thread counts (1, 2, 4, 8, 16, 32, 64, 128, 256)
  int hashThreadCounts[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  int hashThreadCountsSize = sizeof(hashThreadCounts)/sizeof(int);
  // - different flage ( RANDOMX_FLAG_DEFAULT, RANDOMX_FLAG_LARGE_PAGES, RANDOMX_FLAG_HARD_AES, RANDOMX_FLAG_FULL_MEM, RANDOMX_FLAG_JIT, RANDOMX_FLAG_SECURE, RANDOMX_FLAG_ARGON2_SSSE3, RANDOMX_FLAG_ARGON2_AVX2, RANDOMX_FLAG_ARGON2 )
  //randomx_flags flags[] = {RANDOMX_FLAG_DEFAULT, RANDOMX_FLAG_LARGE_PAGES, RANDOMX_FLAG_HARD_AES, RANDOMX_FLAG_FULL_MEM, RANDOMX_FLAG_JIT, RANDOMX_FLAG_SECURE, RANDOMX_FLAG_ARGON2_SSSE3, RANDOMX_FLAG_ARGON2_AVX2, RANDOMX_FLAG_ARGON2};

  // test every combination of flags and thread counts for each dataset and hash
  // going to take some time to complete

  // https://www.onlinegdb.com/online_c++_compiler
  // https://stackoverflow.com/questions/9430568/generating-combinations-in-c




}

// MAIN
int main() { 

  // testing for randomx
  const char myKey[] = "0123456789abcdef";
  const char myInput[] = "0123456789abcdefg";
  char hash[RANDOMX_HASH_SIZE];

  randomx_flags flags = randomx_get_flags();
  flags |= RANDOMX_FLAG_JIT;
  flags |= RANDOMX_FLAG_FULL_MEM;
  flags |= RANDOMX_FLAG_HARD_AES;
  flags |= RANDOMX_FLAG_ARGON2_SSSE3;

  pn("Got flags");

  randomx_cache* cache = RANDOMX_EXPORT::randomx_alloc_cache(flags);
  RANDOMX_EXPORT::randomx_init_cache(cache, myKey, sizeof(myKey));

  pn("Got cache");

  RANDOMX_EXPORT::randomx_dataset *dataset = RANDOMX_EXPORT::randomx_alloc_dataset(flags);

  pn("Got dataset");

  unsigned long datasetItemCount = RANDOMX_EXPORT::randomx_dataset_item_count();
  std::thread t1DS(&randomx_init_dataset, dataset, cache, 0, datasetItemCount / 2);
  std::thread t2DS(&randomx_init_dataset, dataset, cache, datasetItemCount / 2, datasetItemCount - datasetItemCount / 2);
  t1DS.join();
  t2DS.join();

  pn("Got datasetItems");

  randomx_vm *vm = RANDOMX_EXPORT::randomx_create_vm(flags, cache, dataset);
  //RANDOMX_EXPORT::randomx_release_cache(cache);
  //RANDOMX_EXPORT::randomx_release_dataset(dataset);

  pn("Got vm");
  pn("Starting to hash");
  RANDOMX_EXPORT::randomx_calculate_hash(vm, myInput, sizeof(myInput), hash);

  for (unsigned i = 0; i < RANDOMX_HASH_SIZE; ++i)
    std::cout << std::hex << std::setw(2) << std::setfill('0') << ((int)hash[i] & 0xff);

  std::cout << std::endl; 

  // end testing for randomx
  // benchmarking for randomx

  // to set dataset threads
  // https://github.com/tevador/RandomX/blob/master/src/tests/benchmark.cpp#L312

  pn("Benchmarking RandomX");
  pn("This will take a while");

  

  return 0;

  
  // byte ba[] = { 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  // 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  Logger log(5);
  Stratum stratum(log);
  Miner miner(log, stratum);

  // for any interups
  thread t1(CLIinput, log, stratum, miner);
  t1.detach();

  // cout << stratum.connectSock("50.220.121.209", 3333) << endl; // proHashing
  // cout << stratum.connectSock("144.126.135.34", 7033); // binance

  // if(stratum.connectSock("50.220.121.209", 3333)){
  //    stratum.subscribe();
  //}
  stratum.connectSock("50.220.121.209", 3359);
  stratum.algorithm(randomXAlgo);
  stratum.subscribe();
  stratum.authorize("rowisamine", "n=001,o=CPPtesting");

  //
  //miner.start(1);

  // Everything is connected and ready to start mining
  // We need to make a miner now,
  // I was thinking of making a differnt class that uses thoes methods
  //    - Cause its in the same file it can easly call the Work Queue
  //    -

  while(!dsd) sleep(1);
  return 0;
}

/*
#include <hash-library/sha256.h>y
#include <nlohmann/json.hpp>

p();

https://braiins.com/stratum-v1/docs
https://github.com/ma261065/DotNetStratumMiner

*/
