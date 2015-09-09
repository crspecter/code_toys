 #include <iostream> 
   #include <list> 
    
   using namespace std; 
    
   int main() { 
       int total = 0; 
       cin >> total; 
    
      int number = 0; 
      cin >> number; 
   
      list<int> table; 
   
      for(int i = 1; i <= total; ++i) { 
          table.push_back(i);     
      } 
      int shout = 1;   
      int last = 0; 
   
      list<int>::iterator it = table.begin(); 
	  list<int>::iterator next;
	  list<int>::iterator del;
	  while(table.size() != 1)
	  {
		  for(int i=0; i < number; i++)
		  {
			  if(it == table.end())
			  	it = table.begin();
			  del = it;
			  ++it;
		  }
		  table.erase(del);
		  if(it == table.end())
		  	it = table.begin();
		  next = it;
		  it = next;
	  }

   
      cout << *table.begin() << endl; 
  }  


