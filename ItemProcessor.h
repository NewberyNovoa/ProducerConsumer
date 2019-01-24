#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <fstream>
#include <list>
#include <vector>
#include <chrono>

using namespace std;



class ItemProcessor{
  public:
    ItemProcessor();
    ItemProcessor(const string &inputFilePath, const string &outputFilePath, const string &sortMethod ){
      this-> inputFilePath = inputFilePath;
      this-> outputFilePath = outputFilePath;
      this-> sortMethod = sortMethod;
    }
    //~ItemProcessor();

    bool start();
  
  protected:
    bool openFiles();
    bool produce();
    bool process(list<string>::iterator &);
    void closeFiles();
    static void insertionSort(vector<int> &);
    static void bubbleSort(vector<int> &);
    vector<int> build_vector(string &);
    void writeFile(vector <int> &v);
    
  
  private:
    string inputFilePath, outputFilePath;   //path files
  	list<string> itemsList;                 //Buffer
  	mutex mtxRead, mtxWrite;                // mutex for critical sections
    ifstream inputFile;                     //Files
    ofstream outputFile;
    string sortMethod;                      //input for sortMethod
};

//Check files
bool ItemProcessor::openFiles(){

inputFile.open(inputFilePath);
outputFile.open (outputFilePath);

if(inputFile.fail() || outputFile.fail())
  return false;
else 
  return true;

}

//Close files
void ItemProcessor::closeFiles(){
  inputFile.close();
  outputFile.close();
}

//Produce data to buffer
bool ItemProcessor::produce(){

  string item;
  
  while(!inputFile.eof())     //storing the input in memory for best performance
  {
    getline(inputFile,item);
    itemsList.push_back(item);
  }

  if(itemsList.size() > 0)
    return true;
  else
    return false;

}

//Buld vector with no blanks
vector<int> ItemProcessor::build_vector(string &line)
{
           //Unlock the mutex (locked in line 80) inorder to another thread can build another vector at same time
  cout<<"BuildingVector "<<endl;
  vector<int> vectorNoBlanks;
  
  for(char& c : line)
  {
    if(c != ' ')
    {
      vectorNoBlanks.push_back(c - '0');
    }
    else if (c == ' ')
    {
      this_thread::sleep_for (chrono::seconds(1));   //Blank space found, this thread must wait 1 sec
    }
  } 

  return vectorNoBlanks;

}

//insertionSort
void ItemProcessor::insertionSort(vector<int> &vectorNoBlanks){

  cout<<"Sorting "<<endl;
    int i, j, tmp;

    for (i = 1; i < (int)vectorNoBlanks.size(); i++) 
    {
        j = i;
        while (j > 0 && vectorNoBlanks[j - 1] > vectorNoBlanks[j])
         {
            tmp = vectorNoBlanks[j];
            vectorNoBlanks[j] = vectorNoBlanks[j - 1];
            vectorNoBlanks[j - 1] = tmp;
            j--;
        }
    }
}

//Bubblesort
void ItemProcessor::bubbleSort(vector<int> &vectorNoBlanks) 
{ 
   int i, j, tmp; 
   bool swapped; 
   for (i = 0; i < (int)vectorNoBlanks.size()-1; i++) 
   { 
     swapped = false; 
     for (j = 0; j < (int)vectorNoBlanks.size()-i-1; j++) 
     { 
        if (vectorNoBlanks[j] > vectorNoBlanks[j+1]) 
        { 
          tmp = vectorNoBlanks[j];
          vectorNoBlanks[j]=vectorNoBlanks[j+1];
          vectorNoBlanks[j+1] = tmp; 
          swapped = true; 
        } 
     } 
  
     // IF no two elements were swapped by inner loop, then break 
     if (swapped == false) 
        break; 
   } 
}

//Write on file the processed item
void ItemProcessor::writeFile (vector <int> &v)
{
  
  string lineSorted;
  

   //Build a string of the processed item
  for (vector<int>::iterator it=v.begin(); it != v.end(); ++it)
  {
    
    lineSorted+= to_string(*it) + ",";
    //outputFile << *it << ","; 
    
  }

  mtxWrite.lock(); // critical section (exclusive access to the outputfile signaled by locking mtxWrite):
  outputFile<<lineSorted;//Writing in the outputFile the sorted vector and separeted by comas
  cout<<lineSorted<<endl;
  outputFile << '\n';//Adding end line after wiriting the line
  cout<<"Writing in file "<<endl;
  mtxWrite.unlock(); //unloking the write file inorder to get another trhead acces to the file

}

//Consumer porcess for items
bool ItemProcessor::process(list<string>::iterator &it)
{
  cout<<"ProcessingLine "<<endl;
  mtxRead.lock();   
  while (it != itemsList.end())   //while are items to proces in buffer
  {             
    
    string item = *it;// store the item in a temp variable in order to gabe acces to the information to anther thread
    
   // mtxRead.lock();                //Lock the mutex 
    it++;  
    //mtxRead.unlock();
    mtxRead.unlock();                        // next item to proces
    vector<int> vectorNoBlanks = build_vector(item); // build the vector

    if(sortMethod == "bubbleSort")
      bubbleSort(vectorNoBlanks);  //sort the vector
    else
      insertionSort(vectorNoBlanks);

    writeFile(vectorNoBlanks); // write in file the vector  

  }

  return true;
}

bool ItemProcessor::start(){

    bool couldOpen = openFiles();
    bool wasFileRead = produce();
    bool isOK = false;

    if(couldOpen && wasFileRead)
      isOK = true;

    
    if (itemsList.empty())
        return false;

    list<string>::iterator it=itemsList.begin();
    
  thread workerThread1 (&ItemProcessor::process ,this ,ref(it));
  thread workerThread2 (&ItemProcessor::process ,this ,ref(it));
  thread workerThread3 (&ItemProcessor::process ,this ,ref(it));
  thread workerThread4 (&ItemProcessor::process ,this ,ref(it));
   
//join al threads to the mainthread
  workerThread1.join();
  workerThread2.join();
  workerThread3.join();
  workerThread4.join();

  closeFiles();

  return isOK;
}
