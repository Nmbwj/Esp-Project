#include <Arduino.h>
#include<vector>
#include<algorithm>
#include<functional>


template<class T>
void printVector( String s, const std::vector<T>& v){
  
  Serial.print(s + " = (");
  if(v.size() == 0){
    Serial.print(") Size: " + String(v.size()) + " Capacity: " + String(v.capacity())+ "\n");
    return;
  }

  typename std::vector<T>::const_iterator i = v.begin();
  for( ; i != v.end()-1; i++)
  Serial.print(String(*i) + " " );
  Serial.print(String(*i)+ ") Size: " + String(v.size()) + " Capacity: " + String(v.capacity())+ "\n");
  
}

bool f1(int n){
  return n < 4;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  int a[] = {1,2,3,4,5};

  std::vector<int> v1; // v1 is empty, size = 0, capacity =0
  printVector("V1",v1);
  

  for(int j = 1; j<= 5; j++)
  v1.push_back(j); // v1 = (1 2 3 4 5), size = 5, capacity = 8
  printVector("V1",v1);

  std::vector<int> v2(3,7);
  printVector("V2",v2);

  std::vector<int>::iterator i1 = v1.begin()+1;
  std::vector<int> v3(i1, i1+2);
  printVector("V3",v3);

  std::vector<int> v4(v1);
  printVector("V4", v4);

  std::vector<int> v5(5);
  printVector("V5", v5);

  v5[1] = v5.at(3) = 9;
  printVector("V5", v5);

  v3.reserve(6);
  printVector("V3",v3);

  v4.resize(7);
  printVector("V4", v4);

  v4.resize(3);
  printVector("V4", v4);

  v4.clear();
  printVector("V4", v4);

  v4.insert(v4.end(), v3[1]);
  printVector("V4", v4);

  v4.insert(v4.end(), v3.at(1));
  printVector("V4", v4);

  v4.insert(v4.end(), 2,4);
  printVector("V4", v4);

  v4.insert(v4.end(), v1.begin()+1, v1.end()-1);
  printVector("V4", v4);

  v4.erase(v4.end()-2);
  printVector("V4", v4);

  v4.erase(v4.begin(), v4.begin()+4);
  printVector("V4", v4);

  v4.assign(3,8);
  printVector("V4", v4);

  v4.assign(a, a+3);
  printVector("V4", v4);

  std::vector<int>::reverse_iterator i3 = v4.rbegin();
  for( ; i3 != v4.rend(); i3++)
  Serial.print(String(*i3)+ " " );
  Serial.println();
  
  // Algorithim

  v5[0] = 3;
  printVector("V5", v5);
  replace_if(v5.begin(), v5.end(), f1, 7);
  printVector("V5", v5);
  v5[0] = 3; v5[2] = v5[4] = 0;
  printVector("V5", v5);
  replace(v5.begin(), v5.end(),0,7);
  printVector("V5", v5);
  sort(v5.begin(),v5.end());
  printVector("V5", v5);
  sort(v5.begin(), v5.end(), std::greater<int> ());
  printVector("V5", v5);
  v5.front() = 2;
  printVector("V5", v5);
  
}

void loop() {
  // put your main code here, to run repeatedly:


}
